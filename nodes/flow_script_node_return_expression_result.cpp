#include "flow_script_node_return_expression_result.hpp"


void FlowScriptNodeReturnExpressionResult::exec_step(FlowScriptNodeContext *p_context)
{
	Variant result = p_context->get_bridge_ptr()->evaluate_expression(expression);
	p_context->set_return_value(result);
	p_context->finish();
}


void FlowScriptNodeReturnExpressionResult::set_json_data(const Dictionary &p_data)
{
	if (p_data.has("expression"))
	{
		expression = p_data["expression"];
	}
}


void FlowScriptNodeReturnExpressionResult::get_json_data(Dictionary &r_data) const
{
	r_data["expression"] = expression;
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
