#include "flow_script.hpp"
#include "flow_script_node.hpp"
#include "core/config/engine.h"
#include "core/object/script_language.h"
#include "core/io/resource_loader.h"
#include "core/io/resource_uid.h"


void FlowScript::_bind_methods()
{
	BIND_ENUM_CONSTANT(ENCODED_CONNECTION_FROM_NODE);
	BIND_ENUM_CONSTANT(ENCODED_CONNECTION_FROM_LIST);
	BIND_ENUM_CONSTANT(ENCODED_CONNECTION_FROM_SLOT);
	BIND_ENUM_CONSTANT(ENCODED_CONNECTION_TO_INCLUDE);
	BIND_ENUM_CONSTANT(ENCODED_CONNECTION_TO_NODE);
	BIND_ENUM_CONSTANT(ENCODED_CONNECTION_MAX);

	BIND_ENUM_CONSTANT(ERR_INCLUDE_OK);
	BIND_ENUM_CONSTANT(ERR_INCLUDE_NULL);
	BIND_ENUM_CONSTANT(ERR_INCLUDE_CIRCULAR_REFERENCE);
	BIND_ENUM_CONSTANT(ERR_INCLUDE_SELF);
	BIND_ENUM_CONSTANT(ERR_INCLUDE_ALREADY_PRESENT);

	BIND_ENUM_CONSTANT(ERR_NAME_OK);
	BIND_ENUM_CONSTANT(ERR_NAME_TAKEN);
	BIND_ENUM_CONSTANT(ERR_NAME_INVALID_IDENTIFIER);
	BIND_ENUM_CONSTANT(ERR_NAME_EMPTY);

	ClassDB::bind_method(D_METHOD("clear_references_to_include", "include_id"), &FlowScript::clear_references_to_include);
	ClassDB::bind_method(D_METHOD("clear_references_to_node", "include_id", "node_id"), &FlowScript::clear_references_to_node);
	ClassDB::bind_method(D_METHOD("has_target", "include_id", "node_id"), &FlowScript::has_target);
	ClassDB::bind_method(D_METHOD("get_include_count"), &FlowScript::get_include_count);
	ClassDB::bind_method(D_METHOD("contains_include_flow_script_recursive", "flow_script"), &FlowScript::contains_include_flow_script_recursive);
	ClassDB::bind_method(D_METHOD("can_include_flow_script", "flow_script"), &FlowScript::can_include_flow_script);
	ClassDB::bind_method(D_METHOD("get_can_include_flow_script_error", "flow_script"), &FlowScript::get_can_include_flow_script_error);
	ClassDB::bind_method(D_METHOD("has_include", "include_id"), &FlowScript::has_include);
	ClassDB::bind_method(D_METHOD("get_first_available_include_slot", "from"), &FlowScript::get_first_available_include_slot, DEFVAL(FlowScriptConstants::INCLUDE_ID_MIN));
	ClassDB::bind_method(D_METHOD("add_include_to_first_available_slot", "flow_script"), &FlowScript::add_include_to_first_available_slot);
	ClassDB::bind_method(D_METHOD("remove_include_list", "include_id_list"), &FlowScript::remove_include_list);
	ClassDB::bind_method(D_METHOD("remove_include", "include_id"), &FlowScript::remove_include);
	ClassDB::bind_method(D_METHOD("get_node_count"), &FlowScript::get_node_count);
	ClassDB::bind_method(D_METHOD("is_node_slot_available", "node_id"), &FlowScript::is_node_slot_available);
	ClassDB::bind_method(D_METHOD("has_node", "node_id"), &FlowScript::has_node);
	ClassDB::bind_method(D_METHOD("can_add_node_data", "node_data"), &FlowScript::can_add_node_data);
	ClassDB::bind_method(D_METHOD("set_node_data", "node_id", "node_data"), &FlowScript::set_node_data);
	ClassDB::bind_method(D_METHOD("get_node_data", "node_id"), &FlowScript::get_node_data);
	ClassDB::bind_method(D_METHOD("set_node_name", "node_id", "name"), &FlowScript::set_node_name);
	ClassDB::bind_method(D_METHOD("get_node_name", "node_id"), &FlowScript::get_node_name);
	ClassDB::bind_method(D_METHOD("set_node_rect", "node_id", "rect"), &FlowScript::set_node_rect);
	ClassDB::bind_method(D_METHOD("get_node_rect", "node_id"), &FlowScript::get_node_rect);
	ClassDB::bind_method(D_METHOD("set_node_position", "node_id", "position"), &FlowScript::set_node_position);
	ClassDB::bind_method(D_METHOD("get_node_position", "node_id"), &FlowScript::get_node_position);
	ClassDB::bind_method(D_METHOD("set_node_size", "node_id", "size"), &FlowScript::set_node_size);
	ClassDB::bind_method(D_METHOD("get_node_size", "node_id"), &FlowScript::get_node_size);
	ClassDB::bind_method(D_METHOD("get_node_connection_list_count", "node_id"), &FlowScript::get_node_connection_list_count);
	ClassDB::bind_method(D_METHOD("get_node_connection_list_length", "node_id", "list"), &FlowScript::get_node_connection_list_length);
	ClassDB::bind_method(D_METHOD("set_node_connection", "from_node_id", "list", "slot", "to_include_id", "to_node_id"), &FlowScript::bind_set_node_connection);
	ClassDB::bind_method(D_METHOD("get_node_connection", "from_node_id", "list", "slot"), &FlowScript::bind_get_node_connection);
	ClassDB::bind_method(D_METHOD("remove_node_connection", "node_id", "list", "slot"), &FlowScript::bind_remove_node_connection);
	ClassDB::bind_method(D_METHOD("remove_node_list", "node_id_list"), &FlowScript::remove_node_list);
	ClassDB::bind_method(D_METHOD("remove_node", "node_id"), &FlowScript::remove_node);
	ClassDB::bind_method(D_METHOD("get_first_available_node_slot", "from"), &FlowScript::get_first_available_node_slot, DEFVAL(FlowScriptConstants::NODE_ID_MIN));
	ClassDB::bind_method(D_METHOD("add_node_to_first_available_slot", "node_data"), &FlowScript::add_node_to_first_available_slot);
	ClassDB::bind_method(D_METHOD("set_include_flow_script", "include_id", "flow_script"), &FlowScript::set_include_flow_script);
	ClassDB::bind_method(D_METHOD("get_include_flow_script", "include_id"), &FlowScript::get_include_flow_script);
	ClassDB::bind_method(D_METHOD("set_include_position", "include_id", "pos"), &FlowScript::set_include_position);
	ClassDB::bind_method(D_METHOD("get_include_position", "include_id"), &FlowScript::get_include_position);
	ClassDB::bind_method(D_METHOD("get_every_node_resource_recursive"), &FlowScript::bind_get_every_node_resource_recursive);
	ClassDB::bind_method(D_METHOD("get_every_node_resource_connected_to_node", "origin_node_id", "include_origin"), &FlowScript::bind_get_every_node_resource_connected_to_node, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("get_include_id_list"), &FlowScript::get_include_id_list);
	ClassDB::bind_method(D_METHOD("get_node_id_list"), &FlowScript::get_node_id_list);
	ClassDB::bind_method(D_METHOD("get_node_name_list"), &FlowScript::bind_get_node_name_list);
	ClassDB::bind_method(D_METHOD("get_node_id_by_name", "name"), &FlowScript::get_node_id_by_name);
	ClassDB::bind_method(D_METHOD("has_node_with_name", "name"), &FlowScript::has_node_with_name);
	ClassDB::bind_method(D_METHOD("can_add_node_name", "name"), &FlowScript::can_add_node_name);
	ClassDB::bind_method(D_METHOD("get_can_add_node_name_error", "name"), &FlowScript::get_can_add_node_name_error);
	ClassDB::bind_method(D_METHOD("clear_node_name", "node_id"), &FlowScript::clear_node_name);
	ClassDB::bind_method(D_METHOD("clear_all_node_connections"), &FlowScript::clear_all_node_connections);
	ClassDB::bind_method(D_METHOD("has_node_connection", "connection_info"), &FlowScript::bind_has_node_connection);
	ClassDB::bind_method(D_METHOD("add_node_connection", "connection_info"), &FlowScript::bind_add_node_connection);
	ClassDB::bind_method(D_METHOD("set_node_connection_list", "connections"), &FlowScript::bind_set_node_connection_list);
	ClassDB::bind_method(D_METHOD("get_node_connection_list"), &FlowScript::bind_get_node_connection_list);
	ClassDB::bind_method(D_METHOD("set_encoded_node_connection_list", "connections"), &FlowScript::set_encoded_node_connection_list);
	ClassDB::bind_method(D_METHOD("get_encoded_node_connection_list"), &FlowScript::get_encoded_node_connection_list);
	ClassDB::bind_method(D_METHOD("get_connections_to_target", "include_id", "node_id"), &FlowScript::bind_get_connections_to_target);

	ADD_SIGNAL(MethodInfo("include_changed", PropertyInfo(Variant::INT, "include_id")));
	ADD_SIGNAL(MethodInfo("include_rect_changed", PropertyInfo(Variant::INT, "include_id")));
	ADD_SIGNAL(MethodInfo("include_added", PropertyInfo(Variant::INT, "include_id")));
	ADD_SIGNAL(MethodInfo("removing_include", PropertyInfo(Variant::INT, "include_id")));
	ADD_SIGNAL(MethodInfo("include_removed", PropertyInfo(Variant::INT, "include_id")));
	ADD_SIGNAL(MethodInfo("node_changed", PropertyInfo(Variant::INT, "node_id")));
	ADD_SIGNAL(MethodInfo("node_renamed", PropertyInfo(Variant::INT, "node_id")));
	ADD_SIGNAL(MethodInfo("node_rect_changed", PropertyInfo(Variant::INT, "node_id")));
	ADD_SIGNAL(MethodInfo("node_connection_changed", PropertyInfo(Variant::INT, "node_id"), PropertyInfo(Variant::INT, "list"), PropertyInfo(Variant::INT, "slot"), PropertyInfo(Variant::INT, "old_target_include_id"), PropertyInfo(Variant::INT, "old_target_node_id"), PropertyInfo(Variant::INT, "new_target_include_id"), PropertyInfo(Variant::INT, "new_target_node_id")));
	ADD_SIGNAL(MethodInfo("node_added", PropertyInfo(Variant::INT, "node_id")));
	ADD_SIGNAL(MethodInfo("removing_node", PropertyInfo(Variant::INT, "node_id")));
	ADD_SIGNAL(MethodInfo("node_removed", PropertyInfo(Variant::INT, "node_id")));
}


