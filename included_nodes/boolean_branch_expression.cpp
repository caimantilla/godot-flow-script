#include "boolean_branch_expression.hpp"


void FlowScriptNodeBooleanBranchExpression::_bind_methods()
{
	BIND_ENUM_CONSTANT(CONNECTION_LIST_TRUE);
	BIND_ENUM_CONSTANT(CONNECTION_LIST_FALSE);

	ClassDB::bind_method(D_METHOD("set_expression", "expression"), &FlowScriptNodeBooleanBranchExpression::set_expression);
	ClassDB::bind_method(D_METHOD("get_expression"), &FlowScriptNodeBooleanBranchExpression::get_expression);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "condition", PROPERTY_HINT_EXPRESSION), "set_expression", "get_expression");
}


void FlowScriptNodeBooleanBranchExpression::set_expression(const String &p_expression)
{
	if (expression == p_expression)
		return;
	expression = p_expression;
	emit_changed();
}


String FlowScriptNodeBooleanBranchExpression::get_expression() const
{
	return expression;
}


void FlowScriptNodeBooleanBranchExpression::exec_step(FlowScriptNodeContext *p_context)
{
	bool result = p_context->get_bridge_ptr()->evaluate_multiline_boolean_expression(expression, true);
	FlowScriptNodeID target = FlowScript::NODE_ID_INVALID;
	if (result)
	{
		target = p_context->get_flow_script_ptr()->get_node_connection(p_context->get_current_node_id(), CONNECTION_LIST_TRUE, 0);
	}
	else
	{
		target = p_context->get_flow_script_ptr()->get_node_connection(p_context->get_current_node_id(), CONNECTION_LIST_FALSE, 0);
	}
	p_context->advance_to_node(target);
}
