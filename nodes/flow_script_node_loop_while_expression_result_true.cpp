#include "flow_script_node_loop_while_expression_result_true.hpp"


void FlowScriptNodeLoopWhileExpressionResultTrue::_bind_methods()
{
	BIND_ENUM_CONSTANT(CONNECTION_LIST_BREAK);
	BIND_ENUM_CONSTANT(CONNECTION_LIST_LOOP);

	BIND_ENUM_CONSTANT(VARIABLE_STEP_COUNT);
	BIND_ENUM_CONSTANT(VARIABLE_RESTORE_SAVE);

	ClassDB::bind_method(D_METHOD("set_expression", "text"), &FlowScriptNodeLoopWhileExpressionResultTrue::set_expression);
	ClassDB::bind_method(D_METHOD("get_expression"), &FlowScriptNodeLoopWhileExpressionResultTrue::get_expression);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "expression", PROPERTY_HINT_EXPRESSION), "set_expression", "get_expression");
}


void FlowScriptNodeLoopWhileExpressionResultTrue::exec_startup(FlowScriptNodeContext *p_context)
{
	p_context->set_variable(VARIABLE_STEP_COUNT, -1);
	p_context->set_variable(VARIABLE_RESTORE_SAVE, false);
}


void FlowScriptNodeLoopWhileExpressionResultTrue::exec_step(FlowScriptNodeContext *p_context)
{
	if (bool(p_context->get_variable(VARIABLE_RESTORE_SAVE)))
	{
		p_context->set_variable(VARIABLE_RESTORE_SAVE, false);
		return;
	}
	int64_t curr_step_count = p_context->get_variable(VARIABLE_STEP_COUNT);
	curr_step_count++;
	p_context->set_variable(VARIABLE_STEP_COUNT, curr_step_count);
}


void FlowScriptNodeLoopWhileExpressionResultTrue::set_state(FlowScriptNodeContext *p_context, const Dictionary &p_state)
{
	p_context->set_variable(VARIABLE_RESTORE_SAVE, true);
	if (p_state.has("step_count"))
	{
		p_context->set_variable(VARIABLE_STEP_COUNT, p_state["step_count"]);
	}
}


void FlowScriptNodeLoopWhileExpressionResultTrue::get_state(const FlowScriptNodeContext *p_context, Dictionary &r_state) const
{
	r_state["step_count"] = p_context->get_variable(VARIABLE_STEP_COUNT);
}


void FlowScriptNodeLoopWhileExpressionResultTrue::set_json_data(const Dictionary &p_data)
{
	if (p_data.has("expression"))
	{
		set_expression(p_data["expression"]);
	}
}


void FlowScriptNodeLoopWhileExpressionResultTrue::get_json_data(Dictionary &r_data) const
{
	if (!expression.is_empty())
	{
		r_data["expression"] = expression;
	}
}


void FlowScriptNodeLoopWhileExpressionResultTrue::get_output_connection_list_lengths(List<int64_t> &r_lengths) const
{
	r_lengths.push_back(1);
	r_lengths.push_back(1);
}


void FlowScriptNodeLoopWhileExpressionResultTrue::set_expression(const String &p_text)
{
	if (p_text == expression)
	{
		return;
	}
	expression = p_text;
	emit_changed();
}


String FlowScriptNodeLoopWhileExpressionResultTrue::get_expression() const
{
	return expression;
}