void FlowScript::_get_property_list(List<PropertyInfo> *p_list) const
{
	update_cache_include_id_list();
	update_cache_node_id_list();
	update_cache_node_name_list();

	for (const FlowScriptIncludeID curr_include_id : cache_include_id_list)
	{
		const String prefix = "includes/" + itos(curr_include_id) + "/";

		p_list->push_back(PropertyInfo(Variant::OBJECT, prefix + "flow_script", PROPERTY_HINT_RESOURCE_TYPE, "FlowScript", PROPERTY_USAGE_NO_EDITOR, "FlowScript"));
		p_list->push_back(PropertyInfo(Variant::VECTOR2I, prefix + "position", PROPERTY_HINT_NONE, String(), PROPERTY_USAGE_NO_EDITOR));
	}

	for (const FlowScriptNodeID curr_node_id : cache_node_id_list)
	{
		const NodeInstance &curr_node = get_node_instance_const(curr_node_id);
		const String prefix = "nodes/" + itos(curr_node_id) + "/";

		p_list->push_back(PropertyInfo(Variant::OBJECT, prefix + "data", PROPERTY_HINT_RESOURCE_TYPE, "FlowScriptNode", PROPERTY_USAGE_NO_EDITOR, "FlowScriptNode"));
		p_list->push_back(PropertyInfo(Variant::STRING_NAME, prefix + "name", PROPERTY_HINT_NONE, String(), map_node_id_to_name.has(curr_node_id) ? PROPERTY_USAGE_NO_EDITOR : PROPERTY_USAGE_NONE));
		p_list->push_back(PropertyInfo(Variant::RECT2I, prefix + "rect", PROPERTY_HINT_NONE, String(), PROPERTY_USAGE_NONE));
		p_list->push_back(PropertyInfo(Variant::VECTOR2I, prefix + "position", PROPERTY_HINT_NONE, String(), PROPERTY_USAGE_NO_EDITOR));
		p_list->push_back(PropertyInfo(Variant::VECTOR2I, prefix + "size", PROPERTY_HINT_NONE, String(), curr_node.should_save_size() ? PROPERTY_USAGE_NO_EDITOR : PROPERTY_USAGE_NONE));
	}

	for (const StringName &curr_node_name : cache_node_name_list)
	{
		const String prefix = "node_names/" + curr_node_name + "/";

		p_list->push_back(PropertyInfo(Variant::INT, prefix + "node_id", PROPERTY_HINT_NONE, String(), PROPERTY_USAGE_NONE));
	}

	p_list->push_back(PropertyInfo(Variant::PACKED_INT32_ARRAY, "_encoded_node_connection_list_v1", PROPERTY_HINT_NONE, String(), PROPERTY_USAGE_NO_EDITOR));
}


bool FlowScript::_set(const StringName &p_name, const Variant &p_value)
{

	if (p_name == SNAME("_encoded_node_connection_list_v1"))
	{
		set_encoded_node_connection_list(p_value);
		return true;
	}

	const String name = p_name;

	if (name.begins_with("includes/"))
	{
		const String include_id_str = name.get_slicec('/', 1);
		ERR_FAIL_COND_V(!include_id_str.is_valid_int(), false);

		const FlowScriptIncludeID include_id = include_id_str.to_int();
		ERR_FAIL_COND_V(!FlowScriptConstants::is_include_id_valid(include_id), false);

		const String include_property = name.get_slicec('/', 2);

		if (include_property == "flow_script")
		{
			set_include_flow_script(include_id, p_value);
			return true;
		}
		else
		{
			ERR_FAIL_COND_V(!has_include(include_id), false);

			if (include_property == "position")
			{
				set_include_position(include_id, p_value);
				return true;
			}
			else
			{
				ERR_FAIL_V(false);
			}
		}
	}
	else if (name.begins_with("nodes/"))
	{
		const String node_id_str = name.get_slicec('/', 1);
		ERR_FAIL_COND_V(!node_id_str.is_valid_int(), false);

		const FlowScriptNodeID node_id = node_id_str.to_int();
		ERR_FAIL_COND_V(!FlowScriptConstants::is_node_id_valid(node_id), false);

		const String node_property = name.get_slicec('/', 2);

		if (node_property == "data")
		{
			set_node_data(node_id, p_value);
			return true;
		}
		else
		{
			ERR_FAIL_COND_V(!has_node(node_id), false);

			if (node_property == "name")
			{
				set_node_name(node_id, p_value);
				return true;
			}
			else if (node_property == "rect")
			{
				set_node_rect(node_id, p_value);
				return true;
			}
			else if (node_property == "position")
			{
				set_node_position(node_id, p_value);
				return true;
			}
			else if (node_property == "size")
			{
				set_node_size(node_id, p_value);
				return true;
			}
			else
			{
				ERR_FAIL_V(false);
			}
		}
	}
	else if (name.begins_with("node_names/"))
	{
		const String node_name = name.get_slicec('/', 1);
		const String node_name_property = name.get_slicec('/', 2);

		if (node_name_property == "node_id")
		{
			const FlowScriptNodeID node_id = p_value;
			ERR_FAIL_COND_V(!has_node(node_id), false);
			set_node_name(node_id, node_name);
			return true;
		}
		else
		{
			ERR_FAIL_V(false);
		}
	}

	return false;
}


bool FlowScript::_get(const StringName &p_name, Variant &r_ret) const
{
	if (p_name == SNAME("_encoded_node_connection_list_v1"))
	{
		r_ret = get_encoded_node_connection_list();
		return true;
	}

	const String name = p_name;

	if (name.begins_with("includes/"))
	{
		const String include_id_str = name.get_slicec('/', 1);
		ERR_FAIL_COND_V(!include_id_str.is_valid_int(), false);

		const FlowScriptIncludeID include_id = include_id_str.to_int();
		ERR_FAIL_COND_V(!has_include(include_id), false);

		const String include_property = name.get_slicec('/', 2);

		if (include_property == "flow_script")
		{
			r_ret = get_include_flow_script(include_id);
			return true;
		}
		else if (include_property == "position")
		{
			r_ret = get_include_position(include_id);
			return true;
		}
		else
		{
			ERR_FAIL_V(false);
		}
	}
	else if (name.begins_with("nodes/"))
	{
		const String node_id_str = name.get_slicec('/', 1);
		ERR_FAIL_COND_V(!node_id_str.is_valid_int(), false);

		const FlowScriptNodeID node_id = node_id_str.to_int();
		ERR_FAIL_COND_V(!has_node(node_id), false);

		const String node_property = name.get_slicec('/', 2);

		if (node_property == "data")
		{
			r_ret = get_node_data(node_id);
			return true;
		}
		else if (node_property == "name")
		{
			r_ret = get_node_name(node_id);
			return true;
		}
		else if (node_property == "rect")
		{
			r_ret = get_node_rect(node_id);
			return true;
		}
		else if (node_property == "position")
		{
			r_ret = get_node_position(node_id);
			return true;
		}
		else if (node_property == "size")
		{
			r_ret = get_node_size(node_id);
			return true;
		}
		else
		{
			ERR_FAIL_V(false);
		}
	}
	else if (name.begins_with("node_names/"))
	{
		const String node_name = name.get_slicec('/', 1);
		const String node_name_property = name.get_slicec('/', 2);

		if (node_name_property == "node_id")
		{
			const FlowScriptNodeID node_id = get_node_id_by_name(node_name);
			ERR_FAIL_COND_V(node_id == FlowScriptConstants::NODE_ID_INVALID, false);

			r_ret = node_id;
			return true;
		}
		else
		{
			ERR_FAIL_V(false);
		}
	}

	return false;
}


