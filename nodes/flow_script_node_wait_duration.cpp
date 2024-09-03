#include "flow_script_node_wait_duration.hpp"
#include "../flow_script_timer_proxy.hpp"


void FlowScriptNodeWaitDuration::_bind_methods()
{
	BIND_ENUM_CONSTANT(CONNECTION_LIST_ADVANCE);

	BIND_ENUM_CONSTANT(VARIABLE_TIMER);
	BIND_ENUM_CONSTANT(VARIABLE_FINISH_NEXT_STEP);
	BIND_ENUM_CONSTANT(VARIABLE_LOAD_SAVE_FLAG);
	BIND_ENUM_CONSTANT(VARIABLE_LOAD_SAVE_DURATION);
}


void FlowScriptNodeWaitDuration::exec_startup(FlowScriptNodeContext *p_context)
{
	FlowScriptTimerProxy *timer = p_context->get_bridge_ptr()->create_timer_proxy();
	if (timer != nullptr)
	{
		// i don't totally understand why this has to be deferred but whatever
		timer->connect(SNAME("finished"), callable_mp(p_context, &FlowScriptNodeContext::invoke_step), CONNECT_DEFERRED);
	}

	p_context->set_variable(VARIABLE_TIMER, timer);
	p_context->set_variable(VARIABLE_FINISH_NEXT_STEP, false);
	p_context->set_variable(VARIABLE_LOAD_SAVE_FLAG, false);
	p_context->set_variable(VARIABLE_LOAD_SAVE_DURATION, real_t(0));
}


void FlowScriptNodeWaitDuration::exec_step(FlowScriptNodeContext *p_context)
{
	FlowScriptTimerProxy *timer = get_timer(p_context);

	if (timer == nullptr)
	{
		ERR_PRINT(RTR("No valid timer could be found, so execution is impossible. Advancing."));
		p_context->advance(FlowScriptNodeOutputConnection(CONNECTION_LIST_ADVANCE, 0));
		return;
	}

	if (p_context->get_variable(VARIABLE_FINISH_NEXT_STEP))
	{
		p_context->advance(FlowScriptNodeOutputConnection(CONNECTION_LIST_ADVANCE, 0));
	}
	else
	{
		real_t duration;
		if (p_context->get_variable(VARIABLE_LOAD_SAVE_FLAG))
		{
			duration = p_context->get_variable(VARIABLE_LOAD_SAVE_DURATION);
		}
		else
		{
			duration = get_initial_duration(p_context);
		}
		p_context->set_variable(VARIABLE_FINISH_NEXT_STEP, true);
		if (duration > real_t(0))
		{
			timer->start(duration);
		}
		else
		{
			ERR_PRINT(vformat(RTR("Invalid duration: %f. Advancing."), duration));
			p_context->invoke_step();
		}
	}
}


void FlowScriptNodeWaitDuration::exec_cleanup(FlowScriptNodeContext *p_context)
{
	FlowScriptTimerProxy *timer = get_timer(p_context);
	if (timer != nullptr)
	{
		timer->disconnect(SNAME("finished"), callable_mp(p_context, &FlowScriptNodeContext::invoke_step));
		memdelete(timer);
	}
}


void FlowScriptNodeWaitDuration::get_output_connection_list_lengths(List<int64_t> &r_lengths) const
{
	r_lengths.push_back(1);
}


void FlowScriptNodeWaitDuration::set_state(FlowScriptNodeContext *p_context, const Dictionary &p_state)
{
	if (p_state.has("time_remaining"))
	{
		real_t time_remaining = p_state["time_remaining"];
		p_context->set_variable(VARIABLE_LOAD_SAVE_FLAG, true);
		p_context->set_variable(VARIABLE_LOAD_SAVE_DURATION, time_remaining);
	}
}


void FlowScriptNodeWaitDuration::get_state(const FlowScriptNodeContext *p_context, Dictionary &r_state) const
{
	FlowScriptTimerProxy *timer = get_timer(p_context);
	if (timer != nullptr)
	{
		r_state["time_remaining"] = timer->get_time_remaining();
	}
}


FlowScriptTimerProxy *FlowScriptNodeWaitDuration::get_timer(const FlowScriptNodeContext *p_context) const
{
	Object *timer_obj = p_context->get_variable(VARIABLE_TIMER);
	ERR_FAIL_NULL_V(timer_obj, nullptr);
	FlowScriptTimerProxy *timer = Object::cast_to<FlowScriptTimerProxy>(timer_obj);
	ERR_FAIL_NULL_V(timer, nullptr);
	return timer;
}
