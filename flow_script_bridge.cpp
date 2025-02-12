#include "flow_script_bridge.hpp"
#include "flow_script_built_in_node_interface.hpp"


String FlowScriptBridge::ERR_STR_NO_AVAILABLE_FIBERS = "Cannot exceed limit of " + itos(FlowScriptConstants::FIBERS_MAX) + " active fibers.";


void FlowScriptBridge::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_flow_script", "flow_script"), &FlowScriptBridge::set_flow_script);
	ClassDB::bind_method(D_METHOD("get_flow_script"), &FlowScriptBridge::get_flow_script);
	ClassDB::bind_method(D_METHOD("get_built_in_node_interface"), &FlowScriptBridge::get_built_in_node_interface);
	ClassDB::bind_method(D_METHOD("execute_branch_with_finish_callback", "initial_node_id", "finish_callback"), &FlowScriptBridge::execute_branch_with_finish_callback);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "flow_script", PROPERTY_HINT_RESOURCE_TYPE, "FlowScript", PROPERTY_USAGE_DEFAULT, "FlowScript"), "set_flow_script", "get_flow_script");

	GDVIRTUAL_BIND(_create_built_in_node_interface);
}


void FlowScriptBridge::set_flow_script(const Ref<FlowScript> &p_flow_script)
{
	ERR_FAIL_COND_MSG(is_active(), RTR("Cannot change the FlowScript during execution."));
	if (flow_script == p_flow_script)
	{
		return;
	}
	flow_script = p_flow_script;
}


Ref<FlowScript> FlowScriptBridge::get_flow_script() const
{
	return flow_script;
}


FlowScript *FlowScriptBridge::get_flow_script_ptr() const
{
	return flow_script.ptr();
}


void FlowScriptBridge::set_state(const Dictionary &p_state)
{
	if (p_state.has("fibers"))
	{
		Dictionary fiber_state_map = p_state["fibers"];
		for (const Variant &key : fiber_state_map.keys())
		{
			if (key.get_type() != Variant::INT)
				continue;
			FlowScriptExecutionFiberID fiber_id = key;
			Dictionary fiber_state = fiber_state_map[key];
			fiber_list[fiber_id].set_state(fiber_state);
		}
	}
	update_cache_next_free_fiber_id();
}


Dictionary FlowScriptBridge::get_state() const
{
	Dictionary r_state;

	Dictionary fiber_state_map;
	for (const FlowScriptNodeContext &fiber : fiber_list)
	{
		if (!fiber.is_active())
			continue;
		Dictionary fiber_state;
		fiber.get_state(fiber_state);
		fiber_state_map[fiber.self_id] = fiber_state;
	}
	r_state["fibers"] = fiber_state_map;

	return r_state;
}


int FlowScriptBridge::get_active_fiber_count() const
{
	int ret = 0;
	for (const FlowScriptNodeContext &fiber : fiber_list)
	{
		if (fiber.is_active())
		{
			ret++;
		}
	}
	return ret;
}


bool FlowScriptBridge::is_active() const
{
	for (const FlowScriptNodeContext &fiber : fiber_list)
	{
		if (fiber.is_active())
			return true;
	}
	return false;
}


FlowScriptBuiltInNodeInterface *FlowScriptBridge::create_built_in_node_interface()
{
	return nullptr;
}


FlowScriptBuiltInNodeInterface *FlowScriptBridge::get_built_in_node_interface()
{
	if (built_in_node_interface == nullptr)
	{
		GDVIRTUAL_CALL(_create_built_in_node_interface, built_in_node_interface);
		if (built_in_node_interface == nullptr)
		{
			built_in_node_interface = create_built_in_node_interface();
		}
		if (built_in_node_interface == nullptr)
		{
			ERR_PRINT("Failed to create the built-in node interface; a dummy will be used instead. Please override _create_built_in_node_interface and return a valid extension of FlowScriptBuiltInNodeInterface.");
			built_in_node_interface = memnew(FlowScriptBuiltInNodeInterface);
		}
	}
	return built_in_node_interface;
}


bool FlowScriptBridge::can_create_fiber() const
{
	return cache_next_free_fiber_id != FlowScriptConstants::FIBER_ID_INVALID;
}


void FlowScriptBridge::execute_branch_with_finish_callback(const FlowScriptNodeID p_initial_node_id, const Callable &p_finish_callback)
{
	ERR_FAIL_COND(!flow_script.is_valid());
	ERR_FAIL_COND(!can_create_fiber());

	update_cache_next_free_fiber_id();

	fiber_list[cache_next_free_fiber_id].finished_callback = p_finish_callback;
	fiber_list[cache_next_free_fiber_id].start(flow_script, p_initial_node_id);
}