void FlowScript::_resource_path_changed()
{
	emit_changed();
}


bool FlowScript::has_target(const FlowScriptNodeReference &p_target) const
{
	if (p_target.include_id == FlowScriptConstants::INCLUDE_ID_INVALID)
	{
		return has_node(p_target.node_id);
	}
	else if (has_include(p_target.include_id))
	{
		return get_include_instance_const(p_target.include_id).flow_script->has_node(p_target.node_id);
	}
	else
	{
		return false;
	}
}


FlowScriptIncludeID FlowScript::get_include_count() const
{
	return map_includes.size();
}


bool FlowScript::has_include(const FlowScriptIncludeID p_include_id) const
{
	return map_includes.has(p_include_id);
}


bool FlowScript::contains_include_flow_script_recursive(const Ref<FlowScript> &p_flow_script) const
{
	for (const KeyValue<FlowScriptIncludeID, IncludeInstance> &E : map_includes)
	{
		if (E.value.flow_script == p_flow_script)
		{
			return true;
		}
		else
		{
			if (E.value.flow_script->contains_include_flow_script_recursive(p_flow_script))
			{
				return true;
			}
		}
	}

	return false;
}


bool FlowScript::can_include_flow_script(const Ref<FlowScript> &p_flow_script) const
{
	return get_can_include_flow_script_error(p_flow_script) == ERR_INCLUDE_OK;
}


FlowScript::IncludeAddError FlowScript::get_can_include_flow_script_error(const Ref<FlowScript> &p_flow_script) const
{
	if (p_flow_script.is_null())
	{
		return ERR_INCLUDE_NULL;
	}
	else if (p_flow_script == this)
	{
		return ERR_INCLUDE_SELF;
	}
	else if (p_flow_script->contains_include_flow_script_recursive(Ref<FlowScript>(this)))
	{
		return ERR_INCLUDE_CIRCULAR_REFERENCE;
	}
	else
	{
		for (const KeyValue<FlowScriptIncludeID, IncludeInstance> &E : map_includes)
		{
			if (E.value.flow_script == p_flow_script)
			{
				return ERR_INCLUDE_ALREADY_PRESENT;
			}
			return ERR_INCLUDE_OK;
		}
	}
}


FlowScript::IncludeInstance &FlowScript::get_include_instance(const FlowScriptIncludeID p_include_id)
{
	DEV_ASSERT(has_include(p_include_id));
	return map_includes[p_include_id];
}


const FlowScript::IncludeInstance &FlowScript::get_include_instance_const(const FlowScriptIncludeID p_include_id) const
{
	DEV_ASSERT(has_include(p_include_id));
	return map_includes[p_include_id];
}


FlowScriptNodeID FlowScript::get_node_count() const
{
	return map_nodes.size();
}


bool FlowScript::is_node_slot_available(const FlowScriptNodeID p_node_id) const
{
	return !has_node(p_node_id) && FlowScriptConstants::is_node_id_valid(p_node_id);
}


bool FlowScript::has_node(const FlowScriptNodeID p_node_id) const
{
	return map_nodes.has(p_node_id);
}


FlowScript::NodeInstance &FlowScript::get_node_instance(const FlowScriptNodeID p_node_id)
{
	DEV_ASSERT(map_nodes.has(p_node_id));
	return map_nodes[p_node_id];
}


const FlowScript::NodeInstance &FlowScript::get_node_instance_const(const FlowScriptNodeID p_node_id) const
{
	DEV_ASSERT(map_nodes.has(p_node_id));
	return map_nodes[p_node_id];
}


bool FlowScript::can_add_node_data(const Ref<FlowScriptNode> &p_data) const
{
	if (p_data.is_null())
	{
		return false;
	}
	else
	{
		for (const KeyValue<FlowScriptNodeID, NodeInstance> &E : map_nodes)
		{
			if (E.value.data == p_data)
			{
				return false;
			}
		}
		return true;
	}
}


void FlowScript::set_node_data(const FlowScriptNodeID p_node_id, const Ref<FlowScriptNode> &p_data)
{
	ERR_FAIL_COND(!FlowScriptConstants::is_node_id_valid(p_node_id));

	if (p_data.is_valid())
	{
		if (map_nodes.has(p_node_id))
		{
			if (map_nodes[p_node_id].data == p_data)
			{
				return;
			}
			set_node_data(p_node_id, Ref<FlowScriptNode>());
		}
		p_data->connect_changed(callable_mp(this, &FlowScript::on_node_changed).bind(p_node_id));

		NodeInstance node_instance = NodeInstance();
		node_instance.data = p_data;
		map_nodes.insert(p_node_id, node_instance);
		cache_node_id_list_dirty = true;

		update_connection_outputs_for_node(p_node_id);
		emit_signal(SNAME("node_added"), p_node_id);
	}
	else
	{
		if (!map_nodes.has(p_node_id))
		{
			return;
		}

		NodeInstance &node_instance = map_nodes[p_node_id];

		emit_signal(SNAME("removing_node"), p_node_id);
		clear_references_to_node(FlowScriptNodeReference::create_same_script_reference(p_node_id));
		node_instance.data->disconnect_changed(callable_mp(this, &FlowScript::on_node_changed));
		map_nodes.erase(p_node_id);
		cache_node_id_list_dirty = true;
		emit_signal(SNAME("node_removed"), p_node_id);
	}

	emit_changed();
}


Ref<FlowScriptNode> FlowScript::get_node_data(const FlowScriptNodeID p_node_id) const
{
	ERR_FAIL_COND_V(!map_nodes.has(p_node_id), Ref<FlowScriptNode>());
	return map_nodes[p_node_id].data;
}


FlowScriptNode *FlowScript::get_node_data_ptr(const FlowScriptNodeID p_node_id) const
{
	ERR_FAIL_COND_V(!map_nodes.has(p_node_id), nullptr);
	return map_nodes[p_node_id].data.ptr();
}


void FlowScript::set_node_rect(const FlowScriptNodeID p_node_id, const Rect2i &p_rect)
{
	ERR_FAIL_COND(!has_node(p_node_id));

	NodeInstance &node_instance = get_node_instance(p_node_id);
	if (node_instance.rect == p_rect)
	{
		return;
	}
	node_instance.rect = p_rect;
	emit_signal(SNAME("node_rect_changed"), p_node_id);
	emit_changed();
}


Rect2i FlowScript::get_node_rect(const FlowScriptNodeID p_node_id) const
{
	ERR_FAIL_COND_V(!has_node(p_node_id), Rect2i());

	return get_node_instance_const(p_node_id).rect;
}


void FlowScript::set_node_position(const FlowScriptNodeID p_node_id, const Point2i &p_position)
{
	ERR_FAIL_COND(!has_node(p_node_id));

	NodeInstance &node_instance = get_node_instance(p_node_id);
	if (node_instance.rect.position == p_position)
	{
		return;
	}
	node_instance.rect.position = p_position;
	emit_signal(SNAME("node_rect_changed"), p_node_id);
	emit_changed();
}


Point2i FlowScript::get_node_position(const FlowScriptNodeID p_node_id) const
{
	ERR_FAIL_COND_V(!has_node(p_node_id), Point2i());

	return get_node_instance_const(p_node_id).rect.position;
}


void FlowScript::set_node_size(const FlowScriptNodeID p_node_id, const Size2i &p_size)
{
	ERR_FAIL_COND(!has_node(p_node_id));

	NodeInstance &node_instance = get_node_instance(p_node_id);
	if (node_instance.rect.size == p_size)
	{
		return;
	}
	node_instance.rect.size = p_size;
	emit_signal(SNAME("node_rect_changed"), p_node_id);
	emit_changed();
}


