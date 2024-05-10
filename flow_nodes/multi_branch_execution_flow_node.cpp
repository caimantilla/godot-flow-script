#include "multi_branch_execution_flow_node.hpp"


void MultiBranchExecutionFlowNode::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_next_flow_node_id", "flow_node_id"), &MultiBranchExecutionFlowNode::set_next_flow_node_id);
	ClassDB::bind_method(D_METHOD("get_next_flow_node_id"), &MultiBranchExecutionFlowNode::get_next_flow_node_id);
	ClassDB::bind_method(D_METHOD("set_execution_stack", "execution_stack"), &MultiBranchExecutionFlowNode::set_execution_stack);
	ClassDB::bind_method(D_METHOD("get_execution_stack"), &MultiBranchExecutionFlowNode::get_execution_stack);
	ClassDB::bind_method(D_METHOD("set_execution_stack_size", "size"), &MultiBranchExecutionFlowNode::set_execution_stack_size);
	ClassDB::bind_method(D_METHOD("get_execution_stack_size"), &MultiBranchExecutionFlowNode::get_execution_stack_size);

	ADD_PROPERTY(PropertyInfo(TYPE_FLOW_NODE_ID, "next_flow_node_id"), "set_next_flow_node_id", "get_next_flow_node_id");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "execution_stack_size", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR), "set_execution_stack_size", "get_execution_stack_size");
	ADD_PROPERTY(PropertyInfo(TYPE_FLOW_NODE_ID_ARRAY, "execution_stack"), "set_execution_stack", "get_execution_stack");
}


bool MultiBranchExecutionFlowNode::_is_property_flow_node_reference(const StringName &p_name) const
{
	return p_name == SNAME("next_flow_node_id");
}


bool MultiBranchExecutionFlowNode::_is_property_flow_node_reference_list(const StringName &p_name) const
{
	return p_name == SNAME("execution_stack");
}


void MultiBranchExecutionFlowNode::_on_external_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to)
{
	if (p_from == next_flow_node_id)
	{
		next_flow_node_id = p_to;
	}

	for (int i = execution_stack.size() - 1; i > -1; i--)
	{
		if (execution_stack.get(i) == p_from)
		{
			set_execution_stack_entry_at(i, p_to);
		}
	}
}


void MultiBranchExecutionFlowNode::set_next_flow_node_id(const FlowNodeID p_node_id)
{
	if (p_node_id == next_flow_node_id)
	{
		return;
	}

	next_flow_node_id = p_node_id;
	emit_changed();
}


FlowNodeID MultiBranchExecutionFlowNode::get_next_flow_node_id() const
{
	return next_flow_node_id;
}


void MultiBranchExecutionFlowNode::set_execution_stack(const FlowNodeIDArray &p_execution_stack)
{
	FlowNodeIDArray new_stack = p_execution_stack;

	if (new_stack.size() > MAX_STACK_SIZE)
	{
		new_stack.resize(MAX_STACK_SIZE);
	}

	execution_stack = new_stack;
	emit_changed();
}


FlowNodeIDArray MultiBranchExecutionFlowNode::get_execution_stack() const
{
	return execution_stack;
}


void MultiBranchExecutionFlowNode::set_execution_stack_size(int p_size)
{
	p_size = MAX(MIN_STACK_SIZE, p_size);
	p_size = MIN(MAX_STACK_SIZE, p_size);

	if (p_size == execution_stack.size())
	{
		return;
	}

	int old_size = execution_stack.size();
	execution_stack.resize(p_size);

	// Clear out any leftover memories when enlarging the stack
	for (int i = old_size; i < p_size; i++)
	{
		execution_stack.set(i, FLOW_NODE_ID_NIL);
	}

	emit_changed();
}


int MultiBranchExecutionFlowNode::get_execution_stack_size() const
{
	return execution_stack.size();
}


void MultiBranchExecutionFlowNode::set_execution_stack_entry_at(const int p_idx, const FlowNodeID p_flow_node_id)
{
	ERR_FAIL_INDEX(p_idx, execution_stack.size());

	if (execution_stack.get(p_idx) == p_flow_node_id)
	{
		return;
	}

	execution_stack.set(p_idx, p_flow_node_id);
	emit_changed();
}


FlowNodeID MultiBranchExecutionFlowNode::get_execution_stack_entry_at(const int p_idx) const
{
	ERR_FAIL_INDEX_V(p_idx, execution_stack.size(), FLOW_NODE_ID_NIL);

	return execution_stack.get(p_idx);
}


void MultiBranchExecutionFlowNode::add_execution_stack_entry(const FlowNodeID p_flow_node_id)
{
	ERR_FAIL_COND_MSG(execution_stack.size() + 1 >= MAX_STACK_SIZE, "Can't add any more execution stack entries, limit exceeded!");
	
	if (p_flow_node_id == FLOW_NODE_ID_NIL)
	{
		return;
	}

	execution_stack.push_back(p_flow_node_id);
	emit_changed();
}


void MultiBranchExecutionFlowNode::remove_execution_stack_entry_at(const int p_idx)
{
	ERR_FAIL_INDEX(p_idx, execution_stack.size());
	execution_stack.remove_at(p_idx);
	emit_changed();
}
