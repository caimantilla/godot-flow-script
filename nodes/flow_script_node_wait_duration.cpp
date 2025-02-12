#include "flow_script_node_wait_duration.hpp"


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
	FlowScriptBuiltInTimerProxy *timer = p_context->get_bridge_ptr()->get_built_in_node_interface()->create_timer_proxy();
	if (timer != nullptr)
	{
		// i don't totally understand why this has to be deferred but whatever
		timer->connect(SNAME("finished"), callable_mp(p_context, &FlowScriptNodeContext::invoke_step), CONNECT_DEFERRED);
	}

	p_context->set_variable(VARIABLE_TIMER, timer);
	p_context->set_variable(VARIABLE_FINISH_NEXT_STEP, false);
	p_context->set_variable(VARIABLE_LOAD_SAVE_FLAG, false);
	p_context->set_variable(VARIABLE_LOAD_SAVE_DURATION, 0.0);
}


void FlowScriptNodeWaitDuration::exec_step(FlowScriptNodeContext *p_context)
{
	FlowScriptBuiltInTimerProxy *timer = get_timer(p_context);

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
		double duration;
		if (p_context->get_variable(VARIABLE_LOAD_SAVE_FLAG))
		{
			duration = p_context->get_variable(VARIABLE_LOAD_SAVE_DURATION);
		}
		else
		{
			duration = get_initial_duration(p_context);
		}
		p_context->set_variable(VARIABLE_FINISH_NEXT_STEP, true);
		if (duration > 0.0)
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
	FlowScriptBuiltInTimerProxy *timer = get_timer(p_context);
	if (timer != nullptr)
	{
		timer->disconnect(SNAME("finished"), callable_mp(p_context, &FlowScriptNodeContext::invoke_step));
		timer->finish();
	}
}


void FlowScriptNodeWaitDuration::get_output_connection_list_lengths(List<FlowScriptNodeConnectionListLength> *p_lengths) const
{
	p_lengths->push_back(1);
}


void FlowScriptNodeWaitDuration::set_runtime_state(FlowScriptNodeContext *p_context, const Dictionary &p_state)
{
	const double time_remaining = p_state.get("time_remaining", 0.0);
	p_context->set_variable(VARIABLE_LOAD_SAVE_FLAG, true);
	p_context->set_variable(VARIABLE_LOAD_SAVE_DURATION, time_remaining);
}


Dictionary FlowScriptNodeWaitDuration::get_runtime_state(const FlowScriptNodeContext *p_context) const
{
	Dictionary d;
	const FlowScriptBuiltInTimerProxy *timer = get_timer(p_context);
	d["time_remaining"] = timer->get_time_remaining();
	return d;
}


FlowScriptBuiltInTimerProxy *FlowScriptNodeWaitDuration::get_timer(const FlowScriptNodeContext *p_context) const
{
	Object *timer_obj = p_context->get_variable(VARIABLE_TIMER);
	ERR_FAIL_NULL_V(timer_obj, nullptr);
	FlowScriptBuiltInTimerProxy *timer = Object::cast_to<FlowScriptBuiltInTimerProxy>(timer_obj);
	ERR_FAIL_NULL_V(timer, nullptr);
	return timer;
}