Size2i FlowScript::get_node_size(const FlowScriptNodeID p_node_id) const
{
	ERR_FAIL_COND_V(!has_node(p_node_id), Size2i());

	return get_node_instance_const(p_node_id).rect.size;
}


FlowScriptNodeConnectionListNo FlowScript::get_node_connection_list_count(const FlowScriptNodeID p_node_id) const
{
	ERR_FAIL_COND_V(!has_node(p_node_id), 0);

	return get_node_instance_const(p_node_id).connection_lists.size();
}


FlowScriptNodeConnectionListLength FlowScript::get_node_connection_list_length(const FlowScriptNodeID p_node_id, const FlowScriptNodeConnectionListNo p_list) const
{
	ERR_FAIL_COND_V(!has_node(p_node_id), 0);

	const NodeInstance &node_instance = get_node_instance_const(p_node_id);
	ERR_FAIL_INDEX_V(p_list, node_instance.connection_lists.size(), 0);

	return node_instance.connection_lists[p_list].size();
}


void FlowScript::set_node_connection(const FlowScriptNodeID p_node_id, const FlowScriptNodeOutputConnection &p_connection, const FlowScriptNodeReference &p_target_node)
{
	ERR_FAIL_COND(!has_node(p_node_id));
	ERR_FAIL_COND(!has_target(p_target_node));

	NodeInstance &node_instance = map_nodes[p_node_id];
	ERR_FAIL_INDEX(p_connection.list, node_instance.connection_lists.size());

	LocalVector<FlowScriptNodeReference, FlowScriptNodeConnectionListSlotNo> &conn_list = node_instance.connection_lists[p_connection.list];
	ERR_FAIL_INDEX(p_connection.slot, conn_list.size());

	FlowScriptNodeReference current_target = conn_list[p_connection.slot];

	if (current_target == p_target_node)
	{
		return;
	}

	cache_node_connections_dirty = true;

	conn_list[p_connection.slot] = p_target_node;

	emit_signal(SNAME("node_connection_changed"), p_node_id, p_connection.list, p_connection.slot, current_target.include_id, current_target.node_id, p_target_node.include_id, p_target_node.node_id);
	emit_changed();
}


FlowScriptNodeReference FlowScript::get_node_connection(const FlowScriptNodeID p_node_id, const FlowScriptNodeOutputConnection &p_connection) const
{
	ERR_FAIL_COND_V(!has_node(p_node_id), FlowScriptNodeReference::create_null_reference());

	const NodeInstance &node_instance = get_node_instance_const(p_node_id);
	ERR_FAIL_INDEX_V(p_connection.list, node_instance.connection_lists.size(), FlowScriptNodeReference::create_null_reference());

	const LocalVector<FlowScriptNodeReference, FlowScriptNodeConnectionListSlotNo> &conn_list = node_instance.connection_lists[p_connection.list];
	ERR_FAIL_INDEX_V(p_connection.slot, conn_list.size(), FlowScriptNodeReference::create_null_reference());

	return conn_list[p_connection.slot];
}


void FlowScript::remove_node_connection(const FlowScriptNodeID p_node_id, const FlowScriptNodeOutputConnection &p_connection)
{
	set_node_connection(p_node_id, p_connection, FlowScriptNodeReference::create_null_reference());
}


bool FlowScript::insert_node_copy(const FlowScriptNodeID p_node_id, const NodeInstance p_node_copy)
{
	ERR_FAIL_COND_V(!p_node_copy.is_valid(), false);
	ERR_FAIL_COND_V(map_nodes.has(p_node_id), false);

	set_node_data(p_node_id, p_node_copy.data);
	ERR_FAIL_COND_V(!map_nodes.has(p_node_id), false);

	NodeInstance &node_instance = map_nodes[p_node_id];
	node_instance.rect = p_node_copy.rect;

	return true;
}


FlowScript::NodeInstance FlowScript::create_node_copy(const FlowScriptNodeID p_node_id)
{
	NodeInstance copy = NodeInstance();

	ERR_FAIL_COND_V_MSG(!map_nodes.has(p_node_id), copy, vformat(RTR("Cannot copy node that does not exist (Slot #%d)"), p_node_id));

	const NodeInstance &origin = map_nodes[p_node_id];

	copy.data = origin.data->duplicate(true);
	copy.rect = origin.rect;

	return copy;
}


bool FlowScript::remove_node_list(const PackedFlowScriptNodeIDArray &p_node_id_list)
{
	ERR_FAIL_COND_V(p_node_id_list.is_empty(), false);
	for (const FlowScriptNodeID id : p_node_id_list)
	{
		ERR_FAIL_COND_V(!remove_node(id), false);
	}
	return true;
}


bool FlowScript::remove_node(const FlowScriptNodeID p_node_id)
{
	set_node_data(p_node_id, Ref<FlowScriptNode>());
	return !has_node(p_node_id);
}


FlowScriptNodeID FlowScript::get_first_available_node_slot(const FlowScriptNodeID p_from) const
{
	const FlowScriptNodeID from = MAX(p_from, FlowScriptConstants::NODE_ID_MIN);

	for (FlowScriptNodeID curr_id = from; curr_id <= FlowScriptConstants::NODE_ID_MAX; curr_id++)
	{
		if (!map_nodes.has(curr_id))
		{
			return curr_id;
		}
	}

	return FlowScriptConstants::NODE_ID_INVALID;
}


FlowScriptNodeID FlowScript::add_node_to_first_available_slot(const Ref<FlowScriptNode> &p_data)
{
	FlowScriptNodeID slot = get_first_available_node_slot();
	ERR_FAIL_COND_V(slot == FlowScriptConstants::NODE_ID_INVALID, FlowScriptConstants::NODE_ID_INVALID);

	set_node_data(slot, p_data);
	ERR_FAIL_COND_V(!has_node(slot), FlowScriptConstants::NODE_ID_INVALID);
	return slot;
}


void FlowScript::clear_references_to_include(const FlowScriptIncludeID p_include_id)
{
	for (const KeyValue<FlowScriptNodeID, NodeInstance> &E : map_nodes)
	{
		const FlowScriptNodeID node_id = E.key;
		const NodeInstance &node_instance = E.value;

		for (FlowScriptNodeConnectionListNo list_no = 0; list_no < node_instance.connection_lists.size(); list_no++)
		{
			const LocalVector<FlowScriptNodeReference, FlowScriptNodeConnectionListSlotNo> &list = node_instance.connection_lists[list_no];

			for (FlowScriptNodeConnectionListSlotNo slot_no = 0; slot_no < list.size(); slot_no++)
			{
				if (p_include_id == list[slot_no].include_id)
				{
					set_node_connection(node_id, FlowScriptNodeOutputConnection::create_connection(list_no, slot_no), FlowScriptNodeReference::create_null_reference());
				}
			}
		}
	}
}


void FlowScript::clear_references_to_node(const FlowScriptNodeReference &p_target)
{
	for (const KeyValue<FlowScriptNodeID, NodeInstance> &E : map_nodes)
	{
		const FlowScriptNodeID node_id = E.key;
		const NodeInstance &node_instance = E.value;

		for (FlowScriptNodeConnectionListNo list_no = 0; list_no < node_instance.connection_lists.size(); list_no++)
		{
			const LocalVector<FlowScriptNodeReference, FlowScriptNodeConnectionListSlotNo> &list = node_instance.connection_lists[list_no];

			for (FlowScriptNodeConnectionListSlotNo slot_no = 0; slot_no < list.size(); slot_no++)
			{
				if (p_target == list[slot_no])
				{
					set_node_connection(node_id, FlowScriptNodeOutputConnection::create_connection(list_no, slot_no), FlowScriptNodeReference::create_null_reference());
				}
			}
		}
	}
}


