#include "flow_script_node_context.hpp"
#include "flow_script.hpp"
#include "flow_script_execution_controller.hpp"
#include "flow_script_bridge.hpp"
#include "flow_script_node.hpp"


void FlowScriptNodeContext::_bind_methods()
{
	BIND_CONSTANT(VARIABLES_MAX);

	ClassDB::bind_method(D_METHOD("advance_to_node", "next_node_id"), &FlowScriptNodeContext::advance_to_node);
	ClassDB::bind_method(D_METHOD("set_variable", "idx", "value"), &FlowScriptNodeContext::set_variable);
	ClassDB::bind_method(D_METHOD("get_variable", "idx"), &FlowScriptNodeContext::get_variable);
	ClassDB::bind_method(D_METHOD("has_variable", "idx"), &FlowScriptNodeContext::has_variable);
	ClassDB::bind_method(D_METHOD("get_current_node_id"), &FlowScriptNodeContext::get_current_node_id);
	ClassDB::bind_method(D_METHOD("await_branch_list", "initial_node_ids"), &FlowScriptNodeContext::bind_await_branch_list);
	ClassDB::bind_method(D_METHOD("await_branch_solo", "initial_node_id"), &FlowScriptNodeContext::await_branch_solo);
	ClassDB::bind_method(D_METHOD("get_bridge"), &FlowScriptNodeContext::get_bridge_ptr);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "current_node_id"), "", "get_current_node_id");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "bridge", PROPERTY_HINT_NODE_TYPE, "FlowScriptBridge", PROPERTY_USAGE_NONE, "FlowScriptBridge"), "", "get_bridge");
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


void FlowScriptNodeContext::invoke_step()
{
	ERR_FAIL_COND(exec_blocked);
	get_current_node_ptr()->exec_step(this);
}


void FlowScriptNodeContext::advance_to_node(const FlowScriptNodeID p_next_node_id)
{
	ERR_FAIL_COND(exec_blocked);
	exec_blocked = true;
	if (get_flow_script_ptr()->has_node(current_node_id))
	{
		get_current_node_ptr()->exec_cleanup(this);
	}
	current_node_id = p_next_node_id;
	if (get_flow_script_ptr()->has_node(current_node_id))
	{
		get_current_node_ptr()->exec_startup(this);
		exec_blocked = false;
		get_current_node_ptr()->exec_step(this);
	}
	else
	{
		exec_blocked = false;
		execution_controller_ptr->internal_fiber_finish(self_id);
	}
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
	return execution_controller_ptr->get_flow_script_ptr()->get_node_ref(current_node_id);
}


FlowScriptNode *FlowScriptNodeContext::get_current_node_ptr() const
{
	return execution_controller_ptr->get_flow_script_ptr()->get_node_ptr(current_node_id);
}


Ref<FlowScript> FlowScriptNodeContext::get_flow_script_ref() const
{
	return execution_controller_ptr->get_flow_script();
}


FlowScript *FlowScriptNodeContext::get_flow_script_ptr() const
{
	return execution_controller_ptr->get_flow_script_ptr();
}


FlowScriptBridge *FlowScriptNodeContext::get_bridge_ptr() const
{
	return execution_controller_ptr->get_bridge();
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


bool FlowScriptNodeContext::await_branch_list(const List<FlowScriptNodeID> p_initial_node_ids)
{
	int32_t to_wait_bits = execution_controller_ptr->internal_execute_sub_branch_list(p_initial_node_ids);
	if (to_wait_bits == 0)
	{
		return false;
	}
	else
	{
		awaiting_fibers_bits |= to_wait_bits;
		return true;
	}
}


bool FlowScriptNodeContext::await_branch_solo(const FlowScriptNodeID p_initial_node_id)
{
	List<FlowScriptNodeID> pass_list;
	pass_list.push_back(p_initial_node_id);
	return await_branch_list(pass_list);
}


String FlowScriptNodeContext::create_variable_idx_out_of_range_error(const uint8_t p_idx) const
{
	return "Variable index " + itos(p_idx) + " ouf of range 0-" + itos(VARIABLES_MAX);
}


bool FlowScriptNodeContext::bind_await_branch_list(const PackedInt32Array &p_initial_node_ids)
{
	List<FlowScriptNodeID> pass_list;
	for (int32_t node_id_32 : p_initial_node_ids)
		pass_list.push_back(node_id_32);
	return await_branch_list(pass_list);
}


FlowScriptNodeContext::FlowScriptNodeContext()
{
	self_id = FlowScriptExecutionController::FIBER_ID_INVALID;
	current_node_id = FlowScript::NODE_ID_INVALID;
}
