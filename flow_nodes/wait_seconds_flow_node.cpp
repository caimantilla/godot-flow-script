#include "wait_seconds_flow_node.hpp"
#include "scene/scene_string_names.h"


void WaitSecondsFlowNode::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_timer_duration", "seconds"), &WaitSecondsFlowNode::set_timer_duration);
	ClassDB::bind_method(D_METHOD("get_timer_duration"), &WaitSecondsFlowNode::get_timer_duration);

	ClassDB::bind_method(D_METHOD("set_next_flow_node_id", "node_id"), &WaitSecondsFlowNode::set_next_flow_node_id);
	ClassDB::bind_method(D_METHOD("get_next_flow_node_id"), &WaitSecondsFlowNode::get_next_flow_node_id);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "timer_duration", PROPERTY_HINT_NONE, "0.1,500,0.1,suffix:Second(s)", PROPERTY_USAGE_DEFAULT), "set_timer_duration", "get_timer_duration");
	ADD_PROPERTY(PropertyInfo(TYPE_FLOW_NODE_ID, "next_flow_node_id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_READ_ONLY), "set_next_flow_node_id", "get_next_flow_node_id");
}


void WaitSecondsFlowNode::set_timer_duration_no_signal(const float p_seconds)
{
	timer_duration = p_seconds;
}


void WaitSecondsFlowNode::set_timer_duration(const float p_seconds)
{
	float new_seconds = MAX(p_seconds, MIN_DURATION);
	new_seconds = MIN(new_seconds, MAX_DURATION);

	if (new_seconds == timer_duration)
	{
		return;
	}

	timer_duration = new_seconds;
	emit_changed();
}


float WaitSecondsFlowNode::get_timer_duration() const
{
	return timer_duration;
}


void WaitSecondsFlowNode::set_next_flow_node_id(const FlowNodeID p_flow_node_id)
{
	if (p_flow_node_id == next_flow_node_id)
	{
		return;
	}

	next_flow_node_id = p_flow_node_id;
	emit_changed();
}


FlowNodeID WaitSecondsFlowNode::get_next_flow_node_id() const
{
	return next_flow_node_id;
}


void WaitSecondsFlowNode::_execute(FlowNodeExecutionState *p_state)
{
	ERR_FAIL_COND_MSG(timer_duration < MIN_DURATION, vformat("Wait duration must be at least %f seconds.", MIN_DURATION));

	Callable finished_callback = callable_mp(p_state, &FlowNodeExecutionState::finish).bind(next_flow_node_id, Variant());

	Timer *timer = p_state->get_flow_bridge()->create_seconds_timer(timer_duration, true);
	timer->connect(SNAME("timeout"), finished_callback, CONNECT_ONE_SHOT | CONNECT_DEFERRED);
}


void WaitSecondsFlowNode::_on_external_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to)
{
	if (next_flow_node_id == p_from)
	{
		next_flow_node_id = p_to;
	}
}


bool WaitSecondsFlowNode::_is_property_flow_node_reference(const StringName &p_name) const
{
	return p_name == SNAME("next_flow_node_id");
}
