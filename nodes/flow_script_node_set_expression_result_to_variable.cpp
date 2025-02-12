#include "flow_script_node_set_expression_result_to_variable.hpp"


void FlowScriptNodeSetExpressionResultToVariable::_bind_methods()
{
	BIND_ENUM_CONSTANT(CONNECTION_LIST_ADVANCE);

	ClassDB::bind_method(D_METHOD("set_variable", "name"), &FlowScriptNodeSetExpressionResultToVariable::set_variable);
	ClassDB::bind_method(D_METHOD("get_variable"), &FlowScriptNodeSetExpressionResultToVariable::get_variable);
	ClassDB::bind_method(D_METHOD("set_expression", "text"), &FlowScriptNodeSetExpressionResultToVariable::set_expression);
	ClassDB::bind_method(D_METHOD("get_expression"), &FlowScriptNodeSetExpressionResultToVariable::get_expression);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "variable"), "set_variable", "get_variable");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "expression", PROPERTY_HINT_EXPRESSION), "set_expression", "get_expression");
}


bool FlowScriptNodeSetExpressionResultToVariable::exec_variable_assignment(FlowScriptNodeContext *p_context, const StringName &p_key, const Variant &p_value)
{
	ERR_FAIL_V_MSG(false, "Not implemented. Do not instantiate " + get_class() + " directly.");
}


void FlowScriptNodeSetExpressionResultToVariable::set_variable(const StringName &p_name)
{
	if (p_name == variable)
	{
		return;
	}
	variable = p_name;
	emit_changed();
}


StringName FlowScriptNodeSetExpressionResultToVariable::get_variable() const
{
	return variable;
}


void FlowScriptNodeSetExpressionResultToVariable::set_expression(const String &p_text)
{
	if (p_text == expression)
	{
		return;
	}
	expression = p_text;
	emit_changed();
}


String FlowScriptNodeSetExpressionResultToVariable::get_expression() const
{
	return expression;
}


void FlowScriptNodeSetExpressionResultToVariable::exec_step(FlowScriptNodeContext *p_context)
{
	const Variant result = p_context->get_bridge_ptr()->get_built_in_node_interface()->evaluate_expression(expression);
	if (!exec_variable_assignment(p_context, variable, result))
	{
		ERR_PRINT(vformat("Failed to assign variable \"%s\" to %s.", variable, result.stringify()));
	}
	p_context->advance(FlowScriptNodeOutputConnection(CONNECTION_LIST_ADVANCE, 0));
}


void FlowScriptNodeSetExpressionResultToVariable::set_data_state(const Dictionary &p_data)
{
	set_variable(p_data.get("variable", String()));
	set_expression(p_data.get("expression", String()));
}


Dictionary FlowScriptNodeSetExpressionResultToVariable::get_data_state() const
{
	Dictionary d;
	d["variable"] = variable;
	d["expression"] = expression;
	return d;
}


void FlowScriptNodeSetExpressionResultToVariable::get_output_connection_list_lengths(List<FlowScriptNodeConnectionListLength> *p_lengths) const
{
	p_lengths->push_back(1);
}
