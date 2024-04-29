#include "flow_bridge.hpp"
#include "core/variant/variant_op.h"
#include "core/core_string_names.h"
#include "../singletons/flow_config_manager.hpp"


void FlowBridge::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_READY:
			refresh_local_variable_names();
			break;
		case NOTIFICATION_POSTINITIALIZE:
			init_signals();
			break;
	}
}


void FlowBridge::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_state", "state"), &FlowBridge::set_state);
	ClassDB::bind_method(D_METHOD("get_state"), &FlowBridge::get_state);

	ClassDB::bind_method(D_METHOD("has_local", "variable_name"), &FlowBridge::has_local);
	ClassDB::bind_method(D_METHOD("get_local", "variable_name"), &FlowBridge::get_local);
	ClassDB::bind_method(D_METHOD("set_local", "variable_name", "value"), &FlowBridge::set_local);
	ClassDB::bind_method(D_METHOD("get_local_variable_names"), &FlowBridge::get_local_variable_names);

	ClassDB::bind_method(D_METHOD("has_global", "variable_name"), &FlowBridge::has_global);
	ClassDB::bind_method(D_METHOD("get_global", "variable_name"), &FlowBridge::get_global);
	ClassDB::bind_method(D_METHOD("set_global", "variable_name", "value"), &FlowBridge::set_global);
	ClassDB::bind_method(D_METHOD("get_global_variable_names"), &FlowBridge::get_global_variable_names);

	ClassDB::bind_method(D_METHOD("print_to_console", "message"), &FlowBridge::print_to_console);
	ClassDB::bind_method(D_METHOD("create_seconds_timer", "duration", "auto_start"), &FlowBridge::create_seconds_timer);

	ClassDB::bind_method(D_METHOD("split_multi_line_expression", "expression"), &FlowBridge::split_multi_line_expression);
	ClassDB::bind_method(D_METHOD("evaluate_multi_line_expression", "expression"), &FlowBridge::evaluate_multi_line_expression);
	ClassDB::bind_method(D_METHOD("evaluate_multi_line_boolean_expression", "expression", "succeed_if_expression_empty"), &FlowBridge::evaluate_multi_line_boolean_expression);
	ClassDB::bind_method(D_METHOD("evaluate_expression", "expression"), &FlowBridge::evaluate_expression);

	GDVIRTUAL_BIND(_set_state, "state");
	GDVIRTUAL_BIND(_get_state);
	GDVIRTUAL_BIND(_has_global, "variable_name");
	GDVIRTUAL_BIND(_get_global, "variable_name");
	GDVIRTUAL_BIND(_set_global, "variable_name", "value");
	GDVIRTUAL_BIND(_get_global_variable_names);
	GDVIRTUAL_BIND(_print_to_console, "message");
	GDVIRTUAL_BIND(_create_seconds_timer, "duration", "auto_start");
}


bool FlowBridge::is_local_available_as_property(const String &p_name) const
{
	bool exists;
	get(p_name, &exists);
	return exists;
}


bool FlowBridge::is_property_valid_local(const PropertyInfo &p_info) const
{
	return p_info.usage & PROPERTY_USAGE_SCRIPT_VARIABLE && p_info.usage & PROPERTY_USAGE_STORAGE;
}


void FlowBridge::add_local_variable_name(const String &p_name)
{
	ERR_FAIL_COND(locals_names.has(p_name));
	locals_names.push_back(p_name);
}


void FlowBridge::refresh_local_variable_names()
{
	locals_names.clear();

	List<PropertyInfo> p_list;
	get_property_list(&p_list);

	for (const PropertyInfo &p_info : p_list)
	{
		if (is_property_valid_local(p_info))
		{
			add_local_variable_name(p_info.name);
		}
	}

	for (const KeyValue<String, Variant> &kv : locals_map)
	{
		String key = kv.key;
		add_local_variable_name(key);
	}
}


void FlowBridge::on_property_list_changed()
{
	if (!is_ready())
	{
		return;
	}

	refresh_local_variable_names();
}


