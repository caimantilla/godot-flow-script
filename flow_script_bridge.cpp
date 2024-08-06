#include "flow_script_bridge.hpp"


FlowScriptBridge::ExpressionEvaluatableVariables::ExpressionEvaluatableVariables(FlowScriptBridge *p_bridge)
{
	keys = p_bridge->get_named_variable_keys();
	values = p_bridge->get_named_variable_values();
}


void FlowScriptBridge::_bind_methods()
{
	ClassDB::bind_static_method("FlowScriptBridge", D_METHOD("split_multiline_expression", "expression"), &FlowScriptBridge::split_multiline_expression);
	ClassDB::bind_method(D_METHOD("evaluate_expression", "expression"), &FlowScriptBridge::evaluate_expression);
	ClassDB::bind_method(D_METHOD("evaluate_multiline_expression", "expression"), &FlowScriptBridge::evaluate_multiline_expression);
	ClassDB::bind_method(D_METHOD("evaluate_boolean_expression", "expression", "succeed_if_expression_empty"), &FlowScriptBridge::evaluate_boolean_expression);
	ClassDB::bind_method(D_METHOD("evaluate_multiline_boolean_expression", "expression", "succeed_if_expression_empty"), &FlowScriptBridge::evaluate_multiline_boolean_expression);

	GDVIRTUAL_BIND(_get_expression_base_instance);
	GDVIRTUAL_BIND(_set_local, "key", "value");
	GDVIRTUAL_BIND(_get_local, "key");
	GDVIRTUAL_BIND(_has_local, "key");
	GDVIRTUAL_BIND(_set_global, "key", "value");
	GDVIRTUAL_BIND(_get_global, "key");
	GDVIRTUAL_BIND(_has_global, "key");
	GDVIRTUAL_BIND(_get_named_variable_keys);
	GDVIRTUAL_BIND(_get_named_variable_values);
}


void FlowScriptBridge::_notification(int p_what)
{
	if (p_what == NOTIFICATION_PREDELETE)
	{
		if (dependency_count != 0)
		{
			cancel_free();
			ERR_PRINT(vformat("Failed to delete %s because there are currently %d routines depending on it.", to_string(), dependency_count));
		}
	}
}


PackedStringArray FlowScriptBridge::split_multiline_expression(const String &p_expression)
{
	PackedStringArray expressions = p_expression.split("\n", false);
	
	for (int i = expressions.size() - 1; i > -1; i--)
	{
		if (expressions[i].strip_edges().is_empty())
		{
			expressions.remove_at(i);
		}
	}
	
	return expressions;
}


Variant FlowScriptBridge::evaluate_expression(const String &p_expression)
{
	if (p_expression.strip_edges().is_empty())
	{
		return Variant();
	}
	ExpressionEvaluatableVariables variables = ExpressionEvaluatableVariables(this);
	return internal_evaluate_expression_line(p_expression, get_expression_base_instance(), variables);
}


Array FlowScriptBridge::evaluate_multiline_expression(const String &p_expression)
{
	PackedStringArray expression_list = split_multiline_expression(p_expression);
	Array ret;
	ret.resize(expression_list.size());

	if (!expression_list.is_empty())
	{
		Object *base_instance = get_expression_base_instance();
		ExpressionEvaluatableVariables variables = ExpressionEvaluatableVariables(this);
		for (int i = 0; i < expression_list.size(); i++)
		{
			ret.set(i, internal_evaluate_expression_line(expression_list[i], get_expression_base_instance(), variables));
		}
	}

	return ret;
}


bool FlowScriptBridge::evaluate_boolean_expression(const String &p_expression, const bool p_succeed_if_expression_empty)
{
	if (p_expression.strip_edges().is_empty())
	{
		return p_succeed_if_expression_empty;
	}
	ExpressionEvaluatableVariables variables = ExpressionEvaluatableVariables(this);
	Variant result = internal_evaluate_expression_line(p_expression, get_expression_base_instance(), variables);
	return !(Variant::evaluate(Variant::OP_NOT_EQUAL, result, true));
}


bool FlowScriptBridge::evaluate_multiline_boolean_expression(const String &p_expression, const bool p_succeed_if_expression_empty)
{
	PackedStringArray expression_list = split_multiline_expression(p_expression);
	bool evaluated_any = false;

	if (!expression_list.is_empty())
	{
		Object *base_instance = get_expression_base_instance();
		ExpressionEvaluatableVariables variables = ExpressionEvaluatableVariables(this);
		for (const String &curr_expression : expression_list)
		{
			evaluated_any = true;
			Variant curr_result = internal_evaluate_expression_line(curr_expression, base_instance, variables);
			if (Variant::evaluate(Variant::OP_NOT_EQUAL, curr_result, true))
			{
				return false;
			}
		}
	}

	return evaluated_any || p_succeed_if_expression_empty;
}


Object *FlowScriptBridge::get_expression_base_instance() const
{
	Object *ret = nullptr;
	GDVIRTUAL_CALL(_get_expression_base_instance, ret);
	return ret;
}


void FlowScriptBridge::set_local(const String &p_key, const Variant &p_value)
{
	GDVIRTUAL_CALL(_set_local, p_key, p_value);
}


Variant FlowScriptBridge::get_local(const String &p_key) const
{
	Variant ret;
	GDVIRTUAL_CALL(_get_local, p_key, ret);
	return ret;
}


bool FlowScriptBridge::has_local(const String &p_key) const
{
	bool ret = false;
	GDVIRTUAL_CALL(_has_local, p_key, ret);
	return ret;
}


void FlowScriptBridge::set_global(const String &p_key, const Variant &p_value)
{
	GDVIRTUAL_CALL(_set_global, p_key, p_value);
}


Variant FlowScriptBridge::get_global(const String &p_key) const
{
	Variant ret;
	GDVIRTUAL_CALL(_get_global, p_key, ret);
	return ret;
}


bool FlowScriptBridge::has_global(const String &p_key) const
{
	bool ret = false;
	GDVIRTUAL_CALL(_has_global, p_key, ret);
	return ret;
}


PackedStringArray FlowScriptBridge::get_named_variable_keys() const
{
	PackedStringArray ret;
	GDVIRTUAL_CALL(_get_named_variable_keys, ret);
	return ret;
}


Array FlowScriptBridge::get_named_variable_values() const
{
	Array ret;
	GDVIRTUAL_CALL(_get_named_variable_values, ret);
	return ret;
}


Variant FlowScriptBridge::internal_evaluate_expression_line(const String &p_expression, Object *p_base_instance, const ExpressionEvaluatableVariables &p_variables)
{
	Error err = expression_evaluator.parse(p_expression, p_variables.keys);
	ERR_FAIL_COND_V_MSG(err != OK, Variant(), "Failed to parse expression: " + p_expression);
	Variant ret = expression_evaluator.execute(p_variables.values, p_base_instance, true, false);
	return ret;
}
