#include "flow_script.hpp"
#include "flow_script_node.hpp"
#include "flow_script_node_instance.hpp"


void FlowScript::_bind_methods()
{
	BIND_CONSTANT(NODE_ID_INVALID);
	BIND_CONSTANT(NODE_ID_MIN);
	BIND_CONSTANT(NODE_ID_MAX);

	BIND_CONSTANT(INCLUDE_FLOW_SCRIPT_ID_INVALID);
	BIND_CONSTANT(INCLUDE_FLOW_SCRIPT_MAX);

	ADD_SIGNAL(MethodInfo("include_flow_script_changed", PropertyInfo(Variant::INT, "include_id")));
	ADD_SIGNAL(MethodInfo("include_flow_script_position_changed", PropertyInfo(Variant::INT, "include_id")));
	ADD_SIGNAL(MethodInfo("node_position_changed", PropertyInfo(Variant::INT, "node_id")));
	ADD_SIGNAL(MethodInfo("node_connections_changed", PropertyInfo(Variant::INT, "node_id")));
	ADD_SIGNAL(MethodInfo("node_added", PropertyInfo(Variant::INT, "node_id")));
	ADD_SIGNAL(MethodInfo("removing_node", PropertyInfo(Variant::INT, "node_id")));
	ADD_SIGNAL(MethodInfo("node_removed", PropertyInfo(Variant::INT, "node_id")));
}


void FlowScript::_get_property_list(List<PropertyInfo> *p_list) const
{
	for (FlowScriptIncludeID incl_id = 0; incl_id < INCLUDE_FLOW_SCRIPT_MAX; incl_id++)
	{
		if (script_includes[incl_id].is_valid())
		{
			String prefix = "includes/" + itos(incl_id) + "/";
			p_list->push_back(PropertyInfo(Variant::OBJECT, prefix + "flow_script", PROPERTY_HINT_RESOURCE_TYPE, "FlowScript", PROPERTY_USAGE_NO_EDITOR, "FlowScript"));
			p_list->push_back(PropertyInfo(Variant::VECTOR2I, prefix + "position", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR));
		}
	}
	for (const KeyValue<FlowScriptNodeID, FlowScriptNodeInstance> &E : node_map)
	{
		String prefix = "nodes/" + itos(E.key) + "/";
		p_list->push_back(PropertyInfo(Variant::OBJECT, prefix + "node", PROPERTY_HINT_RESOURCE_TYPE, "FlowScriptNode", PROPERTY_USAGE_NO_EDITOR, "FlowScriptNode"));
		p_list->push_back(PropertyInfo(Variant::VECTOR2I, prefix + "position", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR));
	}
	p_list->push_back(PropertyInfo(Variant::PACKED_INT32_ARRAY, "nodes/connections", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR));
}


bool FlowScript::_set(const StringName &p_name, const Variant &p_value)
{
	String name = p_name;
	if (name.begins_with("nodes/"))
	{
		String identifier_str = name.get_slicec('/', 1);
		if (identifier_str == "connections")
		{
			ERR_FAIL_COND_V(p_value.get_type() != Variant::PACKED_INT32_ARRAY, false);
			set_graph_connection_data(p_value);
			return true;
		}
		else if (identifier_str.is_valid_int())
		{
			FlowScriptNodeID node_id = identifier_str.to_int();
			if (is_node_slot_available(node_id))
			{
				node_map.insert(node_id, FlowScriptNodeInstance());
			}
			ERR_FAIL_COND_V(!node_map.has(node_id), false);
			String node_instance_property = name.get_slicec('/', 2);
			if (node_instance_property == "node")
			{
				ERR_FAIL_COND(p_value.get_type() != Variant::OBJECT);
				node_map.get(node_id).set_node(p_value);
			}
			else if (node_instance_property == "position")
			{
				ERR_FAIL_COND(p_value.get_type() != Variant::VECTOR2I);
				node_map.get(node_id).position = p_value;
			}
		}
	}
	return false;
}


bool FlowScript::_get(const StringName &p_name, Variant &r_ret) const
{
	String name = p_name;
	if (name.begins_with("nodes/"))
	{
		String identifier_str = name.get_slicec('/', 1);
		if (identifier_str == "connections")
		{
			r_ret = get_graph_connection_data();
			return true;
		}
		else if (identifier_str.is_valid_int())
		{
			FlowScriptNodeID node_id = identifier_str.to_int();
			ERR_FAIL_COND_V(!node_map.has(node_id), false);
			String node_instance_property = name.get_slicec('/', 2);
			if (node_instance_property == "node")
			{
				r_ret = node_map.get(node_id).node;
				return true;
			}
			else if (node_instance_property == "position")
			{
				r_ret = node_map.get(node_id).position;
				return true;
			}
			else
			{
				ERR_FAIL_V(false);
			}
		}
	}
	return false;
}