void FlowScript::set_include_flow_script(const FlowScriptIncludeID p_include_id, const Ref<FlowScript> &p_flow_script)
{
	ERR_FAIL_COND(!FlowScriptConstants::is_include_id_valid(p_include_id));

	if (p_flow_script.is_valid())
	{
		if (map_includes.has(p_include_id))
		{
			if (map_includes[p_include_id].flow_script == p_flow_script)
			{
				return;
			}
			set_include_flow_script(p_include_id, Ref<FlowScript>());
		}
		IncludeInstance include;
		include.flow_script = p_flow_script;
		map_includes.insert(p_include_id, include);
		cache_include_id_list_dirty = true;

		p_flow_script->connect_changed(callable_mp(this, &FlowScript::on_include_changed).bind(p_include_id));
		emit_signal(SNAME("include_added"), p_include_id);
	}
	else
	{
		if (!map_includes.has(p_include_id))
		{
			return;
		}

		IncludeInstance &include = map_includes[p_include_id];
		include.flow_script->disconnect_changed(callable_mp(this, &FlowScript::on_include_changed));

		emit_signal(SNAME("removing_include"), p_include_id);
		clear_references_to_include(p_include_id);
		map_includes.erase(p_include_id);
		cache_include_id_list_dirty = true;
		emit_signal(SNAME("include_removed"), p_include_id);
	}

	emit_changed();
}


Ref<FlowScript> FlowScript::get_include_flow_script(const FlowScriptIncludeID p_include_id) const
{
	ERR_FAIL_COND_V(!has_include(p_include_id), Ref<FlowScript>());
	return get_include_instance_const(p_include_id).flow_script;
}


FlowScriptIncludeID FlowScript::get_first_available_include_slot(const FlowScriptIncludeID p_from) const
{
	const FlowScriptIncludeID from = MAX(p_from, FlowScriptConstants::INCLUDE_ID_MIN);

	for (FlowScriptIncludeID curr_id = from; curr_id <= FlowScriptConstants::INCLUDE_ID_MAX; curr_id++)
	{
		if (!map_includes.has(curr_id))
		{
			return curr_id;
		}
	}

	ERR_FAIL_V_MSG(FlowScriptConstants::INCLUDE_ID_INVALID, RTR("No more include slots available."));
}


FlowScriptIncludeID FlowScript::add_include_to_first_available_slot(const Ref<FlowScript> &p_flow_script)
{
	const FlowScriptIncludeID id = get_first_available_include_slot();
	ERR_FAIL_COND_V(id == FlowScriptConstants::INCLUDE_ID_INVALID, FlowScriptConstants::INCLUDE_ID_INVALID);

	set_include_flow_script(id, p_flow_script);
	ERR_FAIL_COND_V(!has_include(id), FlowScriptConstants::INCLUDE_ID_INVALID);

	return id;
}


bool FlowScript::remove_include_list(const PackedFlowScriptIncludeIDArray &p_include_id_list)
{
	ERR_FAIL_COND_V(p_include_id_list.is_empty(), false);
	for (const FlowScriptIncludeID id : p_include_id_list)
	{
		ERR_FAIL_COND_V(!remove_include(id), false);
	}
	return true;
}


bool FlowScript::remove_include(const FlowScriptIncludeID p_include_id)
{
	set_include_flow_script(p_include_id, Ref<FlowScript>());
	return !has_include(p_include_id);
}


void FlowScript::set_include_position(const FlowScriptIncludeID p_include_id, const Point2i &p_position)
{
	ERR_FAIL_COND(!has_include(p_include_id));

	IncludeInstance &include_instance = get_include_instance(p_include_id);
	if (include_instance.position == p_position)
	{
		return;
	}
	include_instance.position = p_position;
	emit_signal(SNAME("include_rect_changed"), p_include_id);
	emit_changed();
}


Point2i FlowScript::get_include_position(const FlowScriptIncludeID p_include_id) const
{
	ERR_FAIL_COND_V(!has_include(p_include_id), Point2i());
	return map_includes[p_include_id].position;
}


void FlowScript::update_connection_outputs_for_node(const FlowScriptNodeID p_node_id)
{
	ERR_FAIL_COND(!has_node(p_node_id));
	bool changed = false;

	NodeInstance &node_instance = map_nodes[p_node_id];
	List<FlowScriptNodeConnectionListLength> length_list;
	node_instance.data->get_output_connection_list_lengths(&length_list);

	FlowScriptNodeConnectionListNo old_list_count = node_instance.connection_lists.size();
	FlowScriptNodeConnectionListNo new_list_count = length_list.size();

	if (old_list_count != new_list_count)
	{
		changed = true;
		cache_node_connections_dirty = true;
	}

	// Notify for any connections that are being severed.
	for (FlowScriptNodeConnectionListNo list_no = new_list_count; list_no < old_list_count; list_no++)
	{
		const LocalVector<FlowScriptNodeReference, FlowScriptNodeConnectionListSlotNo> &conn_list = node_instance.connection_lists[list_no];

		for (FlowScriptNodeConnectionListSlotNo slot_no = 0; slot_no < conn_list.size(); slot_no++)
		{
			if (conn_list[slot_no].node_id != FlowScriptConstants::NODE_ID_INVALID)
			{
				set_node_connection(p_node_id, FlowScriptNodeOutputConnection::create_connection(list_no, slot_no), FlowScriptNodeReference::create_null_reference());
				// emit_signal(SNAME("node_connection_changed"), p_node_id, list_no, slot_no);
			}
		}
	}

	node_instance.connection_lists.resize(new_list_count);

	FlowScriptNodeConnectionListNo curr_list_no = 0;
	for (const FlowScriptNodeConnectionListLength &desired_length : length_list)
	{
		LocalVector<FlowScriptNodeReference, FlowScriptNodeConnectionListSlotNo> &conn_list = node_instance.connection_lists[curr_list_no];
		if (conn_list.size() != desired_length)
		{
			conn_list.resize(desired_length);
			changed = true;
		}
		curr_list_no++;
	}

	if (changed)
	{
		emit_changed();
	}
}


void FlowScript::on_node_changed(FlowScriptNodeID p_node_id)
{
	update_connection_outputs_for_node(p_node_id);
	emit_signal(SNAME("node_changed"), p_node_id);
	emit_changed();
}


void FlowScript::on_include_changed(FlowScriptIncludeID p_include_id)
{
	emit_signal(SNAME("include_changed"), p_include_id);
	emit_changed();
}


void FlowScript::bind_set_node_connection(const FlowScriptNodeID p_from_node_id, const FlowScriptNodeConnectionListNo p_list, const FlowScriptNodeConnectionListSlotNo p_slot, const FlowScriptIncludeID p_to_include_id, const FlowScriptNodeID p_to_node_id)
{
	set_node_connection(p_from_node_id, FlowScriptNodeOutputConnection::create_connection(p_list, p_slot), FlowScriptNodeReference::create_include_script_reference(p_to_include_id, p_to_node_id));
}


Dictionary FlowScript::bind_get_node_connection(const FlowScriptNodeID p_from_node_id, const FlowScriptNodeConnectionListNo p_list, const FlowScriptNodeConnectionListSlotNo p_slot) const
{
	FlowScriptNodeReference ret_struct = get_node_connection(p_from_node_id, FlowScriptNodeOutputConnection::create_connection(p_list, p_slot));
	return ret_struct.to_dictionary();
}


void FlowScript::bind_remove_node_connection(const FlowScriptNodeID p_from_node_id, const FlowScriptNodeConnectionListNo p_list, const FlowScriptNodeConnectionListSlotNo p_slot)
{
	remove_node_connection(p_from_node_id, FlowScriptNodeOutputConnection::create_connection(p_list, p_slot));
}


TypedArray<FlowScriptNode> FlowScript::bind_get_every_node_resource_recursive() const
{
	List<Ref<FlowScriptNode>> node_list;
	get_every_node_resource_recursive(&node_list);

	TypedArray<FlowScriptNode> ret;
	ret.resize(node_list.size());

	int i = 0;
	for (const Ref<FlowScriptNode> node_ref : node_list)
	{
		ret[i++] = node_ref;
	}

	return ret;
}


TypedArray<FlowScriptNode> FlowScript::bind_get_every_node_resource_connected_to_node(const FlowScriptNodeID p_origin_node_id, const bool p_include_origin) const
{
	List<Ref<FlowScriptNode>> node_list;
	get_every_node_resource_connected_to_node(&node_list, p_origin_node_id, p_include_origin);
	
	TypedArray<FlowScriptNode> ret;
	ret.resize(node_list.size());

	int i = 0;
	for (const Ref<FlowScriptNode> node_ref : node_list)
	{
		ret[i++] = node_ref;
	}

	return ret;
}


void FlowScript::get_every_node_resource_connected_to_node(List<Ref<FlowScriptNode>> *p_list, const FlowScriptNodeID p_origin_node_id, const bool p_include_origin) const
{
	ERR_FAIL_COND_MSG(!has_node(p_origin_node_id), vformat(RTR("Node #%d does not exist."), p_origin_node_id));

	if (p_include_origin)
	{
		p_list->push_back(get_node_data(p_origin_node_id));
	}

	const FlowScript *self_const_ptr = this;
	internal_get_every_node_resource_connected_to_recursive(self_const_ptr, p_origin_node_id, p_list);
}


