#include "core/core_string_names.h"
#include "flow_script.hpp"
#include "../singletons/flow_factory.hpp"
#include "../flow_constants.hpp"


static const String FLOW_NODES_PROPERTY_PREFIX = "flow_nodes/";
static const String PROPERTY_SEPARATOR = "/";


void FlowScript::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("clear"), &FlowScript::clear);
	ClassDB::bind_method(D_METHOD("has_flow_node", "flow_node_id"), &FlowScript::has_flow_node);
	ClassDB::bind_method(D_METHOD("get_flow_node", "flow_node_id"), &FlowScript::get_flow_node);
	ClassDB::bind_method(D_METHOD("get_flow_node_id_list"), &FlowScript::get_flow_node_id_list);

	ADD_SIGNAL(MethodInfo("clearing"));
	ADD_SIGNAL(MethodInfo("cleared"));
	ADD_SIGNAL(MethodInfo("flow_node_added", PropertyInfo(TYPE_FLOW_NODE_ID, "flow_node_id")));
	ADD_SIGNAL(MethodInfo("removing_flow_node", PropertyInfo(TYPE_FLOW_NODE_ID, "flow_node_id")));
	ADD_SIGNAL(MethodInfo("flow_node_removed", PropertyInfo(TYPE_FLOW_NODE_ID, "flow_node_id")));
	ADD_SIGNAL(MethodInfo("flow_node_list_changed"));
	ADD_SIGNAL(MethodInfo("flow_node_id_changed", PropertyInfo(TYPE_FLOW_NODE_ID, "from"), PropertyInfo(TYPE_FLOW_NODE_ID, "to")));
	ADD_SIGNAL(MethodInfo("multiple_flow_nodes_removed", PropertyInfo(TYPE_FLOW_NODE_ID_ARRAY, "flow_node_ids")));
}


