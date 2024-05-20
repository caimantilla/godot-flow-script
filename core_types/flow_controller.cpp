#include "flow_controller.hpp"


void FlowController::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_flow_script", "flow_script"), &FlowController::set_flow_script);
	ClassDB::bind_method(D_METHOD("get_flow_script"), &FlowController::get_flow_script);
	ClassDB::bind_method(D_METHOD("set_flow_bridge", "flow_bridge"), &FlowController::set_flow_bridge);
	ClassDB::bind_method(D_METHOD("get_flow_bridge"), &FlowController::get_flow_bridge);
	ClassDB::bind_method(D_METHOD("has_flow_fiber", "flow_fiber_id"), &FlowController::has_flow_fiber);
	ClassDB::bind_method(D_METHOD("get_flow_fiber", "flow_fiber_id"), &FlowController::get_flow_fiber);
	ClassDB::bind_method(D_METHOD("start", "initial_flow_node_name"), &FlowController::start);
	ClassDB::bind_method(D_METHOD("start_with_return_callback", "initial_flow_node_name", "finished_callback"), &FlowController::start_with_return_callback);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "flow_script", PROPERTY_HINT_NODE_TYPE, "FlowScript", PROPERTY_USAGE_DEFAULT), "set_flow_script", "get_flow_script");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "flow_bridge", PROPERTY_HINT_NODE_TYPE, "FlowBridge", PROPERTY_USAGE_DEFAULT), "set_flow_bridge", "get_flow_bridge");

	ADD_SIGNAL(MethodInfo("flow_fiber_finished", PropertyInfo(Variant::INT, "flow_fiber_id"), PropertyInfo(Variant::NIL, "return_value")));
	ADD_SIGNAL(MethodInfo("all_flow_fibers_finished", PropertyInfo(Variant::NIL, "return_value")));
}


void FlowController::set_flow_script(FlowScript *p_flow_script)
{
	flow_script = p_flow_script;
}


FlowScript *FlowController::get_flow_script() const
{
	return flow_script;
}


int FlowController::get_flow_fiber_count() const
{
	return flow_fiber_map.size();
}


void FlowController::set_flow_bridge(FlowBridge *p_flow_bridge)
{
	flow_bridge = p_flow_bridge;
}


FlowBridge *FlowController::get_flow_bridge() const
{
	return flow_bridge;
}


bool FlowController::has_flow_fiber(const int p_flow_fiber_id) const
{
	return flow_fiber_map.has(p_flow_fiber_id);
}


FlowFiber *FlowController::get_flow_fiber(const int p_flow_fiber_id) const
{
	return flow_fiber_map[p_flow_fiber_id];
}


void FlowController::start_with_return_callback(const String &p_initial_flow_node_name, const Callable &p_finished_callback)
{
	ERR_FAIL_NULL(flow_script);

	FlowNode *initial_flow_node = flow_script->get_flow_node_by_name(p_initial_flow_node_name);
	ERR_FAIL_NULL_MSG(initial_flow_node, vformat("Failed to find FlowNode with name \"%s\".", p_initial_flow_node_name));

	FlowNodeID initial_flow_node_id = initial_flow_node->get_flow_node_id();
	create_new_flow_fiber(initial_flow_node_id, p_finished_callback);
}


void FlowController::start(const String &p_initial_flow_node_name)
{
	start_with_return_callback(p_initial_flow_node_name, Callable());
}


void FlowController::emit_flow_fiber_finished(const int flow_fiber_id, const Variant &p_return_value)
{
	emit_signal("flow_fiber_finished", flow_fiber_id, p_return_value);
}


void FlowController::emit_all_flow_fibers_finished(const Variant &p_return_value)
{
	emit_signal("all_flow_fibers_finished", p_return_value);
}


void FlowController::delete_flow_fiber_instance(FlowFiber *p_flow_fiber)
{
	int fiber_id = p_flow_fiber->get_flow_fiber_id();
	flow_fiber_map.erase(fiber_id);

	SceneTree::get_singleton()->queue_delete(p_flow_fiber);
}