void FlowScript::get_every_node_resource_recursive(List<Ref<FlowScriptNode>> *p_list) const
{
	const FlowScript *self_ptr = this;
	internal_get_every_node_resource_recursive(self_ptr, p_list);
}


void FlowScript::internal_get_every_node_resource_connected_to_recursive(const FlowScript *p_current_script, const FlowScriptNodeID p_current_origin_node_id, List<Ref<FlowScriptNode>> *p_node_list)
{
	ERR_FAIL_NULL(p_current_script);
	ERR_FAIL_COND(!p_current_script->has_node(p_current_origin_node_id));

	const NodeInstance &origin_node_instance = p_current_script->get_node_instance_const(p_current_origin_node_id);

	for (const LocalVector<FlowScriptNodeReference, FlowScriptNodeConnectionListSlotNo> &conn_list : origin_node_instance.connection_lists)
	{
		for (const FlowScriptNodeReference &target : conn_list)
		{
			if (p_current_script->has_include(target.include_id))
			{
				const Ref<FlowScript> target_fs = p_current_script->map_includes[target.include_id].flow_script;
				ERR_CONTINUE(!target_fs->has_node(target.node_id));
				p_node_list->push_back(target_fs->get_node_data(target.node_id));
				internal_get_every_node_resource_connected_to_recursive(target_fs.ptr(), target.node_id, p_node_list);
			}
			else if (p_current_script->has_node(target.node_id))
			{
				p_node_list->push_back(p_current_script->get_node_data(target.node_id));
				internal_get_every_node_resource_connected_to_recursive(p_current_script, target.node_id, p_node_list);
			}
		}
	}
}


void FlowScript::internal_get_every_node_resource_recursive(const FlowScript *p_current_level, List<Ref<FlowScriptNode>> *p_node_list)
{
	for (const FlowScriptNodeID curr_node_id : p_current_level->get_node_id_list())
	{
		p_node_list->push_back(p_current_level->get_node_data(curr_node_id));
	}
	for (const FlowScriptIncludeID curr_include_id : p_current_level->get_include_id_list())
	{
		internal_get_every_node_resource_recursive(p_current_level->get_include_flow_script(curr_include_id).ptr(), p_node_list);
	}
}


void FlowScript::update_cache_include_id_list() const
{
	if (!cache_include_id_list_dirty)
	{
		return;
	}

	cache_include_id_list_dirty = false;

	int i = 0;
	cache_include_id_list.resize(map_includes.size());

	for (const KeyValue<FlowScriptIncludeID, IncludeInstance> &E : map_includes)
	{
		cache_include_id_list.write[i++] = E.key;
	}

	cache_include_id_list.sort();
}


PackedFlowScriptIncludeIDArray FlowScript::get_include_id_list() const
{
	update_cache_include_id_list();
	return cache_include_id_list;
}


void FlowScript::update_cache_node_id_list() const
{
	if (!cache_node_id_list_dirty)
	{
		return;
	}

	cache_node_id_list_dirty = false;

	int i = 0;
	cache_node_id_list.resize(map_nodes.size());

	for (const KeyValue<FlowScriptNodeID, NodeInstance> &E : map_nodes)
	{
		cache_node_id_list.write[i++] = E.key;
	}

	cache_node_id_list.sort();
}


PackedFlowScriptNodeIDArray FlowScript::get_node_id_list() const
{
	update_cache_node_id_list();
	return cache_node_id_list;
}


void FlowScript::update_cache_node_name_list() const
{
	if (!cache_node_name_list_dirty)
	{
		return;
	}

	cache_node_name_list_dirty = false;

	LocalVector<String> str_list;
	str_list.resize(map_node_name_to_id.size());

	int str_idx = 0;

	for (const KeyValue<StringName, FlowScriptNodeID> &E : map_node_name_to_id)
	{
		str_list[str_idx++] = String(E.key);
	}

	str_list.sort();

	cache_node_name_list.resize(str_list.size());
	cache_node_name_list_bind.resize(str_list.size());

	for (int i = 0; i < str_list.size(); i++)
	{
		const StringName sn = StringName(str_list[i]);
		cache_node_name_list.write[i] = sn;
		cache_node_name_list_bind[i] = sn;
	}
}


Vector<StringName> FlowScript::get_node_name_list() const
{
	update_cache_node_name_list();
	return cache_node_name_list;
}


TypedArray<StringName> FlowScript::bind_get_node_name_list() const
{
	update_cache_node_name_list();
	return cache_node_name_list_bind;
}


FlowScriptNodeID FlowScript::get_node_id_by_name(const StringName &p_name) const
{
	if (map_node_name_to_id.has(p_name))
	{
		return map_node_name_to_id[p_name];
	}
	else
	{
		return FlowScriptConstants::NODE_ID_INVALID;
	}
}


bool FlowScript::has_node_with_name(const StringName &p_name) const
{
	return map_node_name_to_id.has(p_name);
}


bool FlowScript::can_add_node_name(const StringName &p_name) const
{
	return get_can_add_node_name_error(p_name) == ERR_NAME_OK;
}


FlowScript::NodeNameAddError FlowScript::get_can_add_node_name_error(const StringName &p_name) const
{
	if (p_name == StringName())
	{
		return ERR_NAME_EMPTY;
	}
	if (map_node_name_to_id.has(p_name))
	{
		return ERR_NAME_TAKEN;
	}
	const String name = p_name;
	if (!name.is_valid_identifier())
	{
		return ERR_NAME_INVALID_IDENTIFIER;
	}
	return ERR_NAME_OK;
}


void FlowScript::clear_node_name(const FlowScriptNodeID p_node_id)
{
	internal_clear_node_name(p_node_id, true);
}


void FlowScript::internal_clear_node_name(const FlowScriptNodeID p_node_id, const bool p_emit)
{
	ERR_FAIL_COND_MSG(!has_node(p_node_id), vformat(RTR("Node #%d does not exist."), p_node_id));
	ERR_FAIL_COND_MSG(!map_node_id_to_name.has(p_node_id), vformat(RTR("Node #%d has no name."), p_node_id));

	cache_node_name_list_dirty = true;

	map_node_name_to_id.erase(map_node_id_to_name[p_node_id]);
	map_node_id_to_name.erase(p_node_id);

	if (p_emit)
	{
		emit_signal(SNAME("node_renamed"), p_node_id);
		emit_changed();
	}
}


void FlowScript::set_node_name(const FlowScriptNodeID p_node_id, const StringName &p_name)
{
	ERR_FAIL_COND(!has_node(p_node_id));

	const String name = p_name;
	ERR_FAIL_COND_MSG(map_node_name_to_id.has(p_name) && map_node_name_to_id[p_name] != p_node_id, vformat(RTR("Cannot give name \"%s\" to node #%d, as it is already used by node #%d."), name, p_node_id, map_node_name_to_id[p_name]));
	ERR_FAIL_COND_MSG(!name.is_empty() && !name.is_valid_identifier(), vformat(RTR("Invalid name \"%s\" cannot be given to node #%d."), name, p_node_id));

	// Given the above checks, this should mean that the name entered is identical to the one that already exists.
	if (map_node_name_to_id.has(p_name))
	{
		return;
	}

	cache_node_name_list_dirty = true;

	if (map_node_id_to_name.has(p_node_id))
	{
		map_node_name_to_id.erase(map_node_id_to_name[p_node_id]);
		map_node_id_to_name.erase(p_node_id);
	}

	if (!name.is_empty())
	{
		map_node_name_to_id.insert(p_name, p_node_id);
		map_node_id_to_name.insert(p_node_id, p_name);
	}

	emit_signal(SNAME("node_renamed"), p_node_id);
	emit_changed();
}


StringName FlowScript::get_node_name(const FlowScriptNodeID p_node_id) const
{
	if (map_node_id_to_name.has(p_node_id))
	{
		return map_node_id_to_name[p_node_id];
	}
	else
	{
		return StringName();
	}
}


void FlowScript::clear_all_node_connections()
{
	cache_node_connections_dirty = true;

	for (KeyValue<FlowScriptNodeID, NodeInstance> &E : map_nodes)
	{
		for (FlowScriptNodeConnectionListNo list_no = 0; list_no < E.value.connection_lists.size(); list_no++)
		{
			const LocalVector<FlowScriptNodeReference, FlowScriptNodeConnectionListSlotNo> &list = E.value.connection_lists[list_no];
			for (FlowScriptNodeConnectionListSlotNo slot_no = 0; slot_no < list.size(); slot_no++)
			{
				set_node_connection(E.key, FlowScriptNodeOutputConnection::create_connection(list_no, slot_no), FlowScriptNodeReference::create_null_reference());
			}
		}
	}
}