bool FlowScript::includes_flow_script(const Ref<FlowScript> &p_other_flow_script) const
{
	if (!p_other_flow_script.is_valid())
	{
		return false;
	}
	for (FlowScriptIncludeID curr_include = 0; curr_include < INCLUDE_FLOW_SCRIPT_MAX; curr_include++)
	{
		if (script_includes[curr_include].is_valid())
		{
			if (script_includes[curr_include].flow_script == p_other_flow_script)
			{
				return true;
			}
			else if (script_includes[curr_include].flow_script->includes_flow_script(p_other_flow_script))
			{
				return true;
			}
		}
	}
	return false;
}


bool FlowScript::has_include_flow_script_instance(const FlowScriptIncludeID p_id) const
{
	return p_id > -1 && p_id < INCLUDE_FLOW_SCRIPT_MAX && script_includes[p_id].is_valid();
}


FlowScriptIncludeInstance FlowScript::get_include_flow_script_instance(const FlowScriptIncludeID p_id) const
{
	ERR_FAIL_COND_V(!has_include_flow_script_instance(p_id), FlowScriptIncludeInstance());
	return script_includes[p_id];
}


bool FlowScript::is_node_slot_available(const FlowScriptNodeID p_node_id) const
{
	return !node_map.has(p_node_id) && p_node_id >= NODE_ID_MIN && p_node_id <= NODE_ID_MAX;
}


bool FlowScript::has_node(const FlowScriptNodeID p_node_id) const
{
	return node_map.has(p_node_id);
}


Ref<FlowScriptNode> FlowScript::get_node_ref(const FlowScriptNodeID p_node_id) const
{
	ERR_FAIL_COND_V(!node_map.has(p_node_id), Ref<FlowScriptNode>());
	return node_map.get(p_node_id).node;
}


FlowScriptNode *FlowScript::get_node_ptr(const FlowScriptNodeID p_node_id) const
{
	ERR_FAIL_COND_V(!node_map.has(p_node_id), nullptr);
	return node_map.get(p_node_id).node.ptr();
}


void FlowScript::set_node_position(const FlowScriptNodeID p_node_id, const Point2i &p_position)
{
	if (node_map.get(p_node_id).position == p_position)
		return;
	node_map.get(p_node_id).position = p_position;
	emit_signal(SNAME("node_position_changed"), p_node_id);
}


Point2i FlowScript::get_node_position(const FlowScriptNodeID p_node_id) const
{
	ERR_FAIL_COND_V(!node_map.has(p_node_id), Point2i());
	return node_map.get(p_node_id).position;
}


void FlowScript::set_node_connection_list_length(const FlowScriptNodeID p_node_id, const uint8_t p_list, const int64_t p_length)
{
	ERR_FAIL_COND(!node_map.has(p_node_id));
	if (node_map.get(p_node_id).get_connection_list_length(p_list) == p_length)
		return;
	node_map.get(p_node_id).set_connection_list_length(p_list, p_length);
	emit_signal(SNAME("node_connections_changed"), p_node_id);
}


int64_t FlowScript::get_node_connection_list_length(const FlowScriptNodeID p_node_id, const uint8_t p_list) const
{
	ERR_FAIL_COND_V(!node_map.has(p_node_id), 0);
	return node_map.get(p_node_id).get_connection_list_length(p_list);
}


void FlowScript::set_node_connection(const FlowScriptNodeID p_node_id, const uint8_t p_list, const int64_t p_slot, const FlowScriptNodeID p_target_node_id)
{
	ERR_FAIL_COND(!node_map.has(p_node_id));
	if (node_map.get(p_node_id).get_connection_at(p_list, p_slot) == p_target_node_id)
		return;
	node_map.get(p_node_id).set_connection_at(p_list, p_slot, p_target_node_id);
	emit_signal(SNAME("node_connections_changed"), p_node_id);
}


FlowScriptNodeID FlowScript::get_node_connection(const FlowScriptNodeID p_node_id, const uint8_t p_list, const int64_t p_slot) const
{
	ERR_FAIL_COND_V(!node_map.has(p_node_id), NODE_ID_INVALID);
	return node_map.get(p_node_id).get_connection_at(p_list, p_slot);
}


bool FlowScript::copy_node_instance_to(const FlowScriptNodeID p_target_node_id, const FlowScriptNodeInstance &p_node_instance)
{
	ERR_FAIL_COND_V(node_map.has(p_target_node_id), false);
	node_map.insert(p_target_node_id, p_node_instance);
	cache_next_available_node_id_dirty = true;
	emit_signal(SNAME("node_added"), p_target_node_id);
	return true;
}


