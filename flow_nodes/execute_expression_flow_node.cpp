#include "execute_expression_flow_node.hpp"


void ExecuteExpressionFlowNode::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_next_flow_node_id", "flow_node_id"), &ExecuteExpressionFlowNode::set_next_flow_node_id);
	ClassDB::bind_method(D_METHOD("get_next_flow_node_id"), &ExecuteExpressionFlowNode::get_next_flow_node_id);
	ClassDB::bind_method(D_METHOD("set_expression", "expression"), &ExecuteExpressionFlowNode::set_expression);
	ClassDB::bind_method(D_METHOD("get_expression"), &ExecuteExpressionFlowNode::get_expression);

	ADD_PROPERTY(PropertyInfo(TYPE_FLOW_NODE_ID, "next_flow_node_id"), "set_next_flow_node_id", "get_next_flow_node_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "expression", PROPERTY_HINT_EXPRESSION), "set_expression", "get_expression");
}


void ExecuteExpressionFlowNode::set_next_flow_node_id(const FlowNodeID p_flow_node_id)
{
	if (p_flow_node_id == next_flow_node_id)
	{
		return;
	}

	next_flow_node_id = p_flow_node_id;
	emit_changed();
}


FlowNodeID ExecuteExpressionFlowNode::get_next_flow_node_id() const
{
	return next_flow_node_id;
}


void ExecuteExpressionFlowNode::set_expression(const String &p_expression)
{
	if (p_expression == expression)
	{
		return;
	}

	expression = p_expression;
	emit_changed();
}


String ExecuteExpressionFlowNode::get_expression() const
{
	return expression;
}


void ExecuteExpressionFlowNode::_execute(FlowNodeExecutionState *p_state)
{
	p_state->get_flow_bridge()->evaluate_multi_line_expression(expression);
	p_state->finish(next_flow_node_id, Variant());
}


bool ExecuteExpressionFlowNode::_is_property_flow_node_reference(const StringName &p_name) const
{
	return p_name == SNAME("next_flow_node_id");
}


void ExecuteExpressionFlowNode::_on_external_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to)
{
	if (next_flow_node_id == p_from)
	{
		next_flow_node_id = p_to;
	}
}