bool FlowScript::has_node_connection(const FlowScriptConnectionInfo &p_connection_info) const
{
	if (map_nodes.has(p_connection_info.from_node_id))
	{
		const NodeInstance &node_instance = map_nodes[p_connection_info.from_node_id];
		if (node_instance.connection_lists.size() > p_connection_info.from_output.list)
		{
			const LocalVector<FlowScriptNodeReference, FlowScriptNodeConnectionListSlotNo> &conn_list = node_instance.connection_lists[p_connection_info.from_output.list];
			if (conn_list.size() > p_connection_info.from_output.slot)
			{
				return conn_list[p_connection_info.from_output.slot] == p_connection_info.target;
			}
		}
	}
	return false;
}


bool FlowScript::bind_has_node_connection(const Dictionary &p_connection_info) const
{
	return has_node_connection(FlowScriptConnectionInfo::create_from_dictionary(p_connection_info));
}


void FlowScript::add_node_connection(const FlowScriptConnectionInfo &p_connection_info)
{
	set_node_connection(p_connection_info.from_node_id, p_connection_info.from_output, p_connection_info.target);
}


void FlowScript::bind_add_node_connection(const Dictionary &p_connection_info)
{
	add_node_connection(FlowScriptConnectionInfo::create_from_dictionary(p_connection_info));
}


void FlowScript::set_node_connection_list(const Vector<FlowScriptConnectionInfo> &p_connections)
{
	clear_all_node_connections();
	for (const FlowScriptConnectionInfo &conn_info : p_connections)
	{
		add_node_connection(conn_info);
	}
}


void FlowScript::bind_set_node_connection_list(const TypedArray<Dictionary> &p_connections)
{
	clear_all_node_connections();
	for (int i = 0; i < p_connections.size(); i++)
	{
		const Dictionary conn_dict = p_connections[i];
		const FlowScriptConnectionInfo conn_info = FlowScriptConnectionInfo::create_from_dictionary(conn_dict);
		add_node_connection(conn_info);
	}
}


Vector<FlowScriptConnectionInfo> FlowScript::get_node_connection_list() const
{
	update_cache_node_connections();
	return cache_node_connection_list;
}


TypedArray<Dictionary> FlowScript::bind_get_node_connection_list() const
{
	update_cache_node_connections();
	return cache_node_connection_list_bind;
}


void FlowScript::set_encoded_node_connection_list(const PackedInt32Array &p_array)
{
	cache_node_connections_dirty = true;

	int base = 0;

	while (base < p_array.size())
	{
		const FlowScriptNodeID from_node_id = p_array[base + ENCODED_CONNECTION_FROM_NODE];
		const FlowScriptNodeOutputConnection from_output = FlowScriptNodeOutputConnection::create_connection(p_array[base + ENCODED_CONNECTION_FROM_LIST], p_array[base + ENCODED_CONNECTION_FROM_SLOT]);
		const FlowScriptNodeReference target = FlowScriptNodeReference::create_include_script_reference(p_array[base + ENCODED_CONNECTION_TO_INCLUDE], p_array[base + ENCODED_CONNECTION_TO_NODE]);

		set_node_connection(from_node_id, from_output, target);

		base += ENCODED_CONNECTION_MAX;
	}
}


PackedInt32Array FlowScript::get_encoded_node_connection_list() const
{
	update_cache_node_connections();
	return cache_encoded_node_connection_list;
}


void FlowScript::get_connections_to_target(const FlowScriptNodeReference &p_target, List<FlowScriptConnectionInfo> *p_connection_list) const
{
	update_cache_node_connections();

	for (const FlowScriptConnectionInfo &conn_info : cache_node_connection_list)
	{
		if (conn_info.target == p_target)
		{
			p_connection_list->push_back(conn_info);
		}
	}
}


TypedArray<Dictionary> FlowScript::bind_get_connections_to_target(const FlowScriptIncludeID p_include_id, const FlowScriptNodeID p_node_id) const
{
	update_cache_node_connections();

	TypedArray<Dictionary> ret;
	FlowScriptNodeReference target = FlowScriptNodeReference::create_include_script_reference(p_include_id, p_node_id);

	for (const FlowScriptConnectionInfo &conn_info : cache_node_connection_list)
	{
		if (conn_info.target == target)
		{
			ret.push_back(conn_info.to_dictionary());
		}
	}

	return ret;
}


void FlowScript::update_cache_node_connections() const
{
	if (!cache_node_connections_dirty)
	{
		return;
	}

	cache_node_connections_dirty = false;
	update_cache_node_id_list();
	cache_node_connection_list.clear();

	for (const FlowScriptNodeID &from_node_id : cache_node_id_list)
	{
		const NodeInstance &from_node_instance = get_node_instance_const(from_node_id);

		for (FlowScriptNodeConnectionListNo list_no = 0; list_no < from_node_instance.connection_lists.size(); list_no++)
		{
			const LocalVector<FlowScriptNodeReference, FlowScriptNodeConnectionListSlotNo> &conn_list = from_node_instance.connection_lists[list_no];

			for (FlowScriptNodeConnectionListSlotNo slot_no = 0; slot_no < conn_list.size(); slot_no++)
			{
				const FlowScriptNodeReference &target = conn_list[slot_no];

				if (has_target(target))
				{
					FlowScriptConnectionInfo conn = FlowScriptConnectionInfo::create(from_node_id, FlowScriptNodeOutputConnection::create_connection(list_no, slot_no), target);
					cache_node_connection_list.push_back(conn);
				}
			}
		}
	}

	cache_node_connection_list_bind.resize(cache_node_connection_list.size());
	cache_encoded_node_connection_list.resize(ENCODED_CONNECTION_MAX * cache_node_connection_list.size());

	for (int conn_idx = 0; conn_idx < cache_node_connection_list.size(); conn_idx++)
	{
		const FlowScriptConnectionInfo &conn = cache_node_connection_list[conn_idx];
		const int encode_origin = ENCODED_CONNECTION_MAX * conn_idx;

		cache_node_connection_list_bind[conn_idx] = conn.to_dictionary();

		cache_encoded_node_connection_list.write[encode_origin + ENCODED_CONNECTION_FROM_NODE] = conn.from_node_id;
		cache_encoded_node_connection_list.write[encode_origin + ENCODED_CONNECTION_FROM_LIST] = conn.from_output.list;
		cache_encoded_node_connection_list.write[encode_origin + ENCODED_CONNECTION_FROM_SLOT] = conn.from_output.slot;
		cache_encoded_node_connection_list.write[encode_origin + ENCODED_CONNECTION_TO_INCLUDE] = conn.target.include_id;
		cache_encoded_node_connection_list.write[encode_origin + ENCODED_CONNECTION_TO_NODE] = conn.target.node_id;
	}
}


void FlowScript::bind_clear_references_to_node(const FlowScriptIncludeID p_include_id, const FlowScriptNodeID p_node_id)
{
	clear_references_to_node(FlowScriptNodeReference::create_include_script_reference(p_include_id, p_node_id));
}


bool FlowScript::bind_has_target(const FlowScriptIncludeID p_include_id, const FlowScriptNodeID p_node_id) const
{
	return has_target(FlowScriptNodeReference::create_include_script_reference(p_include_id, p_node_id));
}


FlowScript::FlowScript()
{
}


void FlowScript::IncludeInstance::set_flow_script(const Ref<FlowScript> &p_flow_script)
{
	flow_script = p_flow_script;
}


Ref<FlowScript> FlowScript::IncludeInstance::get_flow_script() const
{
	return flow_script;
}


void FlowScript::IncludeInstance::set_position(const Point2i &p_pos)
{
	position = p_pos;
}


Point2i FlowScript::IncludeInstance::get_position() const
{
	return position;
}


bool FlowScript::IncludeInstance::is_valid() const
{
	return flow_script.is_valid();
}


void FlowScript::IncludeInstance::set_state(const Dictionary &p_state)
{
	if (p_state.has("flow_script"))
	{
		Ref<FlowScript> new_fs;

		Dictionary d_flow_script = p_state["flow_script"];
		const String fs_path = d_flow_script.get("path", String());
		const String fs_uid = d_flow_script.get("uid", String());

		if (ResourceLoader::exists(fs_uid, "FlowScript"))
		{
			new_fs = ResourceLoader::load(fs_uid, "FlowScript");
		}
		if (new_fs.is_null() && ResourceLoader::exists(fs_path, "FlowScript"))
		{
			new_fs = ResourceLoader::load(fs_path, "FlowScript");
		}

		set_flow_script(new_fs);
	}

	if (p_state.has("position"))
	{
		Dictionary d_pos = p_state["position"];
		set_position(Point2i(d_pos.get("x", 0), d_pos.get("y", 0)));
	}
}


