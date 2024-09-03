#include "flow_script_node_wait_duration_expression_result.hpp"


void FlowScriptNodeWaitDurationExpressionResult::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_expression", "text"), &FlowScriptNodeWaitDurationExpressionResult::set_expression);
	ClassDB::bind_method(D_METHOD("get_expression"), &FlowScriptNodeWaitDurationExpressionResult::get_expression);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "expression", PROPERTY_HINT_EXPRESSION), "set_expression", "get_expression");
}


real_t FlowScriptNodeWaitDurationExpressionResult::get_initial_duration(FlowScriptNodeContext *p_context) const
{
	ERR_FAIL_COND_V_MSG(expression.strip_edges().is_empty(), real_t(0), RTR("No expression has been written."));
	Variant result = p_context->get_bridge_ptr()->evaluate_expression(expression);
	ERR_FAIL_COND_V_MSG(result.get_type() != Variant::INT && result.get_type() != Variant::FLOAT, real_t(0), vformat(RTR("The evaluated expression:\n%s\ndid not evaluate to a number, and therefor cannot be used as a duration."), expression));
	return real_t(result);
}


void FlowScriptNodeWaitDurationExpressionResult::set_expression(const String &p_text)
{
	if (p_text == expression)
	{
		return;
	}
	expression = p_text;
	emit_changed();
}


String FlowScriptNodeWaitDurationExpressionResult::get_expression() const
{
	return expression;
}
