#include "flow_node.hpp"
#include "core/core_string_names.h"
#include "../singletons/flow_type_db.hpp"


void FlowNode::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_flow_node_id"), &FlowNode::get_flow_node_id);
	ClassDB::bind_method(D_METHOD("set_flow_node_name", "name"), &FlowNode::set_flow_node_name);
	ClassDB::bind_method(D_METHOD("get_flow_node_name"), &FlowNode::get_flow_node_name);
	ClassDB::bind_method(D_METHOD("get_flow_type_id"), &FlowNode::get_flow_type_id);
	ClassDB::bind_method(D_METHOD("get_flow_type_name"), &FlowNode::get_flow_type_name);

	ClassDB::bind_method(D_METHOD("set_flow_graph_position", "position"), &FlowNode::set_flow_graph_position);
	ClassDB::bind_method(D_METHOD("get_flow_graph_position"), &FlowNode::get_flow_graph_position);

	ClassDB::bind_method(D_METHOD("is_flow_node_nameable"), &FlowNode::is_flow_node_nameable);

	ClassDB::bind_method(D_METHOD("emit_changed"), &FlowNode::emit_changed);

	ADD_PROPERTY(PropertyInfo(TYPE_FLOW_NODE_ID, "flow_node_id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "", "get_flow_node_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "flow_node_name", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_flow_node_name", "get_flow_node_name");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "flow_type_id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "", "get_flow_type_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "flow_type_name", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "", "get_flow_type_name");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "flow_graph_position", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_flow_graph_position", "get_flow_graph_position");

	ADD_SIGNAL(MethodInfo("id_changed", PropertyInfo(TYPE_FLOW_NODE_ID, "from"), PropertyInfo(TYPE_FLOW_NODE_ID, "to")));
	ADD_SIGNAL(MethodInfo("name_changed", PropertyInfo(Variant::STRING, "from"), PropertyInfo(Variant::STRING, "to")));
	ADD_SIGNAL(MethodInfo("flow_graph_position_changed", PropertyInfo(Variant::VECTOR2I, "from"), PropertyInfo(Variant::VECTOR2I, "to")));
	ADD_SIGNAL(MethodInfo("changed"));

	GDVIRTUAL_BIND(_execute, "execution_state");
	GDVIRTUAL_BIND(_is_property_flow_node_reference, "property");
	GDVIRTUAL_BIND(_is_property_flow_node_reference_list, "property");
	GDVIRTUAL_BIND(_on_external_flow_node_id_changed, "from", "to");
}


void FlowNode::_get_property_list(List<PropertyInfo> *p_list) const
{
	if (!is_flow_node_nameable())
	{
		return;
	}

	// Make the FlowNode stored and editable if the FlowType is nameable
	for (PropertyInfo &p_info : *p_list)
	{
		if (p_info.name == "flow_node_name")
		{
			p_info.usage = PROPERTY_USAGE_DEFAULT;
		}
	}
	// PropertyInfo p_info_flow_node_name = PropertyInfo(Variant::STRING, "flow_node_name", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE);
	// if (is_flow_node_nameable())
	// {
	// 	p_info_flow_node_name.usage |= PROPERTY_USAGE_DEFAULT;
	// }

	// p_list->push_back(p_info_flow_node_name);
}


// bool FlowNode::_set(const StringName &p_name, const Variant &p_value)
// {
// 	if (p_name == SNAME("flow_node_name"))
// 	{
// 		set_flow_node_name(p_value);
// 		return true;
// 	}

// 	return false;
// }


// bool FlowNode::_get(const StringName &p_name, Variant &r_ret) const
// {
// 	if (p_name == SNAME("flow_node_name"))
// 	{
// 		r_ret = get_flow_node_name();
// 		return true;
// 	}

// 	return false;
// }


// bool FlowNode::_property_can_revert(const StringName &p_name) const
// {
// 	if (p_name == SNAME("flow_node_name"))
// 	{
// 		return true;
// 	}

// 	return false;
// }


