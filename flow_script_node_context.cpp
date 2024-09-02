#include "flow_script_node_context.hpp"
#include "flow_script.hpp"
#include "flow_script_bridge.hpp"
#include "flow_script_node.hpp"


void FlowScriptNodeContext::_bind_methods()
{
	BIND_CONSTANT(VARIABLES_MAX);

	ClassDB::bind_method(D_METHOD("set_variable", "idx", "value"), &FlowScriptNodeContext::set_variable);
	ClassDB::bind_method(D_METHOD("get_variable", "idx"), &FlowScriptNodeContext::get_variable);
	ClassDB::bind_method(D_METHOD("has_variable", "idx"), &FlowScriptNodeContext::has_variable);
	ClassDB::bind_method(D_METHOD("get_current_flow_script"), &FlowScriptNodeContext::get_current_flow_script_ref);
	ClassDB::bind_method(D_METHOD("get_current_node_id"), &FlowScriptNodeContext::get_current_node_id);
	ClassDB::bind_method(D_METHOD("get_bridge"), &FlowScriptNodeContext::get_bridge_ref);

	ClassDB::bind_method(D_METHOD("invoke_step"), &FlowScriptNodeContext::invoke_step);
	ClassDB::bind_method(D_METHOD("advance", "connection_list", "connection_slot"), &FlowScriptNodeContext::bind_advance);
	ClassDB::bind_method(D_METHOD("finish"), &FlowScriptNodeContext::finish);
	ClassDB::bind_method(D_METHOD("add_await_branch", "connection_list", "connection_slot"), &FlowScriptNodeContext::bind_add_await_branch);
	ClassDB::bind_method(D_METHOD("execute_await_branches"), &FlowScriptNodeContext::bind_execute_await_branches);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "current_flow_script", PROPERTY_HINT_RESOURCE_TYPE, "FlowScript", PROPERTY_USAGE_DEFAULT, "FlowScript"), "", "get_current_flow_script");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "current_node_id"), "", "get_current_node_id");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "bridge", PROPERTY_HINT_NONE, "FlowScriptBridge", PROPERTY_USAGE_NONE, "FlowScriptBridge"), "", "get_bridge");
}


void FlowScriptNodeContext::reset()
{
	current_node_id = FlowScript::NODE_ID_INVALID;
	awaiting_fibers_bits = 0;
	finished_callback = Callable();
	return_variable = VariableSlot();
	clear_variables();
}


bool FlowScriptNodeContext::is_active() const
{
	return current_node_id != FlowScript::NODE_ID_INVALID;
}


void FlowScriptNodeContext::set_awaiting_fiber(const FlowScriptExecutionFiberID p_fiber_id, const bool p_enabled)
{
	int32_t flag = 1 << ((int)p_fiber_id);
	if (p_enabled)
	{
		awaiting_fibers_bits |= flag;
	}
	else
	{
		awaiting_fibers_bits = awaiting_fibers_bits &~ flag;
	}
}


bool FlowScriptNodeContext::is_awaiting_fiber(const FlowScriptExecutionFiberID p_fiber_id) const
{
	int32_t flag = 1 << ((int)p_fiber_id);
	return (awaiting_fibers_bits & flag) != 0;
}


void FlowScriptNodeContext::set_return_value(const Variant &p_value)
{
	return_variable.value = p_value;
	return_variable.used = true;
}


Variant FlowScriptNodeContext::get_return_value() const
{
	return return_variable.value;
}


void FlowScriptNodeContext::execute_current_node()
{
	exec_blocked = true;
	get_current_node_ptr()->exec_startup(this);
	exec_blocked = false;
	get_current_node_ptr()->exec_step(this);
}


bool FlowScriptNodeContext::prepare_for_execution(const Ref<FlowScript> &p_flow_script, const FlowScriptNodeID p_node_id)
{
	ERR_FAIL_COND_V(!p_flow_script.is_valid(), false);
	ERR_FAIL_COND_V(!p_flow_script->has_node(p_node_id), false);
	current_flow_script = p_flow_script.ptr();
	current_node_id = p_node_id;
	return true;
}