void FlowController::update_next_flow_fiber_id()
{
	for (FlowFiberID current_id = FLOW_FIBER_ID_MIN; current_id < FLOW_FIBER_ID_MAX; current_id += 1)
	{
		if (has_flow_fiber(current_id))
		{
			continue;
		}

		next_flow_fiber_id = current_id;
		return;
	}

	next_flow_fiber_id = FLOW_FIBER_ID_NIL;
	ERR_PRINT(vformat("Too many FlowFibers. Do not exceed %d fibers!", FLOW_FIBER_ID_MAX));
}


bool FlowController::is_new_flow_fiber_id_valid(const FlowFiberID p_new_flow_fiber_id) const
{
	return p_new_flow_fiber_id >= FLOW_FIBER_ID_MIN && p_new_flow_fiber_id < FLOW_FIBER_ID_MAX && !has_flow_fiber(p_new_flow_fiber_id);
}


void FlowController::on_flow_fiber_finished(const Variant &p_return_value, const int p_flow_fiber_id)
{
	FlowFiber *fiber = get_flow_fiber(p_flow_fiber_id);
	delete_flow_fiber_instance(fiber);

	if (p_flow_fiber_id < next_flow_fiber_id)
	{
		next_flow_fiber_id = p_flow_fiber_id;
	}

	last_return_value = p_return_value;

	if (get_flow_fiber_count() == 0)
	{
		emit_all_flow_fibers_finished(last_return_value);
	}
	else
	{
		for (const KeyValue<FlowFiberID, FlowFiber *> &other_fiber_map_entry : flow_fiber_map)
		{
			FlowFiber *other_fiber_instance = other_fiber_map_entry.value;

			// WARNING: This might be problematic. Would it be better to defer the call or make it immediately? Perhaps I should rework this altogether?
			// Immediate call:
			other_fiber_instance->on_external_fiber_finished(p_flow_fiber_id);
			// Deferred call:
			// MessageQueue::get_singleton()->push_callable(callable_mp(other_fiber_instance, &FlowFiber::on_external_fiber_finished).bind(p_flow_fiber_id));
		}
	}

	emit_flow_fiber_finished(p_flow_fiber_id, p_return_value);
}


void FlowController::on_flow_fiber_new_fibers_creation_request(const FlowNodeIDArray &p_initial_flow_node_ids, const bool p_wait_fibers_completion, const FlowFiberID p_calling_fiber_id)
{
	ERR_FAIL_COND(!has_flow_fiber(p_calling_fiber_id));
	
	FlowFiber *calling_fiber = get_flow_fiber(p_calling_fiber_id);
	for (const FlowNodeID new_node_start_point_id : p_initial_flow_node_ids)
	{
		FlowFiber *new_fiber = create_new_flow_fiber(new_node_start_point_id, Callable());
		FlowFiberID new_fiber_id = new_fiber->flow_fiber_id;

		calling_fiber->add_resume_dependent_fiber(new_fiber_id);
	}
}


FlowFiber *FlowController::create_new_flow_fiber(const FlowNodeID p_initial_flow_node_id, const Callable &p_finished_callback)
{
	update_next_flow_fiber_id();
	ERR_FAIL_COND_V_MSG(!is_new_flow_fiber_id_valid(next_flow_fiber_id), nullptr, "Failed to create new fiber.");

	FlowFiber *new_fiber = memnew(FlowFiber);
	new_fiber->flow_fiber_id = next_flow_fiber_id;
	new_fiber->flow_bridge = flow_bridge;
	new_fiber->flow_script = flow_script;
	new_fiber->flow_controller = this;
	new_fiber->finished_callback = p_finished_callback;

	FlowFiberID new_fiber_id = new_fiber->get_flow_fiber_id();

	new_fiber->connect(SNAME("finished"), callable_mp(this, &FlowController::on_flow_fiber_finished).bind(new_fiber_id));
	new_fiber->connect(SNAME("new_fibers_requested"), callable_mp(this, &FlowController::on_flow_fiber_new_fibers_creation_request).bind(new_fiber_id));

	flow_fiber_map.insert(new_fiber_id, new_fiber);

	new_fiber->start_using_id(p_initial_flow_node_id);
	return new_fiber;
}


FlowController::FlowController()
{
}


FlowController::~FlowController()
{
}
