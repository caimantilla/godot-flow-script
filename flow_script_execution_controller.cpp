#include "flow_script_execution_controller.hpp"
#include "flow_script.hpp"
#include "flow_script_bridge.hpp"


String FlowScriptExecutionController::ERR_STR_NO_AVAILABLE_FIBERS = "Cannot exceed limit of " + itos(FlowScriptExecutionController::FIBERS_MAX) + " active fibers.";


void FlowScriptExecutionController::_bind_methods()
{
	BIND_CONSTANT(FIBER_ID_INVALID);
	BIND_CONSTANT(FIBERS_MAX);

	ClassDB::bind_method(D_METHOD("set_flow_script", "flow_script"), &FlowScriptExecutionController::set_flow_script);
	ClassDB::bind_method(D_METHOD("get_flow_script"), &FlowScriptExecutionController::get_flow_script);
	ClassDB::bind_method(D_METHOD("set_bridge", "bridge"), &FlowScriptExecutionController::set_bridge);
	ClassDB::bind_method(D_METHOD("get_bridge"), &FlowScriptExecutionController::get_bridge);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "flow_script", PROPERTY_HINT_RESOURCE_TYPE, "FlowScript", PROPERTY_USAGE_DEFAULT, "FlowScript"), "set_flow_script", "get_flow_script");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "bridge", PROPERTY_HINT_NODE_TYPE, "FlowScriptBridge", PROPERTY_USAGE_DEFAULT, "FlowScriptBridge"), "set_bridge", "get_bridge");
}


void FlowScriptExecutionController::set_flow_script(const Ref<FlowScript> &p_flow_script)
{
	if (flow_script == p_flow_script)
		return;
	ERR_FAIL_COND(is_active());
	flow_script = p_flow_script;
}


Ref<FlowScript> FlowScriptExecutionController::get_flow_script() const
{
	return flow_script;
}


FlowScript *FlowScriptExecutionController::get_flow_script_ptr() const
{
	return flow_script.ptr();
}


void FlowScriptExecutionController::set_bridge(FlowScriptBridge *p_bridge)
{
	if (bridge == p_bridge)
		return;
	ERR_FAIL_COND(is_active());
	if (bridge != nullptr)
		bridge->dependency_count--;
	bridge = p_bridge;
	if (bridge != nullptr)
		bridge->dependency_count++;
}


FlowScriptBridge *FlowScriptExecutionController::get_bridge() const
{
	return bridge;
}


void FlowScriptExecutionController::set_state(const Dictionary &p_state)
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


Dictionary FlowScriptExecutionController::get_state() const
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


int FlowScriptExecutionController::get_active_fiber_count() const
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


bool FlowScriptExecutionController::is_active() const
{
	for (const FlowScriptNodeContext &fiber : fiber_list)
	{
		if (fiber.is_active())
			return true;
	}
	return false;
}


bool FlowScriptExecutionController::can_create_fiber() const
{
	return cache_next_free_fiber_id != FIBER_ID_INVALID;
}


void FlowScriptExecutionController::execute_branch_with_finish_callback(const FlowScriptNodeID p_initial_node_id, const Callable &p_finish_callback)
{
	ERR_FAIL_NULL(bridge);
	ERR_FAIL_COND(!flow_script.is_valid());
	ERR_FAIL_COND(!can_create_fiber());

	update_cache_next_free_fiber_id();

	fiber_list[cache_next_free_fiber_id].finished_callback = p_finish_callback;
	fiber_list[cache_next_free_fiber_id].start(flow_script, p_initial_node_id);
}


bool FlowScriptExecutionController::has_fiber(const FlowScriptExecutionFiberID p_fiber_id) const
{
	return p_fiber_id > 0 && p_fiber_id < FIBERS_MAX && fiber_list[p_fiber_id].is_active();
}


void FlowScriptExecutionController::internal_fiber_finish(const FlowScriptExecutionFiberID p_fiber_id)
{
	cache_next_free_fiber_id = p_fiber_id;

	for (FlowScriptExecutionFiberID curr_iter_id = 0; curr_iter_id < FIBERS_MAX; curr_iter_id++)
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


FlowScriptExecutionFiberID FlowScriptExecutionController::internal_init_branch(const FlowScriptNodeReference &p_node_reference)
{
	ERR_FAIL_COND_V(!can_create_fiber(), FIBER_ID_INVALID);
	Ref<FlowScript> target_flow_script;
	if (p_node_reference.flow_script_id != FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID)
	{
		ERR_FAIL_COND_V(!flow_script->has_include_flow_script_instance(p_node_reference.flow_script_id), FIBER_ID_INVALID);
		target_flow_script = flow_script->get_include_flow_script(p_node_reference.flow_script_id);
	}
	else
	{
		target_flow_script = flow_script;
	}
	ERR_FAIL_COND_V(!target_flow_script->has_node(p_node_reference.node_id), FIBER_ID_INVALID);
	FlowScriptExecutionFiberID new_fiber_id = cache_next_free_fiber_id;
	if (fiber_list[new_fiber_id].prepare_for_execution(target_flow_script, p_node_reference.node_id))
	{
		update_cache_next_free_fiber_id();
		return new_fiber_id;
	}
	else
	{
		ERR_FAIL_V(FIBER_ID_INVALID);
	}
}


bool FlowScriptExecutionController::internal_exec_branch(const FlowScriptExecutionFiberID p_fiber_id)
{
	ERR_FAIL_INDEX_V(p_fiber_id, FIBERS_MAX, false);
	ERR_FAIL_COND_V(!fiber_list[p_fiber_id].is_active(), false);
	fiber_list[p_fiber_id].execute_current_node();
	return true;
}


void FlowScriptExecutionController::update_cache_next_free_fiber_id()
{
	for (FlowScriptExecutionFiberID curr_id = 0; curr_id < FIBERS_MAX; curr_id++)
	{
		if (!fiber_list[curr_id].is_active())
		{
			cache_next_free_fiber_id = curr_id;
			return;
		}
	}
	cache_next_free_fiber_id = FIBER_ID_INVALID;
}


FlowScriptExecutionController::FlowScriptExecutionController()
{
	for (FlowScriptExecutionFiberID curr_id = 0; curr_id < FIBERS_MAX; curr_id++)
	{
		fiber_list[curr_id].self_id = curr_id;
		fiber_list[curr_id].execution_controller_ptr = this;
	}
}


FlowScriptExecutionController::~FlowScriptExecutionController()
{
}