void FlowScript::_get_property_list(List<PropertyInfo> *p_list) const
{
	for (const KeyValue<FlowNodeID, FlowNode *> &map_entry : flow_node_map)
	{
		FlowNodeID flow_node_id = map_entry.key;
		FlowNode *flow_node = map_entry.value;
		String flow_node_property_prefix = FLOW_NODES_PROPERTY_PREFIX + itos(flow_node_id) + "/";

		// Always store the node type ID first, as only properties which come after it will be loaded.
		// A FlowNode's properties cannot be loaded unless a type has already been found.
		PropertyInfo node_type_id_p_info = PropertyInfo(Variant::STRING, flow_node_property_prefix + "flow_type_id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR);
		p_list->push_back(node_type_id_p_info);

		List<PropertyInfo> flow_node_property_list;
		flow_node->get_property_list(&flow_node_property_list);

		for (PropertyInfo &flow_node_property_info : flow_node_property_list)
		{
			// filter script and of course we already stored the type id, don't save it twice please.
			if (flow_node_property_info.type == Variant::NIL || flow_node_property_info.name == "script" || flow_node_property_info.name == "flow_type_id")
			{
				continue;
			}

			flow_node_property_info.usage = flow_node_property_info.usage &~ PROPERTY_USAGE_EDITOR;
			flow_node_property_info.name = flow_node_property_prefix + flow_node_property_info.name;

			p_list->push_back(flow_node_property_info);
		}
	}
}


bool FlowScript::_set(const StringName &p_name, const Variant &p_value)
{
	StringName flow_node_sub_property;
	FlowNodeID flow_node_id = FLOW_NODE_ID_NIL;

	if (parse_flow_node_property(p_name, flow_node_sub_property, flow_node_id))
	{
		if (!has_flow_node(flow_node_id))
		{
			if (flow_node_sub_property == SNAME("flow_type_id"))
			{
				_create_new_flow_node(flow_node_id, p_value, false);
			}
		}

		if (has_flow_node(flow_node_id))
		{
			FlowNode *flow_node = get_flow_node(flow_node_id);
			bool success = false;
			flow_node->set(flow_node_sub_property, p_value, &success);
			
			if (success)
			{
				return true;
			}
		}
	}

	return false;
}


bool FlowScript::_get(const StringName &p_name, Variant &r_ret) const
{
	StringName flow_node_sub_property;
	FlowNodeID flow_node_id;

	if (parse_flow_node_property(p_name, flow_node_sub_property, flow_node_id))
	{
		FlowNode *flow_node = get_flow_node(flow_node_id);
		bool success = false;
		r_ret = flow_node->get(flow_node_sub_property, &success);

		if (success)
		{
			return true;
		}
	}

	return false;
}


bool FlowScript::_property_can_revert(const StringName &p_name) const
{
	StringName flow_node_sub_property;
	FlowNodeID flow_node_id;

	if (parse_flow_node_property(p_name, flow_node_sub_property, flow_node_id))
	{
		FlowNode *flow_node = get_flow_node(flow_node_id);
		if (flow_node->property_can_revert(flow_node_sub_property))
		{
			return true;
		}
	}

	return false;
}


bool FlowScript::_property_get_revert(const StringName &p_name, Variant &r_property) const
{
	StringName flow_node_sub_property;
	FlowNodeID flow_node_id;

	if (parse_flow_node_property(p_name, flow_node_sub_property, flow_node_id))
	{
		FlowNode *flow_node = get_flow_node(flow_node_id);
		r_property = flow_node->property_get_revert(flow_node_sub_property);
		return true;
	}

	return false;
}


void FlowScript::clear()
{
	emit_signal(SNAME("clearing"));

	for (const KeyValue<FlowNodeID, FlowNode *> &map_entry : flow_node_map)
	{
		memdelete(map_entry.value);
	}

	flow_node_map.clear();

	emit_signal(SNAME("cleared"));
}


bool FlowScript::has_flow_node(const FlowNodeID p_flow_node_id) const
{
	return flow_node_map.has(p_flow_node_id);
}


FlowNode *FlowScript::get_flow_node(const FlowNodeID p_flow_node_id) const
{
	ERR_FAIL_COND_V(!has_flow_node(p_flow_node_id), nullptr);
	return flow_node_map.get(p_flow_node_id);
}


FlowNode *FlowScript::get_flow_node_by_name(const String &p_flow_node_name) const
{
	const String target_name = p_flow_node_name.strip_edges();

	if (!target_name.is_empty())
	{
		for (const KeyValue<FlowNodeID, FlowNode *> &map_entry : flow_node_map)
		{
			FlowNode *current_flow_node = map_entry.value;
			String current_flow_node_name = current_flow_node->get_flow_node_name().strip_edges();
			if (target_name == current_flow_node_name)
			{
				return current_flow_node;
			}
		}
	}

	return nullptr;
}


FlowNodeIDArray FlowScript::get_flow_node_id_list() const
{
	FlowNodeIDArray id_list;
	id_list.resize(flow_node_map.size());

	int i = 0;
	for (const KeyValue<FlowNodeID, FlowNode *> &map_entry : flow_node_map)
	{
		id_list.set(i, map_entry.key);
		i++;
	}

	return id_list;
}


int FlowScript::get_flow_node_count() const
{
	return flow_node_map.size();
}


FlowNode *FlowScript::create_new_flow_node(const String &p_flow_type_id)
{
	update_next_flow_node_id();
	FlowNode *flow_node = _create_new_flow_node(next_flow_node_id, p_flow_type_id, true);
	return flow_node;
}


bool FlowScript::remove_flow_node(const FlowNodeID p_flow_node_id)
{
	return _remove_flow_node(p_flow_node_id, true);
}


bool FlowScript::change_flow_node_id(const FlowNodeID p_from, const FlowNodeID p_to)
{
	return _change_flow_node_id(p_from, p_to, true);
}


FlowNodeIDArray FlowScript::remove_multiple_flow_nodes(const FlowNodeIDArray &p_flow_node_ids)
{
	return _remove_multiple_flow_nodes(p_flow_node_ids, true);
}


bool FlowScript::is_new_flow_node_id_valid(const FlowNodeID p_flow_node_id) const
{
	if (p_flow_node_id < FLOW_NODE_ID_MIN || p_flow_node_id >= FLOW_NODE_ID_MAX || flow_node_map.has(p_flow_node_id))
	{
		return false;
	}

	return true;
}


void FlowScript::on_flow_node_property_list_changed()
{
	notify_property_list_changed();
}


void FlowScript::on_flow_node_list_changed(const bool p_emit)
{
	// Honest it's probably not necessary to emit this, it's not like the nodes are shown in the inspector anyways and they're all get serialized either way
	// notify_property_list_changed();

	if (p_emit)
	{
		emit_signal(SNAME("flow_node_list_changed"));
	}
}


void FlowScript::on_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to, const bool p_emit)
{
	on_flow_node_list_changed(p_emit);
	if (p_emit)
	{
		emit_signal(SNAME("flow_node_id_changed"), p_from, p_to);
	}
}


void FlowScript::clear_references_to_flow_node(const FlowNodeID p_flow_node_id)
{
	for (KeyValue<FlowNodeID, FlowNode *> &map_entry : flow_node_map)
	{
		FlowNode *node = map_entry.value;
		node->on_external_flow_node_id_changed(p_flow_node_id, FLOW_NODE_ID_NIL);
	}
}


void FlowScript::clear_references_to_multiple_flow_nodes(const FlowNodeIDArray &p_flow_node_ids)
{
	for (const KeyValue<FlowNodeID, FlowNode *> &map_entry : flow_node_map)
	{
		FlowNode *node = map_entry.value;

		for (const FlowNodeID flow_node_id_to_erase : p_flow_node_ids)
		{
			node->on_external_flow_node_id_changed(flow_node_id_to_erase, FLOW_NODE_ID_NIL);
		}
	}
}