bool FlowScriptBridge::has_fiber(const FlowScriptExecutionFiberID p_fiber_id) const
{
	return p_fiber_id > -1 && p_fiber_id < fiber_list.size() && fiber_list[p_fiber_id].is_active();
}


void FlowScriptBridge::internal_fiber_finish(const FlowScriptExecutionFiberID p_fiber_id)
{
	cache_next_free_fiber_id = p_fiber_id;

	for (FlowScriptExecutionFiberID curr_iter_id = 0; curr_iter_id < fiber_list.size(); curr_iter_id++)
	{
		if (curr_iter_id != p_fiber_id && fiber_list[curr_iter_id].is_active() && fiber_list[p_fiber_id].awaiting_fibers_bits & (1 << p_fiber_id))
		{
			fiber_list[curr_iter_id].awaiting_fibers_bits = fiber_list[curr_iter_id].awaiting_fibers_bits &~ (1 << p_fiber_id);
			if (fiber_list[curr_iter_id].awaiting_fibers_bits == 0)
			{
				fiber_list[curr_iter_id].invoke_step();
			}
		}
	}

	Callable finish_callback = fiber_list[p_fiber_id].finished_callback;
	FlowScriptNodeContext::VariableSlot result_variable = fiber_list[p_fiber_id].return_variable;

	fiber_list[p_fiber_id].reset();

	if (finish_callback.is_valid())
	{
		Callable::CallError call_err;
		Variant dummy_ret_pass;

		if (result_variable.used)
		{
			const Variant **argptr = (const Variant **)alloca(sizeof(Variant *));
			argptr[0] = &result_variable.value;
			finish_callback.callp(argptr, 0, dummy_ret_pass, call_err);
		}
		else
		{
			finish_callback.callp(nullptr, 0, dummy_ret_pass, call_err);
		}
		ERR_FAIL_COND_MSG(call_err.error != Callable::CallError::CALL_OK, "Failed to call finish callback for fiber " + itos(p_fiber_id) + "; Error code: " + itos(call_err.error));
	}
}


FlowScriptExecutionFiberID FlowScriptBridge::internal_init_branch(const FlowScriptNodeReference &p_node_reference)
{
	ERR_FAIL_COND_V(!can_create_fiber(), FlowScriptConstants::FIBER_ID_INVALID);

	Ref<FlowScript> target_flow_script;

	if (p_node_reference.include_id != FlowScriptConstants::INCLUDE_ID_INVALID)
	{
		ERR_FAIL_COND_V(!flow_script->has_include_instance(p_node_reference.include_id), FlowScriptConstants::FIBER_ID_INVALID);
		target_flow_script = flow_script->get_include_flow_script(p_node_reference.include_id);
	}
	else
	{
		target_flow_script = flow_script;
	}

	ERR_FAIL_COND_V(!target_flow_script->has_node(p_node_reference.node_id), FlowScriptConstants::FIBER_ID_INVALID);

	FlowScriptExecutionFiberID new_fiber_id = cache_next_free_fiber_id;
	if (fiber_list[new_fiber_id].prepare_for_execution(target_flow_script, p_node_reference.node_id))
	{
		update_cache_next_free_fiber_id();
		return new_fiber_id;
	}
	else
	{
		ERR_FAIL_V(FlowScriptConstants::FIBER_ID_INVALID);
	}
}


bool FlowScriptBridge::internal_exec_branch(const FlowScriptExecutionFiberID p_fiber_id)
{
	ERR_FAIL_INDEX_V(p_fiber_id, fiber_list.size(), false);
	ERR_FAIL_COND_V(!fiber_list[p_fiber_id].is_active(), false);
	fiber_list[p_fiber_id].execute_current_node();
	return true;
}


void FlowScriptBridge::update_cache_next_free_fiber_id()
{
	for (FlowScriptExecutionFiberID curr_id = 0; curr_id < fiber_list.size(); curr_id++)
	{
		if (!fiber_list[curr_id].is_active())
		{
			cache_next_free_fiber_id = curr_id;
			return;
		}
	}
	cache_next_free_fiber_id = FlowScriptConstants::FIBER_ID_INVALID;
}


FlowScriptBridge::FlowScriptBridge()
{
	for (FlowScriptExecutionFiberID curr_id = 0; curr_id < fiber_list.size(); curr_id++)
	{
		fiber_list[curr_id].self_id = curr_id;
		fiber_list[curr_id].bridge_ptr = this;
	}
}


FlowScriptBridge::~FlowScriptBridge()
{
}