Dictionary FlowScript::IncludeInstance::get_state() const
{
	Dictionary d;

	if (flow_script.is_valid())
	{
		Dictionary d_file;

		const String scr_path = flow_script->get_path();
		const String scr_uid = ResourceUID::get_singleton()->id_to_text(ResourceLoader::get_resource_uid(scr_path));

		if (ResourceLoader::exists(scr_path, "FlowScript"))
		{
			d_file["path"] = scr_path;
		}
		if (ResourceLoader::exists(scr_uid, "FlowScript"))
		{
			d_file["uid"] = scr_uid;
		}

		d["flow_script"] = d_file;
	}

	Dictionary d_pos;
	d_pos["x"] = position.x;
	d_pos["y"] = position.y;
	d["position"] = d_pos;

	return d;
}


void FlowScript::NodeInstance::set_data(const Ref<FlowScriptNode> &p_data)
{
	data = p_data;
}


Ref<FlowScriptNode> FlowScript::NodeInstance::get_data() const
{
	return data;
}


FlowScriptNode *FlowScript::NodeInstance::get_data_ptr() const
{
	return data.ptr();
}


void FlowScript::NodeInstance::set_rect(const Rect2i &p_rect)
{
	rect = p_rect;
}


Rect2i FlowScript::NodeInstance::get_rect() const
{
	return rect;
}


void FlowScript::NodeInstance::set_position(const Point2i &p_pos)
{
	rect.position = p_pos;
}


Point2i FlowScript::NodeInstance::get_position() const
{
	return rect.position;
}


void FlowScript::NodeInstance::set_size(const Size2i &p_size)
{
	rect.size = p_size;
}


Size2i FlowScript::NodeInstance::get_size() const
{
	return rect.size;
}


void FlowScript::NodeInstance::set_state(const Dictionary &p_state)
{
	Ref<FlowScriptNode> new_data;
	if (p_state.has("class"))
	{
		new_data = ClassDB::instantiate(p_state["class"]);
	}
	if (data.is_valid())
	{
		if (p_state.has("script"))
		{
			Ref<Script> scr;
			Dictionary scr_dict = p_state["script"];
			const String scr_path = scr_dict.get("path", String());
			const String scr_uid = scr_dict.get("uid", String());

			if (ResourceLoader::exists(scr_uid, "Script"))
			{
				scr = ResourceLoader::load(scr_uid, "Script");
			}
			if (scr.is_null() && ResourceLoader::exists(scr_path, "Script"))
			{
				scr = ResourceLoader::load(scr_path, "Script");
			}

			new_data->set_script(scr);
		}
		if (p_state.has("data"))
		{
			new_data->set_data_state(p_state["data"]);
		}
	}

	set_data(new_data);

	if (p_state.has("position"))
	{
		const Dictionary pos_dict = p_state["position"];
		set_position(Point2i(pos_dict["x"], pos_dict["y"]));
	}
	if (p_state.has("size"))
	{
		const Dictionary size_dict = p_state["size"];
		set_size(Size2i(size_dict["w"], size_dict["h"]));
	}

	if (p_state.has("connections"))
	{
	}
}


Dictionary FlowScript::NodeInstance::get_state() const
{
	Dictionary d;

	if (data.is_valid())
	{
		d["class"] = data->get_class();
		Ref<Script> scr = data->get_script();
		if (scr.is_valid())
		{
			const String scr_path = scr->get_path();

			if (ResourceLoader::exists(scr_path, "Script"))
			{
				Dictionary script_paths_dict;
				script_paths_dict["path"] = scr_path;

				const ResourceUID::ID scr_uid = ResourceLoader::get_resource_uid(scr_path);
				if (scr_uid != ResourceUID::INVALID_ID)
				{
					script_paths_dict["uid"] = ResourceUID::get_singleton()->id_to_text(scr_uid);
				}

				d["script"] = script_paths_dict;
			}
		}

		d["data"] = data->get_data_state();
	}

	Dictionary pos_dict;
	pos_dict["x"] = rect.position.x;
	pos_dict["y"] = rect.position.y;
	d["position"] = pos_dict;

	if (should_save_size())
	{
		Dictionary size_dict;
		size_dict["w"] = rect.size.width;
		size_dict["h"] = rect.size.height;
		d["size"] = size_dict;
	}

	Array dat_root_conn_list;

	for (FlowScriptNodeConnectionListNo list_no = 0; list_no < connection_lists.size(); list_no++)
	{
		const LocalVector<FlowScriptNodeReference, FlowScriptNodeConnectionListSlotNo> &conn_list = connection_lists[list_no];
		Array dat_sub_conn_list;

		for (FlowScriptNodeConnectionListSlotNo slot_no = 0; slot_no < conn_list.size(); slot_no++)
		{
			const FlowScriptNodeReference &conn = conn_list[slot_no];
			const Dictionary conn_dict = conn.to_dictionary();
			dat_sub_conn_list.push_back(conn_dict);
		}

		dat_root_conn_list.push_back(dat_sub_conn_list);
	}

	return d;
}


bool FlowScript::NodeInstance::is_valid() const
{
	return data.is_valid();
}


bool FlowScript::NodeInstance::should_save_size() const
{
	return rect.size.width > 0 && rect.size.height > 0;
}


void FlowScript::NodeInstance::clear_connections()
{
	connection_lists.clear();
}


void FlowScript::NodeInstance::set_connection_list_count(const FlowScriptNodeConnectionListNo p_count)
{
	connection_lists.resize(p_count);
}


FlowScriptNodeConnectionListNo FlowScript::NodeInstance::get_connection_list_count() const
{
	return connection_lists.size();
}


void FlowScript::NodeInstance::set_connection_list_length(const FlowScriptNodeConnectionListNo p_list, const FlowScriptNodeConnectionListLength p_length)
{
	ERR_FAIL_INDEX(p_list, connection_lists.size());
	connection_lists[p_list].resize(p_length);
}


FlowScriptNodeConnectionListLength FlowScript::NodeInstance::get_connection_list_length(const FlowScriptNodeConnectionListNo p_list) const
{
	ERR_FAIL_INDEX_V(p_list, connection_lists.size(), 0);
	return connection_lists[p_list].size();
}


bool FlowScript::NodeInstance::has_connection(const FlowScriptNodeOutputConnection &p_connection) const
{
	return p_connection.list < connection_lists.size() && p_connection.slot < connection_lists[p_connection.list].size();
}


void FlowScript::NodeInstance::set_connection(const FlowScriptNodeOutputConnection &p_connection, const FlowScriptNodeReference &p_target)
{
	ERR_FAIL_INDEX(p_connection.list, connection_lists.size());
	ERR_FAIL_INDEX(p_connection.slot, connection_lists[p_connection.list].size());

	connection_lists[p_connection.list][p_connection.slot] = p_target;
}


FlowScriptNodeReference FlowScript::NodeInstance::get_connection(const FlowScriptNodeOutputConnection &p_connection) const
{
	ERR_FAIL_INDEX_V(p_connection.list, connection_lists.size(), FlowScriptNodeReference());
	ERR_FAIL_INDEX_V(p_connection.slot, connection_lists[p_connection.list].size(), FlowScriptNodeReference());

	return connection_lists[p_connection.list][p_connection.slot];
}


void FlowScript::NodeInstance::set_connection_include_id(const FlowScriptNodeOutputConnection &p_connection, const FlowScriptIncludeID p_include_id)
{
	set_connection(p_connection, FlowScriptNodeReference::create_include_script_reference(p_include_id, get_connection(p_connection).node_id));
}


FlowScriptIncludeID FlowScript::NodeInstance::get_connection_include_id(const FlowScriptNodeOutputConnection &p_connection) const
{
	return get_connection(p_connection).include_id;
}


void FlowScript::NodeInstance::set_connection_node_id(const FlowScriptNodeOutputConnection &p_connection, const FlowScriptNodeID p_node_id)
{
	set_connection(p_connection, FlowScriptNodeReference::create_include_script_reference(get_connection(p_connection).include_id, p_node_id));
}


FlowScriptNodeID FlowScript::NodeInstance::get_connection_node_id(const FlowScriptNodeOutputConnection &p_connection) const
{
	return get_connection(p_connection).node_id;
}

