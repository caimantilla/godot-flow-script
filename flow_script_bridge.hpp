#ifndef FLOW_SCRIPT_BRIDGE_HPP
#define FLOW_SCRIPT_BRIDGE_HPP


#include "scene/main/node.h"
#include "core/math/expression.h"


class FlowScriptBridge : public Node
{
	friend class FlowScriptExecutionController;

	GDCLASS(FlowScriptBridge, Node);

private:
	class ExpressionEvaluatableVariables
	{
	public:
		PackedStringArray keys;
		Array values;

		ExpressionEvaluatableVariables(FlowScriptBridge *p_bridge);
	};

	uint32_t dependency_count = 0;
	Expression expression_evaluator;

	Variant internal_evaluate_expression_line(const String &p_expression, Object *p_base_instance, const ExpressionEvaluatableVariables &p_variables);

protected:
	static void _bind_methods();
	void _notification(int p_what);

	// Object *bind_get_expression_base_instance() const;
	// void bind_set_local(const String &p_key, const Variant &p_value);
	// Variant bind_get_local(const String &p_key) const;
	// bool bind_has_local(const String &p_key) const;
	// PackedStringArray bind_get_local_variable_keys() const;
	// void bind_set_global(const String &p_key, const Variant &p_value);
	// Variant bind_get_global(const String &p_key) const;
	// bool bind_has_global(const String &p_key) const;
	// PackedStringArray bind_get_global_variable_keys() const;

	GDVIRTUAL0RC(Object *, _get_expression_base_instance);
	GDVIRTUAL2(_set_local, const String &, const Variant &);
	GDVIRTUAL1RC(Variant, _get_local, const String &);
	GDVIRTUAL1RC(bool, _has_local, const String &);
	GDVIRTUAL2(_set_global, const String &, const Variant &);
	GDVIRTUAL1RC(Variant, _get_global, const String &);
	GDVIRTUAL1RC(bool, _has_global, const String &);
	GDVIRTUAL0RC(PackedStringArray, _get_named_variable_keys);
	GDVIRTUAL0RC(Array, _get_named_variable_values);

public:
	static PackedStringArray split_multiline_expression(const String &p_expression);
	Variant evaluate_expression(const String &p_expression);
	Array evaluate_multiline_expression(const String &p_expression);
	bool evaluate_boolean_expression(const String &p_expression, const bool p_succeed_if_expression_empty);
	bool evaluate_multiline_boolean_expression(const String &p_expression, const bool p_succeed_if_expression_empty);

	virtual Object *get_expression_base_instance() const;
	virtual void set_local(const String &p_key, const Variant &p_value);
	virtual Variant get_local(const String &p_key) const;
	virtual bool has_local(const String &p_key) const;
	virtual void set_global(const String &p_key, const Variant &p_value);
	virtual Variant get_global(const String &p_key) const;
	virtual bool has_global(const String &p_key) const;
	virtual PackedStringArray get_named_variable_keys() const;
	virtual Array get_named_variable_values() const;
};


#endif // FLOW_SCRIPT_BRIDGE_HPP
