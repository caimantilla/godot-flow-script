#include "flow_script.hpp"
#include "flow_script_node.hpp"
#include "flow_script_node_instance.hpp"
#include "flow_script_node_reference.hpp"
#include "core/config/engine.h"


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
	ClassDB::bind_method(D_METHOD("remove_node", "node_id"), &FlowScript::remove_node);
	ClassDB::bind_method(D_METHOD("remove_node_list", "node_id_list"), &FlowScript::bind_remove_node_list);
	ClassDB::bind_method(D_METHOD("remove_include", "include_id"), &FlowScript::remove_include_flow_script);
	ClassDB::bind_method(D_METHOD("remove_include_list", "include_id_list"), &FlowScript::bind_remove_include_list);
	ClassDB::bind_method(D_METHOD("get_node_id_by_name", "node_name"), &FlowScript::get_node_id_by_name);
	ClassDB::bind_method(D_METHOD("get_every_node_resource_recursive"), &FlowScript::bind_get_every_node_resource_recursive);
	ClassDB::bind_method(D_METHOD("get_every_node_resource_connected_to_node", "origin_node_id", "include_origin"), &FlowScript::bind_get_every_node_resource_connected_to_node);

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
		bool script_assigned = script_includes[incl_id].is_valid();
		const uint64_t usage_script = script_assigned ? PROPERTY_USAGE_DEFAULT : PROPERTY_USAGE_EDITOR;
		const uint64_t usage_pos = script_assigned ? PROPERTY_USAGE_DEFAULT : PROPERTY_USAGE_NONE;
		String prefix = "includes/" + itos(incl_id) + '/';
		p_list->push_back(PropertyInfo(Variant::OBJECT, prefix + "flow_script", PROPERTY_HINT_RESOURCE_TYPE, "FlowScript", usage_script, "FlowScript"));
		p_list->push_back(PropertyInfo(Variant::VECTOR2I, prefix + "position", PROPERTY_HINT_NONE, "", usage_pos));
	}
	// Looping through every possible node ID like this could be kinda slow, but I want the indices to be ordered and I feel like it'd still be faster than getting the list of IDs and sorting it
	for (FlowScriptNodeID current_node_id = NODE_ID_MIN; current_node_id <= NODE_ID_MAX; current_node_id++)
	{
		if (!node_map.has(current_node_id))
		{
			continue;
		}
		const FlowScriptNodeInstance &current_node_instance = node_map[current_node_id];
		String prefix = "nodes/" + itos(current_node_id) + '/';
		p_list->push_back(PropertyInfo(Variant::OBJECT, prefix + "node", PROPERTY_HINT_RESOURCE_TYPE, "FlowScriptNode", PROPERTY_USAGE_NO_EDITOR, "FlowScriptNode"));
		p_list->push_back(PropertyInfo(Variant::VECTOR2I, prefix + "position", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR));
		
		for (int64_t conn_list_idx = 0; conn_list_idx < current_node_instance.connection_lists.size(); conn_list_idx++)
		{
			for (int64_t conn_slot_idx = 0; conn_slot_idx < current_node_instance.connection_lists[conn_list_idx].size(); conn_slot_idx++)
			{
				FlowScriptNodeReference target_node = current_node_instance.connection_lists[conn_list_idx][conn_slot_idx];
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
			set_include_flow_script(incl_id, p_value);
			return true;
		}
		else
		{
			ERR_FAIL_COND_V(!script_includes[incl_id].is_valid(), false);
			if (incl_instance_property == "position")
			{
				set_include_flow_script_position(incl_id, p_value);
				return true;
			}
		}
	}
	else if (name.begins_with("nodes/"))
	{
		String identifier_str = name.get_slicec('/', 1);
		ERR_FAIL_COND_V(!identifier_str.is_valid_int(), false);

		FlowScriptNodeID node_id = identifier_str.to_int();
		String node_instance_property = name.get_slicec('/', 2);

		if (node_instance_property == "node")
		{
			set_node(node_id, p_value);
			return true;
		}
		else
		{
			ERR_FAIL_COND_V(!has_node(node_id), false);
			if (node_instance_property == "position")
			{
				set_node_position(node_id, p_value);
				return true;
			}
			else if (node_instance_property == "connections")
			{
				String conn_list_idx_str = name.get_slicec('/', 3);
				String conn_slot_idx_str = name.get_slicec('/', 4);
				ERR_FAIL_COND_V(!conn_list_idx_str.is_valid_int(), false);
				ERR_FAIL_COND_V(!conn_slot_idx_str.is_valid_int(), false);
				uint8_t conn_list_idx = conn_list_idx_str.to_int();
				int64_t conn_slot_idx= conn_slot_idx_str.to_int();
				FlowScriptNodeOutputConnection connection_dir = FlowScriptNodeOutputConnection(conn_list_idx, conn_slot_idx);

				String conn_property = name.get_slicec('/', 5);
				if (conn_property == "flow_script_id")
				{
					FlowScriptNodeReference ref = get_node_connection(node_id, connection_dir);
					ref.flow_script_id = p_value;
					set_node_connection(node_id, connection_dir, ref);
					return true;
				}
				else if (conn_property == "node_id")
				{
					FlowScriptNodeReference ref = get_node_connection(node_id, connection_dir);
					ref.node_id = p_value;
					set_node_connection(node_id, connection_dir, ref);
					return true;
				}
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
		ERR_FAIL_COND_V(!has_node(node_id), false);

		const FlowScriptNodeInstance &node_instance = node_map[node_id];
		String node_instance_property = name.get_slicec('/', 2);

		if (node_instance_property == "node")
		{
			r_ret = node_instance.node;
			return true;
		}
		else if (node_instance_property == "position")
		{
			r_ret = node_instance.position;
			return true;
		}
		else if (node_instance_property == "connections")
		{
			String conn_list_idx_str = name.get_slicec('/', 3);
			String conn_slot_idx_str = name.get_slicec('/', 4);
			ERR_FAIL_COND_V(!conn_list_idx_str.is_valid_int(), false);
			ERR_FAIL_COND_V(!conn_slot_idx_str.is_valid_int(), false);
			uint8_t conn_list_idx = conn_list_idx_str.to_int();
			int64_t conn_slot_idx = conn_slot_idx_str.to_int();
			ERR_FAIL_INDEX_V(conn_list_idx, node_instance.connection_lists.size(), false);
			ERR_FAIL_INDEX_V(conn_slot_idx, node_instance.connection_lists[conn_list_idx].size(), false);

			String conn_property = name.get_slicec('/', 5);
			if (conn_property == "flow_script_id")
			{
				r_ret = node_instance.connection_lists[conn_list_idx][conn_slot_idx].flow_script_id;
				return true;
			}
			else if (conn_property == "node_id")
			{
				r_ret = node_instance.connection_lists[conn_list_idx][conn_slot_idx].node_id;
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
	ERR_FAIL_COND(p_node_id >= NODE_ID_MAX);
	if (node_map.has(p_node_id))
	{
		if (node_map[p_node_id].node == p_data)
		{
			return;
		}
		emit_signal(SNAME("removing_node"), p_node_id);
		for (KeyValue<FlowScriptNodeID, FlowScriptNodeInstance> &kv : node_map)
		{
			FlowScriptNodeInstance &node_instance = kv.value;
			for (uint8_t list_idx = 0; list_idx < node_instance.connection_lists.size(); list_idx++)
			{
				for (int64_t slot_idx = 0; slot_idx < node_instance.connection_lists[list_idx].size(); slot_idx++)
				{
					if (node_instance.connection_lists[list_idx][slot_idx] == FlowScriptNodeReference(INCLUDE_FLOW_SCRIPT_ID_INVALID, p_node_id))
					{
						set_node_connection(kv.key, FlowScriptNodeOutputConnection(list_idx, slot_idx), FlowScriptNodeReference());
					}
				}
			}
		}
		node_map[p_node_id].node->disconnect_changed(callable_mp(this, &FlowScript::on_node_changed));
		node_map.erase(p_node_id);
		emit_signal(SNAME("node_removed"), p_node_id);
	}
	if (p_data.is_valid())
	{
		node_map[p_node_id].node = p_data;
		update_connection_outputs_for_node(p_node_id);
		p_data->connect_changed(callable_mp(this, &FlowScript::on_node_changed).bind(p_node_id), CONNECT_REFERENCE_COUNTED);
		emit_signal(SNAME("node_added"), p_node_id);
	}
	emit_changed();
}


Ref<FlowScriptNode> FlowScript::get_node_ref(const FlowScriptNodeID p_node_id) const
{
	ERR_FAIL_COND_V(!node_map.has(p_node_id), Ref<FlowScriptNode>());
	return node_map[p_node_id].node;
}


FlowScriptNode *FlowScript::get_node_ptr(const FlowScriptNodeID p_node_id) const
{
	ERR_FAIL_COND_V(!node_map.has(p_node_id), nullptr);
	return node_map[p_node_id].node.ptr();
}


void FlowScript::set_node_position(const FlowScriptNodeID p_node_id, const Point2i &p_position)
{
	ERR_FAIL_COND(!node_map.has(p_node_id));
	if (node_map[p_node_id].position == p_position)
	{
		return;
	}
	node_map[p_node_id].position = p_position;
	emit_signal(SNAME("node_position_changed"), p_node_id);
	emit_changed();
}


Point2i FlowScript::get_node_position(const FlowScriptNodeID p_node_id) const
{
	ERR_FAIL_COND_V(!node_map.has(p_node_id), Point2i());
	return node_map[p_node_id].position;
}


uint8_t FlowScript::get_node_connection_list_count(const FlowScriptNodeID p_node_id) const
{
	ERR_FAIL_COND_V(!node_map.has(p_node_id), 0);
	return node_map[p_node_id].connection_lists.size();
}


int64_t FlowScript::get_node_connection_list_length(const FlowScriptNodeID p_node_id, const uint8_t p_list) const
{
	ERR_FAIL_COND_V(!node_map.has(p_node_id), 0);
	ERR_FAIL_INDEX_V(p_list, node_map[p_node_id].connection_lists.size(), 0);
	return node_map[p_node_id].connection_lists[p_list].size();
}


void FlowScript::set_node_connection(const FlowScriptNodeID p_node_id, const FlowScriptNodeOutputConnection &p_connection, const FlowScriptNodeReference p_target_node)
{
	ERR_FAIL_COND(!node_map.has(p_node_id));

	FlowScriptNodeInstance &node_instance = node_map[p_node_id];

	ERR_FAIL_INDEX(p_connection.list, node_instance.connection_lists.size());
	ERR_FAIL_INDEX(p_connection.slot, node_instance.connection_lists[p_connection.list].size());

	if (node_instance.connection_lists[p_connection.list][p_connection.slot] == p_target_node)
	{
		return;
	}
	node_instance.connection_lists.write[p_connection.list].write[p_connection.slot] = p_target_node;
	emit_signal(SNAME("node_connection_changed"), p_node_id, p_connection.list, p_connection.slot);
	emit_changed();
}


FlowScriptNodeReference FlowScript::get_node_connection(const FlowScriptNodeID p_node_id, const FlowScriptNodeOutputConnection &p_connection) const
{
	ERR_FAIL_COND_V(!node_map.has(p_node_id), NODE_ID_INVALID);
	const FlowScriptNodeInstance &node_instance = node_map[p_node_id];
	ERR_FAIL_INDEX_V(p_connection.list, node_instance.connection_lists.size(), FlowScriptNodeReference());
	ERR_FAIL_INDEX_V(p_connection.slot, node_instance.connection_lists[p_connection.list].size(), FlowScriptNodeReference());
	return node_instance.connection_lists[p_connection.list][p_connection.slot];
}


bool FlowScript::copy_node_instance_to(const FlowScriptNodeID p_target_node_id, const FlowScriptNodeInstance &p_node_instance)
{
	ERR_FAIL_COND_V(node_map.has(p_target_node_id), false);
	set_node(p_target_node_id, p_node_instance.node);
	node_map[p_target_node_id] = p_node_instance;
	return true;
}


FlowScriptNodeInstance FlowScript::get_copy_of_node_instance(const FlowScriptNodeID p_node_id) const
{
	ERR_FAIL_COND_V(!node_map.has(p_node_id), FlowScriptNodeInstance());
	return node_map[p_node_id];
}


bool FlowScript::remove_node_list(const List<FlowScriptNodeID> &p_node_id_list)
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
	ERR_FAIL_COND_V(!node_map.has(p_node_id), false);
	set_node(p_node_id, Ref<FlowScriptNode>());
	return !node_map.has(p_node_id);
}


FlowScriptNodeID FlowScript::get_node_id_by_name(const String &p_node_name) const
{
	for (const KeyValue<FlowScriptNodeID, FlowScriptNodeInstance> &E : node_map)
	{
		if (E.value.node.is_valid() && E.value.node->get_name() == p_node_name)
			return E.key;
	}
	return NODE_ID_INVALID;
}


FlowScriptNodeID FlowScript::get_first_available_node_slot() const
{
	for (FlowScriptNodeID i = NODE_ID_MIN; i <= NODE_ID_MAX; i++)
	{
		if (!node_map.has(i))
		{
			return i;
		}
	}
	return NODE_ID_INVALID;
}


FlowScriptNodeID FlowScript::add_node_to_first_available_slot(const Ref<FlowScriptNode> &p_node)
{
	FlowScriptNodeID slot = get_first_available_node_slot();
	ERR_FAIL_COND_V(slot == NODE_ID_INVALID, NODE_ID_INVALID);

	set_node(slot, p_node);
	ERR_FAIL_COND_V(!node_map.has(slot), NODE_ID_INVALID);
	return slot;
}


void FlowScript::set_include_flow_script(const FlowScriptIncludeID p_include_id, const Ref<FlowScript> &p_flow_script)
{
	ERR_FAIL_INDEX(p_include_id, INCLUDE_FLOW_SCRIPT_MAX);
	if (script_includes[p_include_id].flow_script == p_flow_script)
	{
		return;
	}
	if (p_flow_script.is_valid())
	{
		ERR_FAIL_COND_EDMSG(includes_flow_script(p_flow_script), RTR("Cannot inculde another FlowScript multiple times."));
		ERR_FAIL_COND_EDMSG(p_flow_script->includes_flow_script(this), RTR("Cannot include another FlowScript that includes this FlowScript; circular inclusions are not supported."));
	}
	else
	{
	}
	// clear all references to the current include
	if (script_includes[p_include_id].is_valid())
	{
		emit_signal(SNAME("removing_include"), p_include_id);
		for (KeyValue<FlowScriptNodeID, FlowScriptNodeInstance> &kv : node_map)
		{
			FlowScriptNodeInstance &node_instance = kv.value;
			for (uint8_t list_idx = 0; list_idx < node_instance.connection_lists.size(); list_idx++)
			{
				for (int64_t slot_idx = 0; slot_idx < node_instance.connection_lists[list_idx].size(); slot_idx++)
				{
					if (node_instance.connection_lists[list_idx][slot_idx].flow_script_id == p_include_id)
					{
						set_node_connection(kv.key, FlowScriptNodeOutputConnection(list_idx, slot_idx), FlowScriptNodeReference());
					}
				}
			}
		}
		script_includes[p_include_id].flow_script->disconnect_changed(callable_mp(this, &FlowScript::on_include_changed));
		script_includes[p_include_id] = FlowScriptIncludeInstance();
		emit_signal(SNAME("include_removed"), p_include_id);
	}
	if (p_flow_script.is_valid())
	{
		script_includes[p_include_id].flow_script = p_flow_script;
		p_flow_script->connect_changed(callable_mp(this, &FlowScript::on_include_changed).bind(p_include_id), CONNECT_REFERENCE_COUNTED);
		emit_signal(SNAME("include_added"), p_include_id);
	}
	emit_changed();
	notify_property_list_changed(); // i just don't feel like making a gui for flowscript inclusion man
}


Ref<FlowScript> FlowScript::get_include_flow_script(const FlowScriptIncludeID p_include_id) const
{
	ERR_FAIL_INDEX_V(p_include_id, INCLUDE_FLOW_SCRIPT_MAX, Ref<FlowScript>());
	return script_includes[p_include_id].flow_script;
}


FlowScriptIncludeID FlowScript::add_include_flow_script(const Ref<FlowScript> &p_other_flow_script)
{
	ERR_FAIL_COND_V(!p_other_flow_script.is_valid(), INCLUDE_FLOW_SCRIPT_ID_INVALID);
	for (FlowScriptIncludeID i = 0; i < INCLUDE_FLOW_SCRIPT_MAX; i++)
	{
		if (!script_includes[i].is_valid())
		{
			set_include_flow_script(i, p_other_flow_script);
			ERR_BREAK(!script_includes[i].is_valid());
			return i;
		}
	}
	return INCLUDE_FLOW_SCRIPT_ID_INVALID;
}


bool FlowScript::remove_include_flow_script_list(const List<FlowScriptIncludeID> &p_include_id_list)
{
	ERR_FAIL_COND_V(p_include_id_list.is_empty(), false);
	for (const FlowScriptIncludeID id : p_include_id_list)
	{
		ERR_FAIL_COND_V(!remove_include_flow_script(id), false);
	}
	return true;
}


bool FlowScript::remove_include_flow_script(const FlowScriptIncludeID p_include_id)
{
	ERR_FAIL_INDEX_V(p_include_id, INCLUDE_FLOW_SCRIPT_MAX, false);
	ERR_FAIL_COND_V(!script_includes[p_include_id].is_valid(), false);
	set_include_flow_script(p_include_id, Ref<FlowScript>());
	ERR_FAIL_COND_V(script_includes[p_include_id].is_valid(), false);
	return true;
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
	emit_changed();
}


Point2i FlowScript::get_include_flow_script_position(const FlowScriptIncludeID p_include_id) const
{
	ERR_FAIL_INDEX_V(p_include_id, INCLUDE_FLOW_SCRIPT_MAX, Point2i());
	return script_includes[p_include_id].position;
}


void FlowScript::update_connection_outputs_for_node(FlowScriptNodeID p_node_id)
{
	ERR_FAIL_COND(!node_map.has(p_node_id));
	bool changed = false;

	FlowScriptNodeInstance &node_instance = node_map[p_node_id];
	List<int64_t> length_list;
	node_instance.node->get_output_connection_list_lengths(length_list);

	uint8_t old_list_count = node_instance.connection_lists.size();
	uint8_t new_list_count = length_list.size();

	if (old_list_count != new_list_count)
	{
		changed = true;
	}

	for (uint8_t list_idx = new_list_count; list_idx < old_list_count; list_idx++)
	{
		for (int64_t slot_idx = 0; slot_idx < node_instance.connection_lists[list_idx].size(); slot_idx++)
		{
			if (node_instance.connection_lists[list_idx][slot_idx].node_id != NODE_ID_INVALID)
			{
				node_instance.connection_lists.write[list_idx].write[slot_idx] = FlowScriptNodeReference();
				emit_signal(SNAME("node_connection_changed"), p_node_id, list_idx, slot_idx);
			}
		}
	}

	node_instance.connection_lists.resize(new_list_count);

	uint8_t curr_list_idx = 0;
	for (const int64_t &curr_desired_length : length_list)
	{
		if (node_instance.connection_lists[curr_list_idx].size() != curr_desired_length)
		{
			node_instance.connection_lists.write[curr_list_idx].resize(curr_desired_length);
			changed = true;
		}
		curr_list_idx++;
	}
	if (changed)
	{
		emit_changed();
	}
}


void FlowScript::on_node_changed(FlowScriptNodeID p_node_id)
{
	update_connection_outputs_for_node(p_node_id);
	emit_changed();
}


void FlowScript::on_include_changed(FlowScriptIncludeID p_include_id)
{
	// pass, emitting changed could be helpful but i think it'd mess things up for the editor plugin, don't bother
}


void FlowScript::bind_set_node_connection(const FlowScriptNodeID p_from_node_id, const uint8_t p_list, const int64_t p_slot, const FlowScriptIncludeID p_to_include_id, const FlowScriptNodeID p_to_node_id)
{
	set_node_connection(p_from_node_id, FlowScriptNodeOutputConnection(p_list, p_slot), FlowScriptNodeReference(p_to_include_id, p_to_node_id));
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
	remove_include_flow_script_list(list);
}


PackedInt32Array FlowScript::bind_get_node_id_list() const
{
	PackedInt32Array ret;
	ret.resize(node_map.size());
	int i = 0;
	for (const KeyValue<FlowScriptNodeID, FlowScriptNodeInstance> &kv : node_map)
	{
		ret.write[i] = kv.key;
		i++;
	}
	return ret;
}


PackedInt32Array FlowScript::bind_get_include_id_list() const
{
	PackedInt32Array ret;
	int size = 0;
	for (int i = 0; i < INCLUDE_FLOW_SCRIPT_MAX; i++)
	{
		if (script_includes[i].is_valid())
		{
			size++;
		}
	}
	ret.resize(size);
	int i = 0;
	for (int j = 0; j < INCLUDE_FLOW_SCRIPT_MAX; j++)
	{
		if (script_includes[j].is_valid())
		{
			ret.write[i] = j;
			i++;
		}
	}
	return ret;
}


List<Ref<FlowScriptNode>> FlowScript::get_every_node_resource_recursive() const
{
	List<Ref<FlowScriptNode>> node_list;
	const FlowScript *this_const = this;
	internal_get_every_node_resource_recursive(this_const, &node_list);
	return node_list;
}


TypedArray<FlowScriptNode> FlowScript::bind_get_every_node_resource_recursive() const
{
	List<Ref<FlowScriptNode>> node_list;

	const FlowScript *this_const = this;
	internal_get_every_node_resource_recursive(this_const, &node_list);

	TypedArray<FlowScriptNode> ret;
	ret.resize(node_list.size());
	int i = 0;
	for (const Ref<FlowScriptNode> node_ref : node_list)
	{
		ret[i] = node_ref;
		i++;
	}
	return ret;
}


TypedArray<FlowScriptNode> FlowScript::bind_get_every_node_resource_connected_to_node(const FlowScriptNodeID p_origin_node_id, const bool p_include_origin) const
{
	List<Ref<FlowScriptNode>> node_list = get_every_node_resource_connected_to_node(p_origin_node_id, p_include_origin);
	
	TypedArray<FlowScriptNode> ret;
	ret.resize(node_list.size());
	int i = 0;
	for (const Ref<FlowScriptNode> node_ref : node_list)
	{
		ret[i] = node_ref;
		i++;
	}
	return ret;
}


List<Ref<FlowScriptNode>> FlowScript::get_every_node_resource_connected_to_node(const FlowScriptNodeID p_origin_node_id, const bool p_include_origin) const
{
	List<Ref<FlowScriptNode>> ret;
	ERR_FAIL_COND_V(!node_map.has(p_origin_node_id), ret);

	if (p_include_origin)
	{
		ret.push_back(node_map[p_origin_node_id].node);
	}

	const FlowScript *self_const_ptr = this;
	internal_get_every_node_resource_connected_to_recursive(self_const_ptr, p_origin_node_id, &ret);

	return ret;
}


void FlowScript::internal_get_every_node_resource_connected_to_recursive(const FlowScript *p_current_script, const FlowScriptNodeID p_current_origin_node_id, List<Ref<FlowScriptNode>> *p_node_list)
{
	ERR_FAIL_NULL(p_current_script);
	ERR_FAIL_COND(!p_current_script->node_map.has(p_current_origin_node_id));

	const FlowScriptNodeInstance &origin_node_instance = p_current_script->node_map[p_current_origin_node_id];
	for (uint8_t list_idx = 0; list_idx < origin_node_instance.connection_lists.size(); list_idx++)
	{
		for (int64_t slot_idx = 0; slot_idx < origin_node_instance.connection_lists[list_idx].size(); slot_idx++)
		{
			const FlowScriptNodeReference &target = origin_node_instance.connection_lists[list_idx][slot_idx];
			if (target.flow_script_id != FlowScript::NODE_ID_INVALID)
			{
				ERR_CONTINUE(!p_current_script->has_include_flow_script_instance(target.flow_script_id));
				const FlowScript *target_script_res = p_current_script->script_includes[target.flow_script_id].flow_script.ptr();
				ERR_CONTINUE(!target_script_res->node_map.has(target.node_id));
				p_node_list->push_back(target_script_res->node_map[target.node_id].node);
				internal_get_every_node_resource_connected_to_recursive(target_script_res, target.node_id, p_node_list);
			}
			else
			{
				if (target.node_id == FlowScript::NODE_ID_INVALID)
				{
					continue;
				}
				ERR_CONTINUE(!p_current_script->node_map.has(target.node_id));
				p_node_list->push_back(p_current_script->node_map[target.node_id].node);
				internal_get_every_node_resource_connected_to_recursive(p_current_script, target.node_id, p_node_list);
			}
		}
	}
}


void FlowScript::internal_get_every_node_resource_recursive(const FlowScript *p_current_level, List<Ref<FlowScriptNode>> *p_node_list)
{
	for (const KeyValue<FlowScriptNodeID, FlowScriptNodeInstance> &kv : p_current_level->node_map)
	{
		if (kv.value.node.is_valid())
		{
			p_node_list->push_back(kv.value.node);
		}
	}
	for (FlowScriptIncludeID i = 0; i < INCLUDE_FLOW_SCRIPT_MAX; i++)
	{
		if (p_current_level->script_includes[i].flow_script.is_valid())
		{
			internal_get_every_node_resource_recursive(p_current_level->script_includes[i].flow_script.ptr(), p_node_list);
		}
	}
}


FlowScript::FlowScript()
{
	cache_node_id_array_ptr = &cache_node_id_array;
}