void FlowBridge::set_state(const Dictionary &p_state)
{
	ERR_FAIL_COND(!p_state.has("local_variables"));

	Dictionary stored_locals = p_state["local_variables"];
	Array stored_local_keys = stored_locals.keys();

	for (int i = 0; i < stored_local_keys.size(); i++)
	{
		Variant stored_local_key_var = stored_local_keys[i];
		Variant stored_local_value = stored_locals[stored_local_key_var];

		ERR_FAIL_COND(stored_local_key_var.get_type() != Variant::STRING);
		String stored_local_key_str = stored_local_key_var;

		set_local(stored_local_key_str, stored_local_value);
	}

	ERR_FAIL_COND(!p_state.has("custom_state"));

	_set_state(p_state["custom_state"]);
	GDVIRTUAL_CALL(_set_state, p_state["custom_state"]);
}


Dictionary FlowBridge::get_state() const
{
	Dictionary ret;

	Dictionary locals_dictionary;
	for (const String &key_str : locals_names)
	{
		locals_dictionary[key_str] = get_local(key_str);
	}
	ret["local_variables"] = locals_dictionary;

	Dictionary custom_state;

	custom_state.merge(_get_state());

	Dictionary virtual_custom_state;
	GDVIRTUAL_CALL(_get_state, virtual_custom_state);
	custom_state.merge(virtual_custom_state, false);

	ret["custom_state"] = custom_state;

	return ret;
}


bool FlowBridge::has_local(const String &p_name) const
{
	if (!is_local_available_as_property(p_name))
	{
		return locals_map.has(p_name);
	}

	return true;
}


Variant FlowBridge::get_local(const String &p_name) const
{
	bool exists_as_property;
	Variant value = get(p_name, &exists_as_property);

	if (exists_as_property)
	{
		return value;
	}
	else if (locals_map.has(p_name))
	{
		return locals_map[p_name];
	}
	else
	{
		return Variant();
	}
}


void FlowBridge::set_local(const String &p_name, const Variant &p_value)
{
	bool property_assignment_success;
	set(p_name, p_value, &property_assignment_success);

	if (!property_assignment_success)
	{
		if (!locals_map.has(p_name))
		{
			add_local_variable_name(p_name);
		}
		locals_map[p_name] = p_value;
	}
}


PackedStringArray FlowBridge::get_local_variable_names() const
{
	return locals_names;
}


Array FlowBridge::get_local_variable_values() const
{
	Array values;
	values.resize(locals_names.size());

	for (int i = 0; i < locals_names.size(); i++)
	{
		values[i] = get_local(locals_names[i]);
	}

	return values;
}


Array FlowBridge::get_global_variable_values() const
{
	PackedStringArray global_names = get_global_variable_names();

	Array values;
	values.resize(global_names.size());

	for (int i = 0; i < global_names.size(); i++)
	{
		values[i] = get_global(global_names[i]);
	}

	return values;
}


bool FlowBridge::has_global(const String &p_name) const
{
	bool ret;
	
	if (!GDVIRTUAL_REQUIRED_CALL(_has_global, p_name, ret))
	{
		ret = _has_global(p_name);
	}

	return ret;
}


Variant FlowBridge::get_global(const String &p_name) const
{
	Variant ret;

	if (!GDVIRTUAL_CALL(_get_global, p_name, ret))
	{
		ret = _get_global(p_name);
	}

	return ret;
}


void FlowBridge::set_global(const String &p_name, const Variant &p_value)
{
	if (!GDVIRTUAL_CALL(_set_global, p_name, p_value))
	{
		_set_global(p_name, p_value);
	}
}


PackedStringArray FlowBridge::get_global_variable_names() const
{
	PackedStringArray ret;

	if (!GDVIRTUAL_CALL(_get_global_variable_names, ret))
	{
		ret = _get_global_variable_names();
	}

	return ret;
}


bool FlowBridge::has_variable(const String &p_name) const
{
	return has_local(p_name) || has_global(p_name);
}


Variant FlowBridge::get_variable(const String &p_name) const
{
	if (has_local(p_name))
	{
		return get_local(p_name);
	}
	else
	{
		return get_global(p_name);
	}
}


PackedStringArray FlowBridge::get_variable_names() const
{
	PackedStringArray ret;

	ret.append_array(get_local_variable_names());
	ret.append_array(get_global_variable_names());

	return ret;
}


