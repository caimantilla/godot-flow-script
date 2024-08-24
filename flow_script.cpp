#include "flow_script.hpp"
#include "flow_script_node.hpp"
#include "flow_script_node_instance.hpp"
#include "flow_script_node_reference.hpp"


void FlowScript::_bind_methods()
{
	BIND_CONSTANT(NODE_ID_INVALID);
	BIND_CONSTANT(NODE_ID_MIN);
	BIND_CONSTANT(NODE_ID_MAX);

	BIND_CONSTANT(INCLUDE_FLOW_SCRIPT_ID_INVALID);
	BIND_CONSTANT(INCLUDE_FLOW_SCRIPT_MAX);

	ClassDB::bind_method(D_METHOD("set_node", "node_id", "data"), &FlowScript::set_node);
	ClassDB::bind_method(D_METHOD("get_node", "node_id"), &FlowScript::get_node_ref);
	ClassDB::bind_method(D_METHOD("set_node_position", "node_id", "position"), &FlowScript::set_node_position);
	ClassDB::bind_method(D_METHOD("get_node_position", "node_id"), &FlowScript::get_node_position);
	ClassDB::bind_method(D_METHOD("set_node_connection", "from_node_id", "list", "slot", "to_node_id", "to_include_id"), &FlowScript::bind_set_node_connection);
	ClassDB::bind_method(D_METHOD("get_node_connection", "from_node_id", "list", "slot"), &FlowScript::bind_get_node_connection);
	ClassDB::bind_method(D_METHOD("set_include_flow_script", "include_id", "flow_script"), &FlowScript::set_include_flow_script);
	ClassDB::bind_method(D_METHOD("get_include_flow_script", "include_id"), &FlowScript::get_include_flow_script);
	ClassDB::bind_method(D_METHOD("set_include_position", "include_id", "position"), &FlowScript::set_include_flow_script_position);
	ClassDB::bind_method(D_METHOD("get_include_position", "include_id"), &FlowScript::get_include_flow_script_position);
	ClassDB::bind_method(D_METHOD("remove_node_list", "node_id_list"), &FlowScript::bind_remove_node_list);
	ClassDB::bind_method(D_METHOD("remove_include_list", "include_id_list"), &FlowScript::bind_remove_include_list);

	ADD_SIGNAL(MethodInfo("include_added", PropertyInfo(Variant::INT, "include_id")));
	ADD_SIGNAL(MethodInfo("removing_include", PropertyInfo(Variant::INT, "include_id")));
	ADD_SIGNAL(MethodInfo("include_removed", PropertyInfo(Variant::INT, "include_id")));
	ADD_SIGNAL(MethodInfo("include_position_changed", PropertyInfo(Variant::INT, "include_id")));
	ADD_SIGNAL(MethodInfo("node_position_changed", PropertyInfo(Variant::INT, "node_id")));
	ADD_SIGNAL(MethodInfo("node_connection_changed", PropertyInfo(Variant::INT, "node_id"), PropertyInfo(Variant::INT, "list"), PropertyInfo(Variant::INT, "slot")));
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
			String prefix = "includes/" + itos(incl_id) + '/';
			p_list->push_back(PropertyInfo(Variant::OBJECT, prefix + "flow_script", PROPERTY_HINT_RESOURCE_TYPE, "FlowScript", PROPERTY_USAGE_NO_EDITOR, "FlowScript"));
			p_list->push_back(PropertyInfo(Variant::VECTOR2I, prefix + "position", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR));
		}
	}
	for (const KeyValue<FlowScriptNodeID, FlowScriptNodeInstance> &node_instance : node_map)
	{
		String prefix = "nodes/" + itos(node_instance.key) + '/';
		p_list->push_back(PropertyInfo(Variant::OBJECT, prefix + "node", PROPERTY_HINT_RESOURCE_TYPE, "FlowScriptNode", PROPERTY_USAGE_NO_EDITOR, "FlowScriptNode"));
		p_list->push_back(PropertyInfo(Variant::VECTOR2I, prefix + "position", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR));
		
		for (int64_t conn_list_idx = 0; conn_list_idx < node_instance.value.connection_lists.size(); conn_list_idx++)
		{
			for (int64_t conn_slot_idx = 0; conn_slot_idx < node_instance.value.connection_lists.get(conn_list_idx).size(); conn_slot_idx++)
			{
				FlowScriptNodeReference target_node = node_instance.value.connection_lists.get(conn_list_idx).get(conn_slot_idx);
				if (target_node.node_id == NODE_ID_INVALID)
				{
					continue;
				}
				String conn_prefix = prefix + "connections/" + itos(conn_list_idx) + '/' + itos(conn_slot_idx) + '/';
				if (target_node.flow_script_id != INCLUDE_FLOW_SCRIPT_ID_INVALID)
				{
					p_list->push_back(PropertyInfo(Variant::INT, prefix + "flow_script_id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR));
				}
				p_list->push_back(PropertyInfo(Variant::INT, conn_prefix + "node_id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR));
			}
		}
	}
}


bool FlowScript::_set(const StringName &p_name, const Variant &p_value)
{
	String name = p_name;
	if (name.begins_with("includes/"))
	{
		String incl_idx_str = name.get_slicec('/', 1);
		ERR_FAIL_COND_V(!incl_idx_str.is_valid_int(), false);
		FlowScriptIncludeID incl_id = incl_idx_str.to_int();
		ERR_FAIL_INDEX_V(incl_id, INCLUDE_FLOW_SCRIPT_MAX, false);
		String incl_instance_property = name.get_slicec('/', 2);
		if (incl_instance_property == "flow_script")
		{
			ERR_FAIL_COND_V(p_value.get_type() != Variant::OBJECT, false);
			script_includes[incl_id].flow_script = p_value;
			return true;
		}
		else if (incl_instance_property == "position")
		{
			script_includes[incl_id].position = p_value;
			return true;
		}
	}
	else if (name.begins_with("nodes/"))
	{
		String identifier_str = name.get_slicec('/', 1);
		ERR_FAIL_COND_V(!identifier_str.is_valid_int(), false);
		FlowScriptNodeID node_id = identifier_str.to_int();
		if (is_node_slot_available(node_id))
		{
			node_map.insert(node_id, FlowScriptNodeInstance());
		}
		ERR_FAIL_COND_V(!node_map.has(node_id), false);
		String node_instance_property = name.get_slicec('/', 2);
		if (node_instance_property == "node")
		{
			node_map[node_id].set_node(p_value);
			init_node(node_id);
			return true;
		}
		else if (node_instance_property == "position")
		{
			node_map.get(node_id).position = p_value;
			return true;
		}
		else if (node_instance_property == "connections")
		{
			String conn_list_idx_str = name.get_slicec('/', 3);
			String conn_slot_idx_str = name.get_slicec('/', 4);
			ERR_FAIL_COND_V(!conn_list_idx_str.is_valid_int(), false);
			ERR_FAIL_COND_V(!conn_slot_idx_str.is_valid_int(), false);
			FlowScriptNodeOutputConnection connection_dir = FlowScriptNodeOutputConnection(conn_list_idx_str.to_int(), conn_slot_idx_str.to_int());
			uint8_t conn_list_idx = conn_list_idx_str.to_int();
			int64_t conn_slot_idx= conn_slot_idx_str.to_int();

			String conn_property = name.get_slicec('/', 5);
			if (conn_property == "flow_script_id")
			{
				node_map.get(node_id).set_connection_flow_script_id(connection_dir, p_value);
				return true;
			}
			else if (conn_property == "node_id")
			{
				node_map.get(node_id).set_connection_node_id(connection_dir, p_value);
				return true;
			}
		}
	}
	return false;
}


bool FlowScript::_get(const StringName &p_name, Variant &r_ret) const
{
	String name = p_name;
	if (name.begins_with("includes/"))
	{
		String incl_idx_str = name.get_slicec('/', 1);
		ERR_FAIL_COND_V(!incl_idx_str.is_valid_int(), false);
		FlowScriptIncludeID incl_id = incl_idx_str.to_int();
		ERR_FAIL_INDEX_V(incl_id, INCLUDE_FLOW_SCRIPT_MAX, false);
		String incl_instance_property = name.get_slicec('/', 2);
		if (incl_instance_property == "flow_script")
		{
			r_ret = script_includes[incl_id].flow_script;
			return true;
		}
		else if (incl_instance_property == "position")
		{
			r_ret = script_includes[incl_id].position;
			return true;
		}
	}
	else if (name.begins_with("nodes/"))
	{
		String node_id_str = name.get_slicec('/', 1);
		ERR_FAIL_COND_V(!node_id_str.is_valid_int(), false);
		FlowScriptNodeID node_id = node_id_str.to_int();
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
		else if (node_instance_property == "connections")
		{
			String conn_list_idx_str = name.get_slicec('/', 3);
			String conn_slot_idx_str = name.get_slicec('/', 4);
			ERR_FAIL_COND_V(!conn_list_idx_str.is_valid_int(), false);
			ERR_FAIL_COND_V(!conn_slot_idx_str.is_valid_int(), false);
			FlowScriptNodeOutputConnection connection_dir = FlowScriptNodeOutputConnection(conn_list_idx_str.to_int(), conn_slot_idx_str.to_int());

			String conn_property = name.get_slicec('/', 5);
			if (conn_property == "flow_script_id")
			{
				r_ret = node_map.get(node_id).get_connection(connection_dir).flow_script_id;
				return true;
			}
			else if (conn_property == "node_id")
			{
				r_ret = node_map.get(node_id).get_connection(connection_dir).node_id;
				return true;
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


void FlowScript::set_node(const FlowScriptNodeID p_node_id, const Ref<FlowScriptNode> &p_data)
{
	ERR_FAIL_COND(p_node_id < NODE_ID_MIN);
	if (p_data.is_valid())
	{
		if (node_map.has(p_node_id))
		{
			if (node_map[p_node_id].node == p_data)
			{
				return;
			}
		}
		else
		{
			node_map.insert(p_node_id, FlowScriptNodeInstance());
		}
		node_map[p_node_id].set_node(p_data);
		init_node(p_node_id);
		emit_signal(SNAME("node_added"), p_node_id);
	}
	else
	{
		if (node_map.has(p_node_id))
		{
			remove_node(p_node_id);
		}
	}
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


uint8_t FlowScript::get_node_connection_list_count(const FlowScriptNodeID p_node_id) const
{
	ERR_FAIL_COND_V(!node_map.has(p_node_id), 0);
	return node_map.get(p_node_id).get_connection_list_count();
}


int64_t FlowScript::get_node_connection_list_length(const FlowScriptNodeID p_node_id, const uint8_t p_list) const
{
	ERR_FAIL_COND_V(!node_map.has(p_node_id), 0);
	return node_map.get(p_node_id).get_connection_list_length(p_list);
}


void FlowScript::set_node_connection(const FlowScriptNodeID p_node_id, const FlowScriptNodeOutputConnection &p_connection, const FlowScriptNodeReference p_target_node)
{
	ERR_FAIL_COND(!node_map.has(p_node_id));
	if (node_map.get(p_node_id).get_connection(p_connection) == p_target_node)
		return;
	node_map.get(p_node_id).set_connection(p_connection, p_target_node);
	emit_signal(SNAME("node_connection_changed"), p_node_id, p_connection.list, p_connection.slot);
}


FlowScriptNodeReference FlowScript::get_node_connection(const FlowScriptNodeID p_node_id, const FlowScriptNodeOutputConnection &p_connection) const
{
	ERR_FAIL_COND_V(!node_map.has(p_node_id), NODE_ID_INVALID);
	return node_map.get(p_node_id).get_connection(p_connection);
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


bool FlowScript::remove_node_list(const List<FlowScriptNodeID> &p_node_id_list)
{
	ERR_FAIL_COND_V(p_node_id_list.is_empty(), false);
	HashSet<FlowScriptNodeID> remove_id_set;
	for (const FlowScriptNodeID &curr_node_id : p_node_id_list)
	{
		ERR_FAIL_COND_V(!node_map.has(curr_node_id), false);
		remove_id_set.insert(curr_node_id);
	}
	for (const FlowScriptNodeID &curr_node_id : p_node_id_list)
	{
		emit_signal(SNAME("removing_node"), curr_node_id);
		uninit_node(curr_node_id);
		node_map.erase(curr_node_id);
		cache_next_available_node_id_dirty = true;
		for (KeyValue<FlowScriptNodeID, FlowScriptNodeInstance> &curr_instance_kv : node_map)
		{
			if (remove_id_set.has(curr_instance_kv.key))
			{
				continue;
			}
			for (int64_t list_idx = 0; list_idx < curr_instance_kv.value.connection_lists.size(); list_idx++)
			{
				for (int64_t slot_idx = 0; slot_idx < curr_instance_kv.value.connection_lists.get(list_idx).size(); slot_idx++)
				{
					if (curr_instance_kv.value.connection_lists.get(list_idx).get(slot_idx) == curr_node_id)
					{
						curr_instance_kv.value.connection_lists.get(list_idx).set(slot_idx, NODE_ID_INVALID);
						emit_signal(SNAME("node_connection_changed"), curr_instance_kv.key, list_idx, slot_idx);
					}
				}
			}
			emit_signal(SNAME("node_removed"), curr_node_id);
		}
	}
	return true;
}


bool FlowScript::remove_node(const FlowScriptNodeID p_node_id)
{
	List<FlowScriptNodeID> list;
	list.push_back(p_node_id);
	return remove_node_list(list);
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


FlowScriptNodeID FlowScript::get_first_available_node_slot() const
{
	update_cache_next_available_node_id();
	return cache_next_available_node_id;
}


FlowScriptNodeID FlowScript::add_node_to_first_available_slot(const Ref<FlowScriptNode> &p_node)
{
	update_cache_next_available_node_id();
	ERR_FAIL_COND_V(cache_next_available_node_id == NODE_ID_INVALID, NODE_ID_INVALID);
	FlowScriptNodeInstance instance;
	node_map.insert(cache_next_available_node_id, instance);
	instance.set_node(p_node);
	init_node(cache_next_available_node_id);
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
	emit_signal(SNAME("include_added"), p_include_id);
}


Ref<FlowScript> FlowScript::get_include_flow_script(const FlowScriptIncludeID p_include_id) const
{
	ERR_FAIL_INDEX_V(p_include_id, INCLUDE_FLOW_SCRIPT_MAX, Ref<FlowScript>());
	return script_includes[p_include_id].flow_script;
}


FlowScriptIncludeID FlowScript::add_include_flow_script(const Ref<FlowScript> &p_other_flow_script)
{
	ERR_FAIL_COND_V(!p_other_flow_script.is_valid(), INCLUDE_FLOW_SCRIPT_ID_INVALID);
	ERR_FAIL_COND_V(includes_flow_script(p_other_flow_script), INCLUDE_FLOW_SCRIPT_ID_INVALID);
	ERR_FAIL_COND_V(p_other_flow_script->includes_flow_script(this), INCLUDE_FLOW_SCRIPT_ID_INVALID); // block circular reference
	for (FlowScriptIncludeID i = 0; i < INCLUDE_FLOW_SCRIPT_MAX; i++)
	{
		if (!script_includes[i].is_valid())
		{
			script_includes[i].flow_script = p_other_flow_script;
			emit_signal(SNAME("include_added"), i);
			return i;
		}
	}
	return INCLUDE_FLOW_SCRIPT_ID_INVALID;
}


bool FlowScript::remove_include_flow_script_list(const List<FlowScriptIncludeID> &p_include_id_list)
{
	HashSet<FlowScriptIncludeID> id_removal_set;
	for (const FlowScriptIncludeID id : p_include_id_list)
	{
		ERR_CONTINUE(!has_include_flow_script_instance(id));
		id_removal_set.insert(id);
	}
	ERR_FAIL_COND_V(id_removal_set.is_empty(), false);

	for (KeyValue<FlowScriptNodeID, FlowScriptNodeInstance> &kv : node_map)
	{
		FlowScriptNodeInstance &instance = kv.value;
		for (uint8_t list_idx = 0; list_idx < instance.connection_lists.size(); list_idx++)
		{
			for (int64_t slot_idx = 0; slot_idx < instance.connection_lists[list_idx].size(); slot_idx++)
			{
				if (id_removal_set.has(instance.connection_lists[list_idx][slot_idx].flow_script_id))
				{
					instance.connection_lists.write[list_idx].write[slot_idx] = FlowScriptNodeReference();
					emit_signal(SNAME("node_connection_changed"), kv.key, list_idx, slot_idx);
				}
			}
		}
	}
	for (const FlowScriptIncludeID &id : id_removal_set)
	{
		emit_signal(SNAME("removing_include"), id);
		script_includes[id].flow_script = Ref<FlowScript>();
		emit_signal(SNAME("include_removed"), id);
	}
	return true;
}


bool FlowScript::remove_include_flow_script(const FlowScriptIncludeID p_include_id)
{
	List<FlowScriptIncludeID> list;
	list.push_back(p_include_id);
	return remove_include_flow_script_list(list);
}


void FlowScript::set_include_flow_script_position(const FlowScriptIncludeID p_include_id, const Point2i &p_position)
{
	ERR_FAIL_INDEX(p_include_id, INCLUDE_FLOW_SCRIPT_MAX);
	if (script_includes[p_include_id].position == p_position)
	{
		return;
	}
	script_includes[p_include_id].position = p_position;
	emit_signal(SNAME("include_position_changed"), p_include_id);
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


void FlowScript::update_connection_outputs_for_node(FlowScriptNodeID p_node_id)
{
	ERR_FAIL_COND(!node_map.has(p_node_id));
	List<int64_t> length_list;
	node_map.get(p_node_id).node->get_output_connection_list_lengths(length_list);
	node_map.get(p_node_id).set_connection_list_count(length_list.size());
	int curr_list_idx = 0;
	for (const int64_t &curr_desired_length : length_list)
	{
		if (node_map.get(p_node_id).get_connection_list_length(curr_list_idx) != curr_desired_length)
		{
			node_map.get(p_node_id).set_connection_list_length(curr_list_idx, curr_desired_length);
		}
		curr_list_idx++;
	}
}


void FlowScript::init_node(FlowScriptNodeID p_node_id)
{
	ERR_FAIL_COND(!node_map.has(p_node_id));
	ERR_FAIL_COND(!node_map.get(p_node_id).is_valid());
	node_map.get(p_node_id).node->connect_changed(callable_mp(this, &FlowScript::on_node_changed).bind(p_node_id));
	update_connection_outputs_for_node(p_node_id);
}


void FlowScript::uninit_node(FlowScriptNodeID p_node_id)
{
	ERR_FAIL_COND(!node_map.has(p_node_id));
	ERR_FAIL_COND(!node_map.get(p_node_id).is_valid());
	node_map.get(p_node_id).node->disconnect_changed(callable_mp(this, &FlowScript::on_node_changed));
}


void FlowScript::on_node_changed(FlowScriptNodeID p_node_id)
{
	update_connection_outputs_for_node(p_node_id);
}


void FlowScript::bind_set_node_connection(const FlowScriptNodeID p_from_node_id, const uint8_t p_list, const int64_t p_slot, const FlowScriptNodeID p_to_node_id, const FlowScriptIncludeID p_to_include_id)
{
	set_node_connection(p_from_node_id, FlowScriptNodeOutputConnection(p_list, p_slot), FlowScriptNodeReference(p_to_node_id, p_to_include_id));
}


Dictionary FlowScript::bind_get_node_connection(const FlowScriptNodeID p_from_node_id, const uint8_t p_list, const int64_t p_slot) const
{
	FlowScriptNodeReference ret_struct = get_node_connection(p_from_node_id, FlowScriptNodeOutputConnection(p_list, p_slot));
	return ret_struct.to_dictionary();
}


void FlowScript::bind_remove_node_list(const PackedInt32Array &p_id_list)
{
	List<FlowScriptNodeID> list;
	for (const int32_t &id : p_id_list)
	{
		list.push_back(FlowScriptNodeID(id));
	}
	remove_node_list(list);
}


void FlowScript::bind_remove_include_list(const PackedInt32Array &p_id_list)
{
	List<FlowScriptIncludeID> list;
	for (const int32_t &id : p_id_list)
	{
		list.push_back(FlowScriptIncludeID(id));
	}
	remove_include_list(list);
}


FlowScript::FlowScript()
{
	cache_node_id_array_ptr = &cache_node_id_array;
}
