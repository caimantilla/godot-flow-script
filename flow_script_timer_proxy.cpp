#include "flow_script_timer_proxy.hpp"


void FlowScriptTimerProxy::_bind_methods()
{
	ADD_SIGNAL(MethodInfo("finished"));

	GDVIRTUAL_BIND(_start, "duration");
	GDVIRTUAL_BIND(_get_time_remaining);
	GDVIRTUAL_BIND(_is_active);
}


void FlowScriptTimerProxy::start(const real_t &p_duration)
{
	if (!GDVIRTUAL_CALL(_start, p_duration))
	{
		ERR_PRINT("_start must be overriden.");
	}
}


real_t FlowScriptTimerProxy::get_time_remaining() const
{
	real_t ret = 0;
	if (!GDVIRTUAL_CALL(_get_time_remaining, ret))
	{
		ERR_PRINT("_get_time_remaining must be overriden.");
	}
	return ret;
}


bool FlowScriptTimerProxy::is_active() const
{
	bool ret = false;
	if (!GDVIRTUAL_CALL(_is_active, ret))
	{
		ERR_PRINT("_is_active must be overriden.");
	}
	return ret;
}


void FlowScriptTimerProxy::emit_finished()
{
	emit_signal(SNAME("finished"));
}
