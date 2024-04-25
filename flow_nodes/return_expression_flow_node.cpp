#include "return_expression_flow_node.hpp"


void ReturnExpressionFlowNode::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_expression", "expression"), &ReturnExpressionFlowNode::set_expression);
	ClassDB::bind_method(D_METHOD("get_expression"), &ReturnExpressionFlowNode::get_expression);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "expression", PROPERTY_HINT_EXPRESSION, "", PROPERTY_USAGE_DEFAULT), "set_expression", "get_expression");
}


void ReturnExpressionFlowNode::set_expression(const String &p_expression)
{
	if (p_expression == expression)
	{
		return;
	}

	expression = p_expression;
	emit_changed();
}


String ReturnExpressionFlowNode::get_expression() const
{
	return expression;
}


void ReturnExpressionFlowNode::_execute(FlowNodeExecutionState *p_state)
{
	Variant result = p_state->get_flow_bridge()->evaluate_expression(expression);
	p_state->finish(FLOW_NODE_ID_NIL, result);
}
