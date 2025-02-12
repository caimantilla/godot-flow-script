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
	int curr_step_count = p_context->get_variable(VARIABLE_STEP_COUNT);
	curr_step_count++;
	p_context->set_variable(VARIABLE_STEP_COUNT, curr_step_count);

	// did i never write this ???
	const bool result = p_context->get_bridge_ptr()->get_built_in_node_interface()->evaluate_multiline_boolean_expression(expression, false);
}


void FlowScriptNodeLoopWhileExpressionResultTrue::set_runtime_state(FlowScriptNodeContext *p_context, const Dictionary &p_state)
{
	p_context->set_variable(VARIABLE_RESTORE_SAVE, true);
	p_context->set_variable(VARIABLE_STEP_COUNT, p_state.get("step_count", -1));
}


Dictionary FlowScriptNodeLoopWhileExpressionResultTrue::get_runtime_state(const FlowScriptNodeContext *p_context) const
{
	Dictionary d;
	d["step_count"] = p_context->get_variable(VARIABLE_STEP_COUNT);
	return d;
}


void FlowScriptNodeLoopWhileExpressionResultTrue::set_data_state(const Dictionary &p_data)
{
	set_expression(p_data.get("expression", String()));
}


Dictionary FlowScriptNodeLoopWhileExpressionResultTrue::get_data_state() const
{
	Dictionary d;
	d["expression"] = expression;
	return d;
}


void FlowScriptNodeLoopWhileExpressionResultTrue::get_output_connection_list_lengths(List<FlowScriptNodeConnectionListLength> *p_lengths) const
{
	p_lengths->push_back(1);
	p_lengths->push_back(1);
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
