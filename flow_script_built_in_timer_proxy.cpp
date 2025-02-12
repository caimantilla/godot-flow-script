#include "flow_script_built_in_timer_proxy.hpp"


void FlowScriptBuiltInTimerProxy::_bind_methods()
{
	ADD_SIGNAL(MethodInfo("finished"));

	GDVIRTUAL_BIND(_start, "duration");
	GDVIRTUAL_BIND(_get_time_remaining);
	GDVIRTUAL_BIND(_is_active);
}


void FlowScriptBuiltInTimerProxy::start(const real_t &p_duration)
{
	if (!GDVIRTUAL_CALL(_start, p_duration))
	{
		ERR_PRINT("_start must be overriden.");
	}
}


real_t FlowScriptBuiltInTimerProxy::get_time_remaining() const
{
	real_t ret = 0;
	if (!GDVIRTUAL_CALL(_get_time_remaining, ret))
	{
		ERR_PRINT("_get_time_remaining must be overriden.");
	}
	return ret;
}


bool FlowScriptBuiltInTimerProxy::is_active() const
{
	bool ret = false;
	if (!GDVIRTUAL_CALL(_is_active, ret))
	{
		ERR_PRINT("_is_active must be overriden.");
	}
	return ret;
}


bool FlowScriptBuiltInTimerProxy::is_finished() const
{
	return has_finished_already;
}


void FlowScriptBuiltInTimerProxy::finish()
{
	ERR_FAIL_COND_MSG(has_finished_already, RTR("Cannot finish timer multiple times."));
	has_finished_already = true;
	call_deferred(SNAME("emit_signal"), SNAME("finished"));
}