bool FlowScriptNodeContext::start(const Ref<FlowScript> &p_flow_script, const FlowScriptNodeID p_initial_node_id)
{
	if (!prepare_for_execution(p_flow_script, p_initial_node_id))
	{
		return false;
	}
	execute_current_node();
	return true;
}


void FlowScriptNodeContext::advance(const FlowScriptNodeOutputConnection &p_connection)
{
	exec_blocked = true;
	get_current_node_ptr()->exec_cleanup(this);
	exec_blocked = false;
	FlowScriptNodeReference next_node_ref = current_flow_script->get_node_connection(current_node_id, p_connection);
	if (next_node_ref.flow_script_id != FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID)
	{
		ERR_FAIL_COND(!current_flow_script->has_include_flow_script_instance(next_node_ref.flow_script_id));
		current_flow_script = current_flow_script->get_include_flow_script(next_node_ref.flow_script_id).ptr();
	}
	if (current_flow_script->has_node(next_node_ref.node_id))
	{
		current_node_id = next_node_ref.node_id;
		execute_current_node();
	}
	else
	{
		bridge_ptr->internal_fiber_finish(self_id);
	}
}


void FlowScriptNodeContext::bind_advance(const uint8_t p_connection_list, const int64_t p_connection_slot)
{
	advance(FlowScriptNodeOutputConnection(p_connection_list, p_connection_slot));
}


void FlowScriptNodeContext::finish()
{
	exec_blocked = true;
	get_current_node_ptr()->exec_cleanup(this);
	exec_blocked = false;
	bridge_ptr->internal_fiber_finish(self_id);
}


bool FlowScriptNodeContext::add_await_branch(const FlowScriptNodeOutputConnection &p_connection)
{
	FlowScriptNodeReference initial_node_ref = current_flow_script->get_node_connection(current_node_id, p_connection);
	FlowScriptExecutionFiberID branch_fiber_id = bridge_ptr->internal_init_branch(initial_node_ref);
	ERR_FAIL_COND_V(branch_fiber_id == FlowScriptBridge::FIBER_ID_INVALID, false);
	awaiting_fibers_bits |= (1 << branch_fiber_id);
	return true;
}


void FlowScriptNodeContext::bind_add_await_branch(const uint8_t p_connection_list, const int64_t p_connection_slot)
{
	add_await_branch(FlowScriptNodeOutputConnection(p_connection_list, p_connection_slot));
}


bool FlowScriptNodeContext::execute_await_branches()
{
	bool exec_ok = false;
	for (FlowScriptExecutionFiberID curr_fiber_id = 0; curr_fiber_id < FlowScriptBridge::FIBERS_MAX; curr_fiber_id++)
	{
		if (curr_fiber_id == self_id || !(awaiting_fibers_bits & (1 << curr_fiber_id)))
		{
			continue;
		}
		if (bridge_ptr->internal_exec_branch(curr_fiber_id))
		{
			exec_ok = true;
		}
	}
	if (!exec_ok)
	{
		invoke_step();
	}
	return exec_ok;
}


void FlowScriptNodeContext::bind_execute_await_branches()
{
	execute_await_branches();
}


bool FlowScriptNodeContext::is_node_reference_valid(const FlowScriptNodeReference &p_node_reference) const
{
	ERR_FAIL_NULL_V(current_flow_script, false);
	if (p_node_reference.flow_script_id != FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID)
	{
		if (!current_flow_script->has_include_flow_script_instance(p_node_reference.flow_script_id))
		{
			return false;
		}
		return current_flow_script->get_include_flow_script(p_node_reference.flow_script_id)->has_node(p_node_reference.node_id);
	}
	else
	{
		return current_flow_script->has_node(p_node_reference.node_id);
	}
}


