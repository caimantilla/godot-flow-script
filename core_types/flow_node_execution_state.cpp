#include "flow_node_execution_state.hpp"


void FlowNodeExecutionState::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_flow_bridge"), &FlowNodeExecutionState::get_flow_bridge);

	ClassDB::bind_method(D_METHOD("set_temporary_variable", "name", "value"), &FlowNodeExecutionState::set_temporary_variable);
	ClassDB::bind_method(D_METHOD("get_temporary_variable", "name"), &FlowNodeExecutionState::get_temporary_variable);
	ClassDB::bind_method(D_METHOD("has_temporary_variable", "name"), &FlowNodeExecutionState::has_temporary_variable);
	ClassDB::bind_method(D_METHOD("get_temporary_variable_names"), &FlowNodeExecutionState::get_temporary_variable_names);
	ClassDB::bind_method(D_METHOD("restore_temporary_state", "temporary_state"), &FlowNodeExecutionState::restore_temporary_state);
	ClassDB::bind_method(D_METHOD("get_temporary_state"), &FlowNodeExecutionState::get_temporary_state);

	ClassDB::bind_method(D_METHOD("finish", "next_flow_node_id", "return_value"), &FlowNodeExecutionState::finish);
	ClassDB::bind_method(D_METHOD("request_multiple_new_fibers", "initial_flow_node_ids", "wait_fibers_completion"), &FlowNodeExecutionState::request_multiple_new_fibers);
	ClassDB::bind_method(D_METHOD("request_new_fiber", "flow_node_id", "wait_fiber_completion"), &FlowNodeExecutionState::request_new_fiber);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "flow_bridge", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE, SNAME("FlowBridge")), "", "get_flow_bridge");

	ADD_SIGNAL(MethodInfo("resumed")); // Used by FlowNodes
	ADD_SIGNAL(MethodInfo("finished", PropertyInfo(TYPE_FLOW_NODE_ID, "next_flow_node_id"), PropertyInfo(Variant::NIL, "return_value")));
	ADD_SIGNAL(MethodInfo("new_fibers_requested", PropertyInfo(TYPE_FLOW_NODE_ID_ARRAY, "initial_flow_node_ids"), PropertyInfo(Variant::BOOL, "wait_fibers_completion")));
}


FlowBridge *FlowNodeExecutionState::get_flow_bridge() const
{
	return flow_bridge;
}


void FlowNodeExecutionState::set_temporary_variable(const StringName &p_name, const Variant &p_value)
{
	set_meta(p_name, p_value);
}


Variant FlowNodeExecutionState::get_temporary_variable(const StringName &p_name) const
{
	return get_meta(p_name);
}


bool FlowNodeExecutionState::has_temporary_variable(const StringName &p_name) const
{
	return has_meta(p_name);
}


PackedStringArray FlowNodeExecutionState::get_temporary_variable_names() const
{
	List<StringName> meta_keys;
	get_meta_list(&meta_keys);

	PackedStringArray var_names;
	var_names.resize(meta_keys.size());

	for (int i = 0; i < meta_keys.size(); i++)
	{
		var_names.set(i, meta_keys[i]);
	}

	return var_names;
}


void FlowNodeExecutionState::restore_temporary_state(const Dictionary &p_temporary_state)
{
	Array keys = p_temporary_state.keys();

	for (int i = 0; i < keys.size(); i++)
	{
		Variant key_var = keys[i];

		if (key_var.get_type() != Variant::STRING)
		{
			continue;
		}

		String key = String(key_var);
		Variant value = p_temporary_state[key_var];

		set_temporary_variable(StringName(key), value);
	}
}


Dictionary FlowNodeExecutionState::get_temporary_state() const
{
	Dictionary state;

	for (const String &var_key : get_temporary_variable_names())
	{
		state[var_key] = get_temporary_variable(var_key);
	}

	return state;
}


void FlowNodeExecutionState::resume()
{
	emit_signal(SNAME("resumed"));
}


void FlowNodeExecutionState::finish(const FlowNodeID p_next_flow_node_id, const Variant &p_return_value)
{
	emit_signal(SNAME("finished"), p_next_flow_node_id, p_return_value);
}


void FlowNodeExecutionState::request_multiple_new_fibers(const FlowNodeIDArray &p_initial_flow_node_ids, bool p_wait_fibers_completion)
{
	emit_signal(SNAME("new_fibers_requested"), p_initial_flow_node_ids, p_wait_fibers_completion);
}


void FlowNodeExecutionState::request_new_fiber(const FlowNodeID p_flow_node_id, bool p_wait_fiber_completion)
{
	FlowNodeIDArray ids;
	ids.push_back(p_flow_node_id);

	request_multiple_new_fibers(ids, p_wait_fiber_completion);
}
