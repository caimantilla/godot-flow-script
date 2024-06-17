#include "execute_external_flow_script_flow_node.hpp"
#include "core/variant/variant_utility.h"
#include "../core_types/flow_controller.hpp"


void ExecuteExternalFlowScriptFlowNode::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_next_flow_node_id", "node_id"), &ExecuteExternalFlowScriptFlowNode::set_next_flow_node_id);
	ClassDB::bind_method(D_METHOD("get_next_flow_node_id"), &ExecuteExternalFlowScriptFlowNode::get_next_flow_node_id);

	ClassDB::bind_method(D_METHOD("set_wait_external_flow_script_finished", "enabled"), &ExecuteExternalFlowScriptFlowNode::set_wait_external_flow_script_finished);
	ClassDB::bind_method(D_METHOD("is_wait_external_flow_script_finished_enabled"), &ExecuteExternalFlowScriptFlowNode::is_wait_external_flow_script_finished_enabled);

	ClassDB::bind_method(D_METHOD("set_external_flow_script", "flow_script"), &ExecuteExternalFlowScriptFlowNode::set_external_flow_script);
	ClassDB::bind_method(D_METHOD("get_external_flow_script"), &ExecuteExternalFlowScriptFlowNode::get_external_flow_script);

	ClassDB::bind_method(D_METHOD("set_external_call_name", "call_name"), &ExecuteExternalFlowScriptFlowNode::set_external_call_name);
	ClassDB::bind_method(D_METHOD("get_external_call_name"), &ExecuteExternalFlowScriptFlowNode::get_external_call_name);

	ADD_PROPERTY(PropertyInfo(TYPE_FLOW_NODE_ID, "next_flow_node_id"), "set_next_flow_node_id", "get_next_flow_node_id");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "wait_external_flow_script_finished"), "set_wait_external_flow_script_finished", "is_wait_external_flow_script_finished_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "external_flow_script", PROPERTY_HINT_RESOURCE_TYPE, "FlowScript", PROPERTY_USAGE_DEFAULT, "FlowScript"), "set_external_flow_script", "get_external_flow_script");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "external_call_name", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT), "set_external_call_name", "get_external_call_name");
}


void ExecuteExternalFlowScriptFlowNode::_validate_property(PropertyInfo &p_property) const
{
	if (Engine::get_singleton()->is_editor_hint())
	{
		if (p_property.name == "external_call_name")
		{
			PackedStringArray procedure_names = get_external_flow_script_procedure_names();
			if (procedure_names.is_empty())
			{
				p_property.usage = p_property.usage &~ PROPERTY_USAGE_EDITOR;
			}
			else
			{
				p_property.hint = PROPERTY_HINT_ENUM_SUGGESTION;
				p_property.hint_string = String(",").join(procedure_names);
				p_property.usage |= PROPERTY_USAGE_EDITOR;
			}
		}
	}
}


void ExecuteExternalFlowScriptFlowNode::_execute(FlowNodeExecutionState *p_state)
{
	FlowController *sub_controller = memnew(FlowController);
	sub_controller->set_flow_script(external_flow_script);
	sub_controller->set_flow_bridge(p_state->get_flow_bridge());

	// As a child of this node's controller should be a safe spot.
	p_state->get_flow_controller()->add_child(sub_controller);

	if (wait_external_flow_script_finished)
	{
		Callable finished_callback = callable_mp(this, &ExecuteExternalFlowScriptFlowNode::on_awaiting_sub_flow_controller_finished).bind(p_state, sub_controller);
		sub_controller->start_with_return_callback(external_call_name, finished_callback);
	}
	else
	{
		Callable finished_callback = callable_mp(this, &ExecuteExternalFlowScriptFlowNode::on_immediate_sub_flow_controller_finished).bind(sub_controller);
		sub_controller->start_with_return_callback(external_call_name, finished_callback);
		p_state->finish(next_flow_node_id, Variant());
	}
}


bool ExecuteExternalFlowScriptFlowNode::_is_property_flow_node_reference(const StringName &p_name) const
{
	return p_name == SNAME("next_flow_node_id");
}


void ExecuteExternalFlowScriptFlowNode::_on_external_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to)
{
	if (p_from == next_flow_node_id)
	{
		next_flow_node_id = p_to;
	}
}


void ExecuteExternalFlowScriptFlowNode::set_next_flow_node_id(const FlowNodeID p_flow_node_id)
{
	if (p_flow_node_id == next_flow_node_id)
	{
		return;
	}

	next_flow_node_id = p_flow_node_id;
	emit_changed();
}


FlowNodeID ExecuteExternalFlowScriptFlowNode::get_next_flow_node_id() const
{
	return next_flow_node_id;
}


void ExecuteExternalFlowScriptFlowNode::set_wait_external_flow_script_finished(const bool p_enabled)
{
	if (p_enabled == wait_external_flow_script_finished)
	{
		return;
	}

	wait_external_flow_script_finished = p_enabled;
	emit_changed();
}


bool ExecuteExternalFlowScriptFlowNode::is_wait_external_flow_script_finished_enabled() const
{
	return wait_external_flow_script_finished;
}


void ExecuteExternalFlowScriptFlowNode::set_external_flow_script(const Ref<FlowScript> &p_external_flow_script)
{
	bool is_same = p_external_flow_script == external_flow_script;
	external_flow_script = p_external_flow_script;
	
	notify_property_list_changed();

	if (!is_same)
	{
		emit_changed();
	}
}


Ref<FlowScript> ExecuteExternalFlowScriptFlowNode::get_external_flow_script() const
{
	return external_flow_script;
}


void ExecuteExternalFlowScriptFlowNode::set_external_call_name(const String &p_external_call_name)
{
	if (p_external_call_name == external_call_name)
	{
		return;
	}

	external_call_name = p_external_call_name;
	emit_changed();
}


String ExecuteExternalFlowScriptFlowNode::get_external_call_name() const
{
	return external_call_name;
}


PackedStringArray ExecuteExternalFlowScriptFlowNode::get_external_flow_script_procedure_names() const
{
	if (external_flow_script.is_valid())
		return external_flow_script->get_flow_node_name_list();

	return PackedStringArray();
}


void ExecuteExternalFlowScriptFlowNode::on_awaiting_sub_flow_controller_finished(const Variant &p_return_value, FlowNodeExecutionState *p_state, FlowController *p_sub_controller)
{
	p_sub_controller->queue_free();
	p_state->finish(next_flow_node_id, p_return_value);
}


void ExecuteExternalFlowScriptFlowNode::on_immediate_sub_flow_controller_finished(const Variant &p_return_value, FlowController *p_sub_controller)
{
	p_sub_controller->queue_free();
}
