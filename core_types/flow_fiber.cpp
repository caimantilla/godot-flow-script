#include "flow_fiber.hpp"
#include "flow_controller.hpp"


void FlowFiber::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_flow_fiber_id"), &FlowFiber::get_flow_fiber_id);
	ClassDB::bind_method(D_METHOD("get_flow_bridge"), &FlowFiber::get_flow_bridge);
	ClassDB::bind_method(D_METHOD("get_flow_controller"), &FlowFiber::get_flow_controller);
	ClassDB::bind_method(D_METHOD("get_flow_script"), &FlowFiber::get_flow_script);
	ClassDB::bind_method(D_METHOD("get_current_flow_node"), &FlowFiber::get_current_flow_node);
	ClassDB::bind_method(D_METHOD("get_current_flow_node_id"), &FlowFiber::get_current_flow_node_id);
	ClassDB::bind_method(D_METHOD("get_current_flow_node_execution_state"), &FlowFiber::get_current_flow_node_execution_state);
	ClassDB::bind_method(D_METHOD("get_return_value"), &FlowFiber::get_return_value);
	ClassDB::bind_method(D_METHOD("is_finished"), &FlowFiber::is_finished);
	ClassDB::bind_method(D_METHOD("is_active"), &FlowFiber::is_active);
	ClassDB::bind_method(D_METHOD("set_finished_callback", "callback"), &FlowFiber::set_finished_callback);
	ClassDB::bind_method(D_METHOD("get_finished_callback"), &FlowFiber::get_finished_callback);

	ADD_PROPERTY(PropertyInfo(TYPE_FLOW_FIBER_ID, "flow_fiber_id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "", "get_flow_fiber_id");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "flow_bridge", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE, SNAME("FlowBridge")), "", "get_flow_bridge");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "flow_controller", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE, SNAME("FlowController")), "", "get_flow_controller");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "flow_script", PROPERTY_HINT_RESOURCE_TYPE, "FlowScript", PROPERTY_USAGE_NONE, SNAME("FlowScript")), "", "get_flow_script");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "current_flow_node", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE, SNAME("FlowNode")), "", "get_current_flow_node");
	ADD_PROPERTY(PropertyInfo(TYPE_FLOW_NODE_ID, "current_flow_node_id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "", "get_current_flow_node_id");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "current_flow_node_execution_state", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE, SNAME("FlowNodeExecutionState")), "", "get_current_flow_node_execution_state");
	ADD_PROPERTY(PropertyInfo(Variant::NIL, "return_value", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "", "get_return_value");
	ADD_PROPERTY(PropertyInfo(Variant::CALLABLE, "finished_callback", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_finished_callback", "get_finished_callback");

	ADD_SIGNAL(MethodInfo("finished", PropertyInfo(Variant::NIL, "return_value")));
	ADD_SIGNAL(MethodInfo("new_fibers_requested", PropertyInfo(TYPE_FLOW_NODE_ID_ARRAY, "initial_flow_node_ids"), PropertyInfo(Variant::BOOL, "wait_fibers_completion")));
}


FlowFiberID FlowFiber::get_flow_fiber_id() const
{
	return flow_fiber_id;
}


FlowBridge *FlowFiber::get_flow_bridge() const
{
	return flow_bridge;
}


FlowController *FlowFiber::get_flow_controller() const
{
	return flow_controller;
}


Ref<FlowScript> FlowFiber::get_flow_script() const
{
	return flow_script;
}


FlowNode *FlowFiber::get_current_flow_node() const
{
	return current_flow_node;
}


FlowNodeID FlowFiber::get_current_flow_node_id() const
{
	return current_flow_node->get_flow_node_id();
}


FlowNodeExecutionState *FlowFiber::get_current_flow_node_execution_state() const
{
	return current_flow_node_execution_state;
}


Variant FlowFiber::get_return_value() const
{
	return return_value;
}


bool FlowFiber::is_finished() const
{
	return current_flow_node_execution_state == nullptr;
}


bool FlowFiber::is_active() const
{
	return !is_finished();
}


void FlowFiber::set_finished_callback(const Callable &p_callback)
{
	finished_callback = p_callback;
}


Callable FlowFiber::get_finished_callback() const
{
	return finished_callback;
}


void FlowFiber::add_resume_dependent_fiber(const FlowFiberID p_other_fiber_id)
{
	ERR_FAIL_COND(resume_dependent_fiber_id_set.has(p_other_fiber_id));
	resume_dependent_fiber_id_set.insert(p_other_fiber_id);
}


void FlowFiber::remove_resume_dependent_fiber(const FlowFiberID p_other_fiber_id)
{
	ERR_FAIL_COND(!resume_dependent_fiber_id_set.has(p_other_fiber_id));
	resume_dependent_fiber_id_set.erase(p_other_fiber_id);
}


bool FlowFiber::is_resume_dependent_on_fiber(const FlowFiberID p_other_fiber_id) const
{
	return resume_dependent_fiber_id_set.has(p_other_fiber_id);
}


bool FlowFiber::has_resume_dependencies() const
{
	return !resume_dependent_fiber_id_set.is_empty();
}


void FlowFiber::start_using_id(const FlowNodeID p_initial_flow_node_id)
{
	execute_flow_node_using_id(p_initial_flow_node_id);
}


void FlowFiber::start_using_name(const String &p_initial_flow_node_name)
{
	execute_flow_node_using_name(p_initial_flow_node_name);
}


void FlowFiber::execute_flow_node_using_id(const FlowNodeID p_flow_node_id)
{
	if (current_flow_node_execution_state != nullptr)
	{
		delete_flow_node_execution_state(current_flow_node_execution_state);
	}

	current_flow_node_execution_state = nullptr;
	current_flow_node = nullptr;

	if (!flow_script->has_flow_node(p_flow_node_id))
	{
		WARN_PRINT(vformat("Unabled to execute non-existent FlowNode #%d in fiber #%d.", p_flow_node_id, flow_fiber_id));
		on_finished(Variant());
		return;
	}

	current_flow_node_execution_state = memnew(FlowNodeExecutionState);
	current_flow_node_execution_state->flow_bridge = flow_bridge;
	current_flow_node_execution_state->flow_controller = flow_controller;
	current_flow_node_execution_state->flow_script = flow_script;
	current_flow_node_execution_state->connect(SNAME("finished"), callable_mp(this, &FlowFiber::on_flow_node_finished), CONNECT_ONE_SHOT);
	// I was tearing my hair out trying to figure out why the while loop wasn't working and turns out this was a one-shot connection? why?????? leaving the original line just in case.......!!!!
	// current_flow_node_execution_state->connect(SNAME("new_fibers_requested"), callable_mp(this, &FlowFiber::on_new_fibers_requested), CONNECT_ONE_SHOT);
	current_flow_node_execution_state->connect(SNAME("new_fibers_requested"), callable_mp(this, &FlowFiber::on_new_fibers_requested));

	current_flow_node = flow_script->get_flow_node(p_flow_node_id);
	current_flow_node->execute(current_flow_node_execution_state);
}


void FlowFiber::execute_flow_node_using_name(const String &p_flow_node_name)
{
	ERR_FAIL_COND(!flow_script.is_valid());
	for (const KeyValue<FlowNodeID, FlowNode *> &map_entry : flow_script->flow_node_map)
	{
		FlowNode *current_flow_node = map_entry.value;
		if (p_flow_node_name == current_flow_node->get_flow_node_name())
		{
			execute_flow_node_using_id(current_flow_node->get_flow_node_id());
			return;
		}
	}

	ERR_FAIL_MSG(vformat("Failed to find and execute a FlowNode with the name \"%s\".", p_flow_node_name));
}


void FlowFiber::delete_flow_node_execution_state(FlowNodeExecutionState *p_flow_node_execution_state) const
{
	ERR_FAIL_NULL(p_flow_node_execution_state);
	SceneTree::get_singleton()->queue_delete(p_flow_node_execution_state);
}


void FlowFiber::on_new_fibers_requested(const FlowNodeIDArray &p_initial_flow_node_ids, bool p_wait_fibers_completion)
{
	emit_signal(SNAME("new_fibers_requested"), p_initial_flow_node_ids, p_wait_fibers_completion);
}


void FlowFiber::on_flow_node_finished(const FlowNodeID p_next_flow_node_id, const Variant &p_return_value)
{
	bool should_fiber_finish = false;
	
	if (!flow_script->has_flow_node(p_next_flow_node_id))
	{
		should_fiber_finish = true;
	}

	if (should_fiber_finish)
	{
		on_finished(p_return_value);
	}
	else
	{
		execute_flow_node_using_id(p_next_flow_node_id);
	}
}


void FlowFiber::on_finished(const Variant &p_return_value)
{
	delete_flow_node_execution_state(current_flow_node_execution_state);
	return_value = p_return_value;

	if (finished_callback.is_valid())
	{
		int argument_count = finished_callback.get_argument_count();

		/**
		 * Null, ie. a default Variant, should be returned in the case that the final node doesn't return a meaningful value.
		 * If the callback doesn't take any arguments at all, this is safe to discard.
		*/
		if (argument_count == 0 && p_return_value == Variant())
		{
			finished_callback.call();
		}
		/**
		 * Otherwise, the callback must take the return value of the final node.
		 * Be sure not to pass an invalid callback.
		*/
		else
		{
			finished_callback.call(p_return_value);
		}
	}

	emit_signal(SNAME("finished"), p_return_value);
}


void FlowFiber::on_external_fiber_finished(const FlowFiberID p_other_fiber_id)
{
	if (!is_resume_dependent_on_fiber(p_other_fiber_id))
	{
		return;
	}

	remove_resume_dependent_fiber(p_other_fiber_id);

	if (!has_resume_dependencies())
	{
		if (current_flow_node_execution_state == nullptr)
		{
			ERR_PRINT("Lost flow node execution state ref...?!?!?!?!?!?!?!? WGHAT");
			on_finished(Variant());
		}
		else
		{
			current_flow_node_execution_state->resume();
		}
	}
}
