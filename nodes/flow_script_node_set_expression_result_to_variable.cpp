#include "flow_script_node_set_expression_result_to_variable.hpp"


void FlowScriptNodeSetExpressionResultToVariable::_bind_methods()
{
	BIND_ENUM_CONSTANT(CONNECTION_LIST_ADVANCE);

	ClassDB::bind_method(D_METHOD("set_variable", "key"), &FlowScriptNodeSetExpressionResultToVariable::set_variable);
	ClassDB::bind_method(D_METHOD("get_variable"), &FlowScriptNodeSetExpressionResultToVariable::get_variable);
	ClassDB::bind_method(D_METHOD("set_expression", "text"), &FlowScriptNodeSetExpressionResultToVariable::set_expression);
	ClassDB::bind_method(D_METHOD("get_expression"), &FlowScriptNodeSetExpressionResultToVariable::get_expression);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "variable"), "set_variable", "get_variable");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "expression", PROPERTY_HINT_EXPRESSION), "set_expression", "get_expression");
}


bool FlowScriptNodeSetExpressionResultToVariable::exec_variable_assignment(FlowScriptNodeContext *p_context, const String &p_key, const Variant &p_value)
{
	ERR_FAIL_V_MSG(false, "Not implemented. Do not instantiate " + get_class() + " directly.");
}


void FlowScriptNodeSetExpressionResultToVariable::set_variable(const String &p_key)
{
	if (p_key == variable)
	{
		return;
	}
	variable = p_key;
	emit_changed();
}


String FlowScriptNodeSetExpressionResultToVariable::get_variable() const
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
	Variant result = p_context->get_bridge_ptr()->evaluate_expression(expression);
	if (!exec_variable_assignment(p_context, variable, result))
	{
		ERR_PRINT(vformat("Failed to assign variable \"%s\" to %s.", variable, result.to_json_string()));
	}
	p_context->advance(FlowScriptNodeOutputConnection(CONNECTION_LIST_ADVANCE, 0));
}


void FlowScriptNodeSetExpressionResultToVariable::set_json_data(const Dictionary &p_data)
{
	if (p_data.has("variable"))
	{
		set_variable(p_data["variable"]);
	}
	if (p_data.has("expression"))
	{
		set_expression(p_data["expression"]);
	}
}


void FlowScriptNodeSetExpressionResultToVariable::get_json_data(Dictionary &r_data) const
{
	if (!variable.is_empty())
	{
		r_data["variable"] = variable;
	}
	if (!expression.is_empty())
	{
		r_data["expression"] = expression;
	}
}


void FlowScriptNodeSetExpressionResultToVariable::get_output_connection_list_lengths(List<int64_t> &r_lengths) const
{
	r_lengths.push_back(1);
}
