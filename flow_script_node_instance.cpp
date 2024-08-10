#include "flow_script_node_instance.hpp"
#include "flow_script.hpp"


void FlowScriptNodeInstance::set_node(const Ref<FlowScriptNode> &p_node)
{
#ifdef TOOLS_ENABLED
	if (node.is_valid())
	{
		node->editor_dependency_dec();
	}
	if (p_node.is_valid())
	{
		p_node->editor_dependency_inc();
	}
#endif // TOOLS_ENABLED
	node = p_node;
}


void FlowScriptNodeInstance::get_state_json(Dictionary &r_state)
{
	if (node.is_valid())
	{
		r_state["type"] = node->get_type_id();
		Dictionary node_dict;
		node->get_json_data(node_dict);
		r_state["data"] = node_dict;
	}
}


bool FlowScriptNodeInstance::is_valid() const
{
	return node.is_valid();
}


void FlowScriptNodeInstance::clear_connections()
{
	connections.clear();
}


void FlowScriptNodeInstance::set_connection_list_length(const uint8_t p_list, const int64_t p_length)
{
	ERR_FAIL_COND(!connections.has(p_list));
	int64_t old_length;
	connections.get(p_list).resize(p_length);
	for (int64_t i = old_length; i < p_length; i++)
	{
		connections.get(p_list).set(i, FlowScript::NODE_ID_INVALID);
	}
}


int64_t FlowScriptNodeInstance::get_connection_list_length(const uint8_t p_list) const
{
	if (connections.has(p_list))
		return connections.get(p_list).size();
	return 0;
}


bool FlowScriptNodeInstance::has_connection_at(const uint8_t p_list, const int64_t p_slot) const
{
	return p_slot > -1 && connections.has(p_list) && p_slot < connections.get(p_list).size();
}


void FlowScriptNodeInstance::set_connection_at(const uint8_t p_list, const int64_t p_slot, const FlowScriptNodeID p_target_node_id)
{
	if (!connections.has(p_list))
	{
		connections.insert(p_list, Vector<FlowScriptNodeID>());
	}
	if (p_slot >= connections.get(p_list).size())
	{
		set_connection_list_length(p_list, p_slot + 1);
	}
	connections.get(p_list).set(p_slot, p_target_node_id);
}


FlowScriptNodeID FlowScriptNodeInstance::get_connection_at(const uint8_t p_list, const int64_t p_slot) const
{
	ERR_FAIL_COND_V(!has_connection_at(p_list, p_slot), FlowScript::NODE_ID_INVALID);
	return connections.get(p_list).get(p_slot);
}


void FlowScriptNodeInstance::add_connection(const uint8_t p_list, const FlowScriptNodeID p_node_id)
{
	if (!connections.has(p_list))
		connections.insert(p_list, Vector<FlowScriptNodeID>());
	connections.get(p_list).push_back(p_node_id);
}
