#include "flow_script_built_in_node_interface.hpp"
#include "scene/scene_string_names.h"


void FlowScriptBuiltInNodeInterface::_bind_methods()
{
	GDVIRTUAL_BIND(_set_local, "key", "value");
	GDVIRTUAL_BIND(_get_local, "key");
	GDVIRTUAL_BIND(_has_local, "key");
	GDVIRTUAL_BIND(_set_global, "key", "value");
	GDVIRTUAL_BIND(_get_global, "key");
	GDVIRTUAL_BIND(_has_global, "key");
	GDVIRTUAL_BIND(_evaluate_expression, "expression");
	GDVIRTUAL_BIND(_evaluate_multiline_expression, "expression");
	GDVIRTUAL_BIND(_evaluate_boolean_expression, "expression", "succeed_if_expression_empty");
	GDVIRTUAL_BIND(_evaluate_multiline_boolean_expression, "expression", "succeed_if_expression_empty");
	GDVIRTUAL_BIND(_create_timer_proxy);
}


void FlowScriptBuiltInNodeInterface::set_local(const StringName &p_key, const Variant &p_value)
{
	if (!GDVIRTUAL_CALL(_set_local, p_key, p_value))
	{
		ERR_PRINT("_set_local must be overriden.");
	}
}


Variant FlowScriptBuiltInNodeInterface::get_local(const StringName &p_key) const
{
	Variant ret;
	if (!GDVIRTUAL_CALL(_get_local, p_key, ret))
	{
		ERR_PRINT("_get_local must be overriden.");
	}
	return ret;
}


bool FlowScriptBuiltInNodeInterface::has_local(const StringName &p_key) const
{
	bool ret = false;
	if (!GDVIRTUAL_CALL(_has_local, p_key, ret))
	{
		ERR_PRINT("_has_local must be overriden.");
	}
	return ret;
}


void FlowScriptBuiltInNodeInterface::set_global(const StringName &p_key, const Variant &p_value)
{
	if (!GDVIRTUAL_CALL(_set_global, p_key, p_value))
	{
		ERR_PRINT("_set_global must be overriden.");
	}
}


Variant FlowScriptBuiltInNodeInterface::get_global(const StringName &p_key) const
{
	Variant ret;
	if (!GDVIRTUAL_CALL(_get_global, p_key, ret))
	{
		ERR_PRINT("_get_global must be overriden.");
	}
	return ret;
}


bool FlowScriptBuiltInNodeInterface::has_global(const StringName &p_key) const
{
	bool ret = false;
	if (!GDVIRTUAL_CALL(_has_global, p_key, ret))
	{
		ERR_PRINT("_has_global must be overriden.");
	}
	return ret;
}


Variant FlowScriptBuiltInNodeInterface::evaluate_expression(const String &p_expression)
{
	Variant ret;
	if (!GDVIRTUAL_CALL(_evaluate_expression, p_expression, ret))
	{
		ERR_PRINT("_evaluate_expression must be overriden.");
	}
	return ret;
}


Array FlowScriptBuiltInNodeInterface::evaluate_multiline_expression(const String &p_expression)
{
	Array ret;
	if (!GDVIRTUAL_CALL(_evaluate_multiline_expression, p_expression, ret))
	{
		ERR_PRINT("_evaluate_multiline_expression must be overriden.");
	}
	return ret;
}


bool FlowScriptBuiltInNodeInterface::evaluate_boolean_expression(const String &p_expression, const bool p_succeed_if_expression_empty)
{
	bool ret = p_succeed_if_expression_empty;
	if (!GDVIRTUAL_CALL(_evaluate_boolean_expression, p_expression, p_succeed_if_expression_empty, ret))
	{
		ERR_PRINT("_evaluate_boolean_expression must be overriden.");
	}
	return ret;
}


bool FlowScriptBuiltInNodeInterface::evaluate_multiline_boolean_expression(const String &p_expression, const bool p_succeed_if_expression_empty)
{
	bool ret = p_succeed_if_expression_empty;
	if (!GDVIRTUAL_CALL(_evaluate_multiline_boolean_expression, p_expression, p_succeed_if_expression_empty, ret))
	{
		ERR_PRINT("_evaluate_multiline_boolean_expression must be overriden.");
	}
	return ret;
}


FlowScriptBuiltInTimerProxy *FlowScriptBuiltInNodeInterface::create_timer_proxy()
{
	FlowScriptBuiltInTimerProxy *timer = _create_timer_proxy();
	bool immediate_finish = false; // The timer should finish immediately if there's an error in creation.

	if (timer == nullptr)
	{
		ERR_PRINT("_create_timer_proxy should return a valid timer.");
		timer = memnew(FlowScriptBuiltInTimerProxy);
		immediate_finish = true;
	}

	timer->connect(SceneStringName(finished), callable_mp(this, &FlowScriptBuiltInNodeInterface::on_timer_finished));
	timer_instance_list.push_back(timer);

	if (immediate_finish)
	{
		callable_mp(timer, &FlowScriptBuiltInTimerProxy::finish).call_deferred();
	}

	return timer;
}


FlowScriptBuiltInTimerProxy *FlowScriptBuiltInNodeInterface::_create_timer_proxy()
{
	FlowScriptBuiltInTimerProxy *timer = nullptr;
	if (!GDVIRTUAL_CALL(_create_timer_proxy, timer))
	{
		ERR_PRINT("_create_timer_proxy must be overriden.");
	}
	return timer;
}


void FlowScriptBuiltInNodeInterface::on_timer_finished()
{
	if (!timer_instance_list_clearing_queued)
	{
		timer_instance_list_clearing_queued = true;
		callable_mp(this, &FlowScriptBuiltInNodeInterface::clear_timer_instances).call_deferred();
	}
}


void FlowScriptBuiltInNodeInterface::clear_timer_instances()
{
	timer_instance_list_clearing_queued = false;

	for (int i = timer_instance_list.size() - 1; i > -1; i--)
	{
		FlowScriptBuiltInTimerProxy *timer = timer_instance_list[i];
		if (timer->is_finished())
		{
			memdelete(timer);
			timer_instance_list.remove_at(i);
		}
	}
}
