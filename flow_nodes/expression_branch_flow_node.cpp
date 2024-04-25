#include "expression_branch_flow_node.hpp"


void ExpressionBranchFlowNode::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_expression", "expression"), &ExpressionBranchFlowNode::set_expression);
	ClassDB::bind_method(D_METHOD("get_expression"), &ExpressionBranchFlowNode::get_expression);

	ClassDB::bind_method(D_METHOD("set_true_flow_node_id", "flow_node_id"), &ExpressionBranchFlowNode::set_true_flow_node_id);
	ClassDB::bind_method(D_METHOD("get_true_flow_node_id"), &ExpressionBranchFlowNode::get_true_flow_node_id);

	ClassDB::bind_method(D_METHOD("set_false_flow_node_id", "flow_node_id"), &ExpressionBranchFlowNode::set_false_flow_node_id);
	ClassDB::bind_method(D_METHOD("get_false_flow_node_id"), &ExpressionBranchFlowNode::get_false_flow_node_id);

	ADD_PROPERTY(PropertyInfo(TYPE_FLOW_NODE_ID, "true_flow_node_id"), "set_true_flow_node_id", "get_true_flow_node_id");
	ADD_PROPERTY(PropertyInfo(TYPE_FLOW_NODE_ID, "false_flow_node_id"), "set_false_flow_node_id", "get_false_flow_node_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "expression", PROPERTY_HINT_EXPRESSION), "set_expression", "get_expression");
}


void ExpressionBranchFlowNode::set_expression(const String &p_expression)
{
	if (p_expression == expression)
	{
		return;
	}

	expression = p_expression;
	emit_changed();
}


String ExpressionBranchFlowNode::get_expression() const
{
	return expression;
}


void ExpressionBranchFlowNode::set_true_flow_node_id(const FlowNodeID p_flow_node_id)
{
	if (p_flow_node_id == true_flow_node_id)
	{
		return;
	}

	true_flow_node_id = p_flow_node_id;
	emit_changed();
}


FlowNodeID ExpressionBranchFlowNode::get_true_flow_node_id() const
{
	return true_flow_node_id;
}


void ExpressionBranchFlowNode::set_false_flow_node_id(const FlowNodeID p_flow_node_id)
{
	if (p_flow_node_id == false_flow_node_id)
	{
		return;
	}

	false_flow_node_id = p_flow_node_id;
	emit_changed();
}


FlowNodeID ExpressionBranchFlowNode::get_false_flow_node_id() const
{
	return false_flow_node_id;
}


void ExpressionBranchFlowNode::_execute(FlowNodeExecutionState *p_state)
{
	int next_flow_node_id;

	if (p_state->get_flow_bridge()->evaluate_multi_line_boolean_expression(expression, false))
	{
		next_flow_node_id = true_flow_node_id;
	}
	else
	{
		next_flow_node_id = false_flow_node_id;
	}

	p_state->finish(next_flow_node_id, Variant());
}


bool ExpressionBranchFlowNode::_is_property_flow_node_reference(const StringName &p_name) const
{
	return (p_name == SNAME("true_flow_node_id")) || (p_name == SNAME("false_flow_node_id"));
}


void ExpressionBranchFlowNode::_on_external_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to)
{
	if (true_flow_node_id == p_from)
	{
		true_flow_node_id = p_to;
	}
	if (false_flow_node_id == p_from)
	{
		false_flow_node_id = p_to;
	}
}