Array FlowBridge::get_variable_values_from_names(PackedStringArray &p_variable_names) const
{
	Array ret;
	ret.resize(p_variable_names.size());

	for (int i = 0; i < p_variable_names.size(); i++)
	{
		ret[i] = get_variable(p_variable_names[i]);
	}

	return ret;
}


Array FlowBridge::get_variable_values() const
{
	PackedStringArray variable_names = get_variable_names();
	return get_variable_values_from_names(variable_names);
}


void FlowBridge::print_to_console(const String &p_text)
{
	if (!GDVIRTUAL_CALL(_print_to_console, p_text))
	{
		_print_to_console(p_text);
	}
}


int FlowBridge::get_elapsed_micro_seconds() const
{
	return OS::get_singleton()->get_ticks_usec();
}


int FlowBridge::get_elapsed_milli_seconds() const
{
	return OS::get_singleton()->get_ticks_msec();
}


PackedStringArray FlowBridge::split_multi_line_expression(const String &p_multi_line_expression) const
{
	PackedStringArray expressions = p_multi_line_expression.split("\n", false);

	for (int i = expressions.size() - 1; i > -1; i--)
	{
		String exp = expressions[i].strip_edges();
		if (exp.is_empty())
		{
			expressions.remove_at(i);
		}
		else
		{
			expressions.set(i, exp);
		}
	}

	return expressions;
}


Array FlowBridge::evaluate_multi_line_expression(const String &p_multi_line_expression)
{
	PackedStringArray variable_names = get_variable_names();
	Array variable_values = get_variable_values_from_names(variable_names);

	Array evaluation_results;
	PackedStringArray expression_strings = split_multi_line_expression(p_multi_line_expression);
	
	if (!expression_strings.is_empty())
	{
		Ref<Expression> expression_obj(memnew(Expression));

		for (String exp_str : expression_strings)
		{
			Error parse_error = expression_obj->parse(exp_str, variable_names);
			if (parse_error != OK)
			{
				ERR_PRINT(vformat("Failed to parse expression: %s", exp_str));
				continue;
			}

			evaluation_results.append(expression_obj->execute(variable_values, this, true, false));
		}
	}

	return evaluation_results;
}


bool FlowBridge::evaluate_multi_line_boolean_expression(const String &p_multi_line_expression, bool p_success_if_empty)
{
	Array results = evaluate_multi_line_expression(p_multi_line_expression);

	if (results.is_empty())
	{
		return p_success_if_empty;
	}

	for (int i = 0; i < results.size(); i++)
	{
		Variant current_result = results[i];
		if (Variant::evaluate(Variant::Operator::OP_NOT_EQUAL, current_result, true))
		{
			return false;
		}
	}

	return true;
}


Variant FlowBridge::evaluate_expression(const String &p_expression)
{
	Array results = evaluate_multi_line_expression(p_expression);

	if (results.is_empty())
	{
		return Variant();
	}

	return results.back();
}


Timer *FlowBridge::create_seconds_timer(float p_duration, bool p_auto_start = true)
{
	Timer *timer;

	if (!GDVIRTUAL_CALL(_create_seconds_timer, p_duration, p_auto_start, timer))
	{
		timer = _create_seconds_timer(p_duration, p_auto_start);
	}

	return timer;
}


Timer *FlowBridge::_create_seconds_timer(float p_duration, bool p_auto_start)
{
	Timer *timer = memnew(Timer);
	Callable timeout_free_callback = Callable::create(timer, SNAME("queue_free"));

	timer->connect(SNAME("timeout"), timeout_free_callback);
	timer->set_name(vformat("%.2f Second Timer", p_duration).replace(".", ","));
	timer->set_wait_time(p_duration);
	timer->set_autostart(p_auto_start);
	timer->set_timer_process_callback(FlowConfigManager::get_singleton()->get_wait_seconds_timer_process_step());
	add_child(timer, true, INTERNAL_MODE_BACK);
	
	return timer;
}


void FlowBridge::_print_to_console(const String &p_text)
{
	print_line_rich(p_text);
}


void FlowBridge::init_signals()
{
	connect(CoreStringNames::get_singleton()->property_list_changed, callable_mp(this, &FlowBridge::on_property_list_changed));
}


FlowBridge::FlowBridge()
{
}