// bool FlowNode::_property_get_revert(const StringName &p_name, Variant &r_property) const
// {
// 	if (p_name == SNAME("flow_node_name"))
// 	{
// 		r_property = String();
// 		return true;
// 	}

// 	return false;
// }


void FlowNode::emit_changed()
{
	emit_signal(CoreStringNames::get_singleton()->changed);
}


void FlowNode::set_flow_node_id(const FlowNodeID p_new_id)
{
	if (p_new_id == flow_node_id)
	{
		return;
	}

	int old_id = flow_node_id;
	flow_node_id = p_new_id;

	emit_signal(SNAME("id_changed"), old_id, p_new_id);
}


FlowNodeID FlowNode::get_flow_node_id() const
{
	return flow_node_id;
}


void FlowNode::set_flow_node_name(const String &p_name)
{
	if (p_name == flow_node_name)
	{
		return;
	}

	String old_name = flow_node_name;
	flow_node_name = p_name;

	emit_signal(SNAME("name_changed"), old_name, p_name);
}


String FlowNode::get_flow_node_name() const
{
	return flow_node_name;
}


void FlowNode::set_flow_graph_position_no_signal(Vector2i p_pos)
{
	flow_graph_position = p_pos;
}


void FlowNode::set_flow_graph_position(Vector2i p_pos)
{
	if (p_pos == flow_graph_position)
	{
		return;
	}

	Vector2i previous_position = flow_graph_position;
	flow_graph_position = p_pos;

	emit_signal(SNAME("flow_graph_position_changed"), previous_position, p_pos);
}


Vector2i FlowNode::get_flow_graph_position() const
{
	return flow_graph_position;
}


void FlowNode::_execute(FlowNodeExecutionState *p_state)
{
	ERR_PRINT("_execute() must be overriden.");
	p_state->finish(FLOW_NODE_ID_NIL, Variant()); // By default, just terminate execution
}


bool FlowNode::_is_property_flow_node_reference(const StringName &p_name) const
{
	return false;
}


bool FlowNode::_is_property_flow_node_reference_list(const StringName &p_name) const
{
	return false;
}


void FlowNode::_on_external_flow_node_id_changed(const FlowNodeID p_old_id, const FlowNodeID p_new_id)
{
}


void FlowNode::execute(FlowNodeExecutionState *p_state)
{
	if (!GDVIRTUAL_CALL(_execute, p_state))
	{
		_execute(p_state);
	}
}


bool FlowNode::is_property_flow_node_reference(const StringName &p_name) const
{
	bool ret = _is_property_flow_node_reference(p_name);
	if (!ret)
	{
		GDVIRTUAL_CALL(_is_property_flow_node_reference, p_name, ret);
	}

	return ret;
}


bool FlowNode::is_property_flow_node_reference_list(const StringName &p_name) const
{
	bool ret = _is_property_flow_node_reference_list(p_name);
	if (!ret)
	{
		GDVIRTUAL_CALL(_is_property_flow_node_reference_list, p_name, ret);
	}

	return ret;
}


void FlowNode::on_external_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to)
{
	_on_external_flow_node_id_changed(p_from, p_to);
	GDVIRTUAL_CALL(_on_external_flow_node_id_changed, p_from, p_to);
}


Ref<FlowType> FlowNode::get_flow_type() const
{
	if (FlowTypeDB::get_singleton()->has_type(flow_type_id))
	{
		return FlowTypeDB::get_singleton()->get_type(flow_type_id);
	}
	else
	{
		return Ref<FlowType>();
	}
}


String FlowNode::get_flow_type_id() const
{
	Ref<FlowType> type = get_flow_type();
	if (type.is_valid())
	{
		return type->get_id();
	}
	else
	{
		return "";
	}
}


String FlowNode::get_flow_type_name() const
{
	Ref<FlowType> type = get_flow_type();
	if (type.is_valid())
	{
		return type->get_name();
	}
	else
	{
		return "";
	}
}


bool FlowNode::is_flow_node_nameable() const
{
	Ref<FlowType> type = get_flow_type();
	if (type.is_valid())
	{
		return get_flow_type()->is_nameable();
	}
	else
	{
		return false;
	}
}
