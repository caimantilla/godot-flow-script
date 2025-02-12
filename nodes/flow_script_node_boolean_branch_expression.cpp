#include "flow_script_node_boolean_branch_expression.hpp"


void FlowScriptNodeBooleanBranchExpression::_bind_methods()
{
	BIND_ENUM_CONSTANT(CONNECTION_LIST_TRUE);
	BIND_ENUM_CONSTANT(CONNECTION_LIST_FALSE);

	ClassDB::bind_method(D_METHOD("set_expression", "expression"), &FlowScriptNodeBooleanBranchExpression::set_expression);
	ClassDB::bind_method(D_METHOD("get_expression"), &FlowScriptNodeBooleanBranchExpression::get_expression);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "condition", PROPERTY_HINT_EXPRESSION), "set_expression", "get_expression");
}


void FlowScriptNodeBooleanBranchExpression::exec_step(FlowScriptNodeContext *p_context)
{
	const bool result = p_context->get_bridge_ptr()->get_built_in_node_interface()->evaluate_multiline_boolean_expression(expression, true);
	const FlowScriptNodeOutputConnection destination = FlowScriptNodeOutputConnection::create_connection(
			result ? CONNECTION_LIST_TRUE : CONNECTION_LIST_FALSE,
			0
	);
	p_context->advance(destination);
}


void FlowScriptNodeBooleanBranchExpression::set_data_state(const Dictionary &p_data)
{
	set_expression(p_data.get("condition", expression));
}


Dictionary FlowScriptNodeBooleanBranchExpression::get_data_state() const
{
	Dictionary d;
	d["condition"] = expression;
	return d;
}


void FlowScriptNodeBooleanBranchExpression::get_output_connection_list_lengths(List<FlowScriptNodeConnectionListLength> *p_lengths) const
{
	p_lengths->push_back(1);
	p_lengths->push_back(1);
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
