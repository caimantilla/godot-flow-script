#ifndef FLOW_SCRIPT_BUILT_IN_TIMER_PROXY_HPP
#define FLOW_SCRIPT_BUILT_IN_TIMER_PROXY_HPP


#include "core/object/object.h"
#include "core/object/class_db.h"
#include "core/object/gdvirtual.gen.inc"


class FlowScriptBuiltInTimerProxy : public Object
{
	GDCLASS(FlowScriptBuiltInTimerProxy, Object);

private:
	bool has_finished_already = false;

protected:
	static void _bind_methods();

	GDVIRTUAL1(_start, const real_t &);
	GDVIRTUAL0RC(real_t, _get_time_remaining);
	GDVIRTUAL0RC(bool, _is_active);

public:
	virtual void start(const real_t &p_duration);
	virtual real_t get_time_remaining() const;
	virtual bool is_active() const;

	bool is_finished() const;
	void finish();
};


#endif // FLOW_SCRIPT_BUILT_IN_TIMER_PROXY_HPP
