#include "flow_script_node_wait_duration_fixed_seconds.hpp"


void FlowScriptNodeWaitDurationFixedSeconds::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_duration", "duration"), &FlowScriptNodeWaitDurationFixedSeconds::set_duration);
	ClassDB::bind_method(D_METHOD("get_duration"), &FlowScriptNodeWaitDurationFixedSeconds::get_duration);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "duration", PROPERTY_HINT_NONE, "suffix:s"), "set_duration", "get_duration");
}


real_t FlowScriptNodeWaitDurationFixedSeconds::get_initial_duration(FlowScriptNodeContext *p_context) const
{
	return duration;
}


void FlowScriptNodeWaitDurationFixedSeconds::set_duration(const real_t &p_duration)
{
	duration = p_duration;
	emit_changed();
}


real_t FlowScriptNodeWaitDurationFixedSeconds::get_duration() const
{
	return duration;
}
