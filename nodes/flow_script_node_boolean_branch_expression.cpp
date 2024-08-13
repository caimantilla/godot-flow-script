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
	bool result = p_context->get_bridge_ptr()->evaluate_multiline_boolean_expression(expression, true);
	FlowScriptNodeOutputConnection connection;
	connection.slot = 0;
	if (result)
	{
		connection.list = CONNECTION_LIST_TRUE;
	}
	else
	{
		connection.list = CONNECTION_LIST_FALSE;
	}
	p_context->advance(connection);
}


void FlowScriptNodeBooleanBranchExpression::set_json_data(const Dictionary &p_data)
{
	if (p_data.has("condition"))
	{
		expression = p_data["condition"];
	}
}


void FlowScriptNodeBooleanBranchExpression::get_json_data(Dictionary &r_data) const
{
	r_data["condition"] = expression;
}


void FlowScriptNodeBooleanBranchExpression::get_output_connection_list_lengths(List<int64_t> &r_lengths) const
{
	r_lengths.push_back(1);
	r_lengths.push_back(1);
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