bool FlowScript::parse_flow_node_property(const StringName &p_in_property, StringName &p_out_property, FlowNodeID &p_out_flow_node_id) const
{
	String in_property = String(p_in_property);

	if (in_property.begins_with(FLOW_NODES_PROPERTY_PREFIX))
	{
		String flow_node_id_str = in_property.get_slice(PROPERTY_SEPARATOR, 1);

		p_out_property = in_property.substr(FLOW_NODES_PROPERTY_PREFIX.length() + flow_node_id_str.length() + 1);
		p_out_flow_node_id = flow_node_id_str.to_int();

		return true;
	}

	return false;
}


bool FlowScript::update_next_flow_node_id()
{
	for (FlowNodeID i = FLOW_NODE_ID_MIN; i < FLOW_NODE_ID_MAX; i++)
	{
		if (!flow_node_map.has(i))
		{
			next_flow_node_id = i;
			return true;
		}
	}

	next_flow_node_id = FLOW_NODE_ID_NIL;
	ERR_FAIL_V_MSG(false, "All FlowNode slots occupied... :(");
}


FlowNode *FlowScript::_create_new_flow_node(const FlowNodeID p_flow_node_id, const String &p_flow_type_id, const bool p_emit)
{
	if (!is_new_flow_node_id_valid(p_flow_node_id))
	{
		return nullptr;
	}

	FlowNode *new_node = FlowFactory::get_singleton()->create_flow_node_of_flow_type(p_flow_type_id);
	ERR_FAIL_NULL_V(new_node, nullptr);

	new_node->flow_node_id = p_flow_node_id;
	new_node->connect(CoreStringNames::get_singleton()->property_list_changed, callable_mp(this, &FlowScript::on_flow_node_property_list_changed));

	flow_node_map.insert(p_flow_node_id, new_node);

	if (p_emit)
	{
		emit_signal(SNAME("flow_node_added"), p_flow_node_id);
	}

	on_flow_node_list_changed(p_emit);
	return new_node;
}


bool FlowScript::_remove_flow_node(const FlowNodeID p_flow_node_id, const bool p_emit)
{
	if (!has_flow_node(p_flow_node_id))
	{
		WARN_PRINT(vformat("Trying to delete non-existent flow node \"%s\".", p_flow_node_id));
		return false;
	}

	FlowNode *node = flow_node_map.get(p_flow_node_id);
	node->disconnect(CoreStringNames::get_singleton()->property_list_changed, callable_mp(this, &FlowScript::on_flow_node_property_list_changed));

	// Do anything important before deletion! Emit even when not supposed to!!!!!!!
	emit_signal(SNAME("removing_flow_node"), p_flow_node_id);

	flow_node_map.erase(p_flow_node_id);
	SceneTree::get_singleton()->queue_delete(node);

	if (p_emit)
	{
		emit_signal(SNAME("flow_node_removed"), p_flow_node_id);
	}

	on_flow_node_list_changed(p_emit);
	return true;
}


bool FlowScript::_change_flow_node_id(const FlowNodeID p_from, const FlowNodeID p_to, bool p_emit)
{
	if (!has_flow_node(p_from))
	{
		ERR_PRINT("Trying to rename non-existent flow node.");
		return false;
	}
	else if (p_from == p_to)
	{
		ERR_PRINT("Trying to change the name of a FlowNode to itself.");
		return false;
	}
	else if (!is_new_flow_node_id_valid(p_to))
	{
		ERR_PRINT(vformat("Trying to change the ID of FlowNode #%d to invalid ID #%d.", p_from, p_to));
		return false;
	}

	FlowNode *node = get_flow_node(p_from);
	node->set_flow_node_id(p_to);
	on_flow_node_id_changed(p_from, p_to, p_emit);

	return true;
}


FlowNodeIDArray FlowScript::_remove_multiple_flow_nodes(const FlowNodeIDArray &p_flow_node_ids, const bool p_emit)
{
	FlowNodeIDArray successful_removals;

	if (p_flow_node_ids.size() == 1)
	{
		FlowNodeID node_id = p_flow_node_ids.get(0);
		if (_remove_flow_node(node_id, p_emit))
		{
			successful_removals.push_back(node_id);
		}
	}
	else
	{
		for (const FlowNodeID flow_node_id : p_flow_node_ids)
		{
			if (_remove_flow_node(flow_node_id, false))
			{
				successful_removals.push_back(flow_node_id);
			}
		}

		if (!successful_removals.is_empty() && p_emit)
		{
			emit_signal(SNAME("multiple_flow_nodes_removed"), successful_removals);
		}

		on_flow_node_list_changed(p_emit);
	}

	return successful_removals;
}


FlowScript::FlowScript()
{
	next_flow_node_id = FLOW_NODE_ID_MIN;
}


FlowScript::~FlowScript()
{
	clear();
}
