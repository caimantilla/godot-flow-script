#ifndef FLOW_BRIDGE_HPP
#define FLOW_BRIDGE_HPP

#include "scene/main/node.h"
#include "scene/main/timer.h"
#include "core/math/expression.h"

class FlowBridge : public Node
{
	GDCLASS(FlowBridge, Node);

private:
	HashMap<String, Variant> locals_map;
	PackedStringArray locals_names;

	void init_signals();

	bool is_local_available_as_property(const String &p_name) const;
	bool is_property_valid_local(const PropertyInfo &p_info) const;
	void add_local_variable_name(const String &p_name);
	void refresh_local_variable_names();
	void on_property_list_changed();

	Array get_variable_values_from_names(PackedStringArray &p_variable_names) const;

protected:
	void _notification(int p_what);
	static void _bind_methods();

	void set_state(const Dictionary &p_state);
	Dictionary get_state() const;

	virtual void _set_state(const Dictionary &p_state) {}
	virtual Dictionary _get_state() const { return Dictionary(); };

	virtual bool _has_global(const String &p_name) const { return false; }
	virtual Variant _get_global(const String &p_name) const { return Variant(); }
	virtual PackedStringArray _get_global_variable_names() const { return PackedStringArray(); }
	virtual void _set_global(const String &p_name, const Variant &p_value) {}
	
	virtual Timer *_create_seconds_timer(float p_duration, bool p_auto_start);
	virtual void _print_to_console(const String &p_text);

	GDVIRTUAL1(_set_state, const Dictionary &);
	GDVIRTUAL0RC(Dictionary, _get_state);
	GDVIRTUAL1RC(bool, _has_global, const String &);
	GDVIRTUAL1RC(Variant, _get_global, const String &);
	GDVIRTUAL2(_set_global, const String &, const Variant &);
	GDVIRTUAL0RC(PackedStringArray, _get_global_variable_names);
	GDVIRTUAL1C(_print_to_console, const Variant &);
	GDVIRTUAL2RC(Timer*, _create_seconds_timer, float, bool);

public:
	bool has_global(const String &p_name) const;
	Variant get_global(const String &p_name) const;
	PackedStringArray get_global_variable_names() const;
	void set_global(const String &p_name, const Variant &p_value);

	Array get_global_variable_values() const;

	bool has_local(const String &p_name) const;
	Variant get_local(const String &p_name) const;
	PackedStringArray get_local_variable_names() const;
	Array get_local_variable_values() const;
	void set_local(const String &p_name, const Variant &p_value);

	bool has_variable(const String &p_name) const;
	Variant get_variable(const String &p_name) const;
	PackedStringArray get_variable_names() const;
	Array get_variable_values() const;

	int get_elapsed_micro_seconds() const;
	int get_elapsed_milli_seconds() const;

	PackedStringArray split_multi_line_expression(const String &p_multi_line_expression) const;
	Array evaluate_multi_line_expression(const String &p_multi_line_expression);
	bool evaluate_multi_line_boolean_expression(const String &p_multi_line_expression, bool p_success_if_empty);
	Variant evaluate_expression(const String &p_expression);

	Timer *create_seconds_timer(float p_duration, bool p_auto_start);
	void print_to_console(const String &p_text);

	FlowBridge();
};

#endif
