#include "flow_script.hpp"
#include "flow_script_node.hpp"
#include "flow_script_node_instance.hpp"


void FlowScript::_bind_methods()
{
	BIND_CONSTANT(NODE_ID_INVALID);
	BIND_CONSTANT(NODE_ID_MIN);
	BIND_CONSTANT(NODE_ID_MAX);

	ClassDB::bind_method(D_METHOD("set_graph_connection_data", "data"), &FlowScript::set_graph_connection_data);
	ClassDB::bind_method(D_METHOD("get_graph_connection_data"), &FlowScript::get_graph_connection_data);
	ClassDB::bind_method(D_METHOD("get_node", "node_id"), &FlowScript::get_node_ref);
	ClassDB::bind_method(D_METHOD("set_node_position", "node_id", "point"), &FlowScript::set_node_position);
	ClassDB::bind_method(D_METHOD("get_node_position", "node_id"), &FlowScript::get_node_position);
	ClassDB::bind_method(D_METHOD("set_node_connection_list_length", "node_id", "list", "length"), &FlowScript::set_node_connection_list_length);
	ClassDB::bind_method(D_METHOD("get_node_connection_list_length", "node_id", "list"), &FlowScript::get_node_connection_list_length);
	ClassDB::bind_method(D_METHOD("set_node_connection", "node_id", "list", "slot", "target_node_id"), &FlowScript::set_node_connection);
	ClassDB::bind_method(D_METHOD("get_node_connection", "node_id", "list", "slot"), &FlowScript::get_node_connection);
	ClassDB::bind_method(D_METHOD("remove_node", "node_id"), &FlowScript::remove_node);
	ClassDB::bind_method(D_METHOD("has_node", "node_id"), &FlowScript::has_node);
	ClassDB::bind_method(D_METHOD("get_node_id_by_name", "node_name"), &FlowScript::get_node_id_by_name);
	ClassDB::bind_method(D_METHOD("get_node_id_list"), &FlowScript::bind_get_node_ids);

	ADD_SIGNAL(MethodInfo("node_position_changed", PropertyInfo(Variant::INT, "node_id")));
	ADD_SIGNAL(MethodInfo("node_connections_changed", PropertyInfo(Variant::INT, "node_id")));
	ADD_SIGNAL(MethodInfo("node_added", PropertyInfo(Variant::INT, "node_id")));
	ADD_SIGNAL(MethodInfo("removing_node", PropertyInfo(Variant::INT, "node_id")));
	ADD_SIGNAL(MethodInfo("node_removed", PropertyInfo(Variant::INT, "node_id")));
}


void FlowScript::_get_property_list(List<PropertyInfo> *p_list) const
{
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
				node_map.get(node_id).node = p_value;
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


void FlowScript::set_graph_connection_data(const PackedInt32Array &p_data)
{
	cache_connection_data = p_data;

	int current_traverse_idx = 0;

	while (current_traverse_idx < cache_connection_data.size())
	{
		FlowScriptNodeID current_node_id = cache_connection_data.get(current_traverse_idx);
		uint8_t current_connection_list_idx = cache_connection_data.get(current_traverse_idx + 1);
		int current_connection_list_size = cache_connection_data.get(current_traverse_idx + 2);

		current_traverse_idx += (3 + current_connection_list_size);

		if (!has_node(current_node_id))
		{
			ERR_PRINT("Node ID " + itos(current_node_id) + " not found in graph.");
			continue;
		}

		FlowScriptNodeInstance *current_node_instance = &(node_map.get(current_node_id));

		int conn_idx_begin = current_traverse_idx + 3;
		int conn_idx_end = current_traverse_idx + 3 + current_connection_list_size;

		for (int conn_idx = conn_idx_begin; conn_idx < conn_idx_end; conn_idx++)
		{
			FlowScriptNodeID target_node_id = cache_connection_data.get(conn_idx);
			current_node_instance->add_connection(current_connection_list_idx, target_node_id);
		}
	}
}


PackedInt32Array FlowScript::get_graph_connection_data() const
{
	update_cache_connection_data();
	return cache_connection_data;
}


bool FlowScript::is_node_slot_available(const FlowScriptNodeID p_node_id) const
{
	return !node_map.has(p_node_id) && p_node_id >= NODE_ID_MIN && p_node_id <= NODE_ID_MAX;
}


Vector<FlowScriptNodeID> *FlowScript::get_node_id_list() const
{
	update_cache_node_id_array();
	return cache_node_id_array_ptr;
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
	emit_signal(SNAME("node_removed"), p_node_id);
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
	instance.node = p_node;
	node_map.insert(cache_next_available_node_id, instance);
	emit_signal(SNAME("node_added"), cache_next_available_node_id);
	return cache_next_available_node_id;
}


void FlowScript::update_cache_connection_data() const
{
	if (!cache_connection_data_dirty)
		return;
	
	cache_connection_data_dirty = false;
	cache_connection_data.clear();

	for (const KeyValue<FlowScriptNodeID, FlowScriptNodeInstance> &E : node_map)
	{
		for (const KeyValue<uint8_t, Vector<FlowScriptNodeID>> &F : E.value.connections)
		{
			if (F.value.is_empty())
				continue;
			
			cache_connection_data.push_back(E.key);
			cache_connection_data.push_back(F.key);
			cache_connection_data.push_back(F.value.size());

			for (FlowScriptNodeID target_node_id : F.value)
				cache_connection_data.push_back(target_node_id);
		}
	}
}


void FlowScript::update_cache_node_id_array() const
{
	if (!cache_node_id_array_dirty)
		return;
	
	cache_node_id_array_dirty = false;
	
	cache_node_id_array.resize(node_map.size());
	int arr_idx = 0;
	for (const KeyValue<FlowScriptNodeID, FlowScriptNodeInstance> &E : node_map)
	{
		cache_node_id_array.set(arr_idx, E.key);
		arr_idx++;
	}
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


PackedInt32Array FlowScript::bind_get_node_ids() const
{
	update_cache_node_id_array();
	PackedInt32Array ret;
	ret.resize(cache_node_id_array.size());
	for (int i = 0; i < cache_node_id_array.size(); i++)
	{
		ret.set(i, cache_node_id_array.get(i));
	}
	return ret;
}


FlowScript::FlowScript()
{
	cache_node_id_array_ptr = &cache_node_id_array;
}
