#include "multi_branch_execution_flow_node.hpp"


static const int MAX_STACK_SIZE = 9;
static const String EXECUTION_STACK_PROPERTY_PREFIX = "execution_stack/";
static const String EXECUTION_STACK_SIZE_PROPERTY_NAME = "execution_stack_size";


void MultiBranchExecutionFlowNode::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_next_flow_node_id", "flow_node_id"), &MultiBranchExecutionFlowNode::set_next_flow_node_id);
	ClassDB::bind_method(D_METHOD("get_next_flow_node_id"), &MultiBranchExecutionFlowNode::get_next_flow_node_id);
	ClassDB::bind_method(D_METHOD("set_execution_stack", "execution_stack"), &MultiBranchExecutionFlowNode::set_execution_stack);
	ClassDB::bind_method(D_METHOD("get_execution_stack"), &MultiBranchExecutionFlowNode::get_execution_stack);

	ADD_PROPERTY(PropertyInfo(TYPE_FLOW_NODE_ID, "next_flow_node_id"), "set_next_flow_node_id", "get_next_flow_node_id");
	ADD_PROPERTY(PropertyInfo(TYPE_FLOW_NODE_ID_ARRAY, "execution_stack", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE), "set_execution_stack", "get_execution_stack");
}


bool MultiBranchExecutionFlowNode::_set(const StringName &p_name, const Variant &p_value)
{
	if (p_name == SNAME("execution_stack_size"))
	{
		_resize_execution_stack(p_value, true);
		return true;
	}

	String name = String(p_name);
	if (name.begins_with(EXECUTION_STACK_PROPERTY_PREFIX))
	{
		String stack_idx_str = name.get_slice("/", 1);
		int stack_idx = stack_idx_str.to_int();

		_set_execution_stack_entry_at(stack_idx, p_value, true);
		return true;
	}

	return false;
}


bool MultiBranchExecutionFlowNode::_get(const StringName &p_name, Variant &r_ret) const
{
	if (p_name == SNAME("execution_stack_size"))
	{
		r_ret = get_execution_stack_size();
		return true;
	}

	String name = String(p_name);
	if (name.begins_with(EXECUTION_STACK_PROPERTY_PREFIX))
	{
		String stack_idx_str = name.get_slice("/", 1);
		int stack_idx = stack_idx_str.to_int();

		r_ret = get_execution_stack_entry_at(stack_idx);
		return true;
	}
	
	return false;
}


void MultiBranchExecutionFlowNode::_get_property_list(List<PropertyInfo> *p_list) const
{
	p_list->push_back(PropertyInfo(Variant::INT, "execution_stack_size", PROPERTY_HINT_RANGE, vformat("0,%d,1", MAX_STACK_SIZE), PROPERTY_USAGE_EDITOR));

	for (int i = 0; i < get_execution_stack_size(); i++)
	{
		String stack_entry_property_name = EXECUTION_STACK_PROPERTY_PREFIX + itos(i);
		p_list->push_back(PropertyInfo(Variant::STRING, stack_entry_property_name, PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR));
	}
}


bool MultiBranchExecutionFlowNode::_property_can_revert(const StringName &p_name) const
{
	return p_name == SNAME("execution_stack_size") || String(p_name).begins_with(EXECUTION_STACK_PROPERTY_PREFIX);
}


bool MultiBranchExecutionFlowNode::_property_get_revert(const StringName &p_name, Variant &r_property) const
{
	if (p_name == SNAME("execution_stack_size"))
	{
		r_property = 0;
		return true;
	}

	if (String(p_name).begins_with(EXECUTION_STACK_PROPERTY_PREFIX))
	{
		r_property = FLOW_NODE_ID_NIL;
		return true;
	}

	return false;
}


void MultiBranchExecutionFlowNode::_on_external_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to)
{
	if (next_flow_node_id == p_from)
	{
		next_flow_node_id = p_to;
	}

	for (int i = 0; i < get_execution_stack_size(); i++)
	{
		FlowNodeID current_id = get_execution_stack_entry_at(i);
		if (current_id == p_from)
		{
			_set_execution_stack_entry_at(i, p_to, false);
		}
	}
}


bool MultiBranchExecutionFlowNode::_is_property_flow_node_reference(const StringName &p_name) const
{
	return (p_name == SNAME("next_flow_node_id")) || (String(p_name).begins_with(SNAME("execution_stack/")));
}


void MultiBranchExecutionFlowNode::set_next_flow_node_id(const FlowNodeID p_flow_node_id)
{
	if (p_flow_node_id == next_flow_node_id)
	{
		return;
	}

	next_flow_node_id = p_flow_node_id;
	emit_changed();
}


FlowNodeID MultiBranchExecutionFlowNode::get_next_flow_node_id() const
{
	return next_flow_node_id;
}


void MultiBranchExecutionFlowNode::set_execution_stack(const FlowNodeIDArray &p_execution_stack)
{
	if (p_execution_stack == execution_stack)
	{
		return;
	}

	execution_stack = p_execution_stack;
	emit_changed();
}


FlowNodeIDArray MultiBranchExecutionFlowNode::get_execution_stack() const
{
	return execution_stack;
}


int MultiBranchExecutionFlowNode::get_execution_stack_size() const
{
	return execution_stack.size();
}


void MultiBranchExecutionFlowNode::set_execution_stack_entry_at(const int p_idx, const FlowNodeID p_new_flow_node_id)
{
	_set_execution_stack_entry_at(p_idx, p_new_flow_node_id, true);
}


FlowNodeID MultiBranchExecutionFlowNode::get_execution_stack_entry_at(const int p_idx) const
{
	if (p_idx > -1 && p_idx < get_execution_stack_size())
	{
		return execution_stack.get(p_idx);
	}

	return FLOW_NODE_ID_NIL;
}


void MultiBranchExecutionFlowNode::remove_execution_stack_entry_at(const int p_idx)
{
	_remove_execution_stack_entry_at(p_idx, true);
}


void MultiBranchExecutionFlowNode::resize_execution_stack(const int p_size)
{
	_resize_execution_stack(p_size, true);
}


int MultiBranchExecutionFlowNode::get_max_execution_stack_size() const
{
	return MAX_STACK_SIZE;
}


void MultiBranchExecutionFlowNode::_set_execution_stack_entry_at(const int p_idx, const FlowNodeID p_new_flow_node_id, const bool p_emit)
{
	if (p_idx > -1 && p_idx < get_execution_stack_size())
	{
		FlowNodeID current = execution_stack.get(p_idx);
		if (current != p_new_flow_node_id)
		{
			execution_stack.set(p_idx, p_new_flow_node_id);

			if (p_emit)
			{
				emit_changed();
			}
		}
	}
}


void MultiBranchExecutionFlowNode::_remove_execution_stack_entry_at(const int p_idx, const bool p_emit)
{
	if (p_idx > -1 && p_idx < get_execution_stack_size())
	{
		execution_stack.remove_at(p_idx);

		if (p_emit)
		{
			emit_changed();
		}
	}
}


void MultiBranchExecutionFlowNode::_resize_execution_stack(const int p_size, const bool p_emit)
{
	if (p_size > -1 && p_size < get_max_execution_stack_size() && p_size != get_execution_stack_size())
	{
		execution_stack.resize(p_size);

		if (p_emit)
		{
			emit_changed();
		}

		notify_property_list_changed();
	}
}