FlowScriptNodeInstance FlowScript::get_copy_of_node_instance(const FlowScriptNodeID p_node_id) const
{
	ERR_FAIL_COND_V(!node_map.has(p_node_id), FlowScriptNodeInstance());
	return node_map.get(p_node_id);
}


bool FlowScript::remove_node(const FlowScriptNodeID p_node_id)
{
	ERR_FAIL_COND_V(!node_map.has(p_node_id), false);
	emit_signal(SNAME("removing_node"), p_node_id);
	node_map.erase(p_node_id);
	cache_next_available_node_id_dirty = true;
	for (KeyValue<FlowScriptNodeID, FlowScriptNodeInstance> &E : node_map)
	{
		bool iter_node_conns_changed = false;
		for (KeyValue<uint8_t, Vector<FlowScriptNodeID>> &conn : E.value.connections)
		{
			for (int curr_conn_idx = conn.value.size() - 1; curr_conn_idx > -1; curr_conn_idx--)
			{
				if (conn.value[curr_conn_idx] == p_node_id)
				{
					// conn.value.remove_at(curr_conn_idx);
					conn.value.set(curr_conn_idx, NODE_ID_INVALID);
					iter_node_conns_changed = true;
				}
			}
		}
		if (iter_node_conns_changed)
		{
			emit_signal(SNAME("node_connections_changed"), E.key);
		}
	}
	emit_signal(SNAME("node_removed"), p_node_id);
	return true;
}


FlowScriptNodeID FlowScript::get_node_id_by_name(const String &p_node_name) const
{
	for (const KeyValue<FlowScriptNodeID, FlowScriptNodeInstance> &E : node_map)
	{
		if (!E.value.node.is_valid())
			continue;
		if (E.value.node->get_name() == p_node_name)
			return E.key;
	}
	return NODE_ID_INVALID;
}


FlowScriptNodeID FlowScript::add_node_to_first_available_slot(const Ref<FlowScriptNode> &p_node)
{
	update_cache_next_available_node_id();
	ERR_FAIL_COND_V(cache_next_available_node_id == NODE_ID_INVALID, NODE_ID_INVALID);
	FlowScriptNodeInstance instance;
	instance.set_node(p_node);
	node_map.insert(cache_next_available_node_id, instance);
	emit_signal(SNAME("node_added"), cache_next_available_node_id);
	return cache_next_available_node_id;
}


void FlowScript::set_include_flow_script(const FlowScriptIncludeID p_include_id, const Ref<FlowScript> &p_flow_script)
{
	ERR_FAIL_INDEX(p_include_id, INCLUDE_FLOW_SCRIPT_MAX);
	if (script_includes[p_include_id].flow_script == p_flow_script)
	{
		return;
	}
	ERR_FAIL_COND(includes_flow_script(p_flow_script));
	script_includes[p_include_id].flow_script = p_flow_script;
	emit_signal(SNAME("include_flow_script_changed"), p_include_id);
}


Ref<FlowScript> FlowScript::get_include_flow_script(const FlowScriptIncludeID p_include_id) const
{
	ERR_FAIL_INDEX_V(p_include_id, INCLUDE_FLOW_SCRIPT_MAX, Ref<FlowScript>());
	return script_includes[p_include_id].flow_script;
}


void FlowScript::set_include_flow_script_position(const FlowScriptIncludeID p_include_id, const Point2i &p_position)
{
	ERR_FAIL_INDEX(p_include_id, INCLUDE_FLOW_SCRIPT_MAX);
	if (script_includes[p_include_id].position == p_position)
	{
		return;
	}
	script_includes[p_include_id].position = p_position;
	emit_signal(SNAME("include_flow_script_position_changed"), p_include_id);
}


Point2i FlowScript::get_include_flow_script_position(const FlowScriptIncludeID p_include_id) const
{
	ERR_FAIL_INDEX_V(p_include_id, INCLUDE_FLOW_SCRIPT_MAX, Point2i());
	return script_includes[p_include_id].position;
}


void FlowScript::update_cache_next_available_node_id() const
{
	if (!cache_next_available_node_id_dirty)
		return;
	
	cache_next_available_node_id_dirty = false;
	
	for (FlowScriptNodeID curr_id = NODE_ID_MIN; curr_id <= NODE_ID_MAX; curr_id++)
	{
		if (!node_map.has(curr_id))
		{
			cache_next_available_node_id = curr_id;
			return;
		}
	}

	cache_next_available_node_id = NODE_ID_INVALID;
}


FlowScript::FlowScript()
{
	cache_node_id_array_ptr = &cache_node_id_array;
}