void FlowScriptNodeContext::invoke_step()
{
	ERR_FAIL_COND(exec_blocked);
	get_current_node_ptr()->exec_step(this);
}


void FlowScriptNodeContext::set_variable(const uint8_t p_idx, const Variant &p_value)
{
	ERR_FAIL_INDEX_MSG(p_idx, VARIABLES_MAX, create_variable_idx_out_of_range_error(p_idx));
	variables[p_idx].used = true;
	variables[p_idx].value = p_value;
}


Variant FlowScriptNodeContext::get_variable(const uint8_t p_idx) const
{
	ERR_FAIL_INDEX_V_MSG(p_idx, VARIABLES_MAX, Variant(), create_variable_idx_out_of_range_error(p_idx));
	return variables[p_idx].value;
}


bool FlowScriptNodeContext::has_variable(const uint8_t p_idx) const
{
	ERR_FAIL_INDEX_V_MSG(p_idx, VARIABLES_MAX, false, create_variable_idx_out_of_range_error(p_idx));
	return variables[p_idx].used;
}


void FlowScriptNodeContext::clear_variables()
{
	for (int i = 0; i < VARIABLES_MAX; i++)
	{
		if (variables[i].used)
		{
			variables[i].used = false;
			variables[i].value = Variant();
		}
	}
}


FlowScriptNodeID FlowScriptNodeContext::get_current_node_id() const
{
	return current_node_id;
}


Ref<FlowScriptNode> FlowScriptNodeContext::get_current_node_ref() const
{
	return bridge_ptr->get_flow_script_ptr()->get_node_ref(current_node_id);
}


FlowScriptNode *FlowScriptNodeContext::get_current_node_ptr() const
{
	return bridge_ptr->get_flow_script_ptr()->get_node_ptr(current_node_id);
}


Ref<FlowScript> FlowScriptNodeContext::get_flow_script_ref() const
{
	return bridge_ptr->get_flow_script();
}


FlowScript *FlowScriptNodeContext::get_flow_script_ptr() const
{
	return bridge_ptr->get_flow_script_ptr();
}


Ref<FlowScriptBridge> FlowScriptNodeContext::get_bridge_ref() const
{
	return Ref<FlowScriptBridge>(bridge_ptr);
}


FlowScriptBridge *FlowScriptNodeContext::get_bridge_ptr() const
{
	return bridge_ptr;
}


void FlowScriptNodeContext::set_state(const Dictionary &p_state)
{
	for (const Variant &key : p_state.keys())
	{
		const Variant &value = p_state[key];
		if (key == "current_node")
		{
			ERR_CONTINUE(value.get_type() != Variant::INT);
			current_node_id = value;
		}
		else if (key == "awaiting")
		{
			ERR_CONTINUE(value.get_type() != Variant::INT);
			awaiting_fibers_bits = value;
		}
		else if (key == "node_state")
		{
			ERR_CONTINUE(value.get_type() != Variant::DICTIONARY);
			get_current_node_ptr()->set_state(this, value);
		}
	}
}


void FlowScriptNodeContext::get_state(Dictionary &r_state) const
{
	if (current_node_id != FlowScript::NODE_ID_INVALID)
		r_state["current_node"] = current_node_id;
	if (awaiting_fibers_bits != 0)
		r_state["awaiting"] = awaiting_fibers_bits;

	Dictionary state_dict_node;
	get_current_node_ptr()->get_state(this, state_dict_node);
	if (!state_dict_node.is_empty())
		r_state["node_state"] = state_dict_node;
}


String FlowScriptNodeContext::create_variable_idx_out_of_range_error(const uint8_t p_idx) const
{
	return "Variable index " + itos(p_idx) + " ouf of range 0-" + itos(VARIABLES_MAX);
}


FlowScriptNodeContext::FlowScriptNodeContext()
{
	self_id = FlowScriptBridge::FIBER_ID_INVALID;
	current_node_id = FlowScript::NODE_ID_INVALID;
}
