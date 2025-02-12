#include "flow_script_node_return_expression_result.hpp"


void FlowScriptNodeReturnExpressionResult::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_expression", "text"), &FlowScriptNodeReturnExpressionResult::set_expression);
	ClassDB::bind_method(D_METHOD("get_expression"), &FlowScriptNodeReturnExpressionResult::get_expression);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "expression", PROPERTY_HINT_EXPRESSION), "set_expression", "get_expression");
}


void FlowScriptNodeReturnExpressionResult::exec_step(FlowScriptNodeContext *p_context)
{
	Variant result = p_context->get_bridge_ptr()->get_built_in_node_interface()->evaluate_expression(expression);
	p_context->set_return_value(result);
	p_context->finish();
}


void FlowScriptNodeReturnExpressionResult::set_data_state(const Dictionary &p_data)
{
	set_expression(p_data.get("expression", String()));
}


Dictionary FlowScriptNodeReturnExpressionResult::get_data_state() const
{
	Dictionary d;
	d["expression"] = expression;
	return d;
}


void FlowScriptNodeReturnExpressionResult::set_expression(const String &p_text)
{
	if (p_text == expression)
	{
		return;
	}
	expression = p_text;
	emit_changed();
}


String FlowScriptNodeReturnExpressionResult::get_expression() const
{
	return expression;
}
