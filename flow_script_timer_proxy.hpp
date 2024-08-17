#ifndef FLOW_SCRIPT_TIMER_PROXY_HPP
#define FLOW_SCRIPT_TIMER_PROXY_HPP


#include "core/object/object.h"
#include "core/object/class_db.h"
#include "core/object/gdvirtual.gen.inc"


class FlowScriptTimerProxy : public Object
{
	GDCLASS(FlowScriptTimerProxy, Object);

protected:
	static void _bind_methods();

	GDVIRTUAL1(_start, const real_t &);
	GDVIRTUAL0RC(real_t, _get_time_remaining);
	GDVIRTUAL0RC(bool, _is_active);

public:
	virtual void start(const real_t &p_duration);
	virtual real_t get_time_remaining() const;
	virtual bool is_active() const;

	void emit_finished();
};


#endif // FLOW_SCRIPT_TIMER_PROXY_HPP
