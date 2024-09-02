#include "flow_script_node_instance.hpp"
#include "flow_script.hpp"
#include "core/object/script_language.h"
#include "core/io/resource_loader.h"


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


void FlowScriptNodeInstance::set_state_json(const Dictionary &p_state)
{
	if (p_state.has("class"))
	{
		node = ClassDB::instantiate(p_state["class"]);
	}
	if (node.is_valid())
	{
		if (p_state.has("script"))
		{
			Ref<Script> scr = ResourceLoader::load(p_state["script"], "Script");
			node->set_script(scr);
		}
		if (p_state.has("data"))
		{
			node->set_json_data(p_state["data"]);
		}
	}
}


void FlowScriptNodeInstance::get_state_json(Dictionary &r_state)
{
	if (node.is_valid())
	{
		r_state["class"] = node->get_class();
		Ref<Script> scr = node->get_script();
		if (scr.is_valid())
		{
			r_state["script"] = scr->get_path();
		}
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
	connection_lists.clear();
}


void FlowScriptNodeInstance::set_connection_list_count(const uint8_t p_count)
{
	connection_lists.resize(p_count);
}


uint8_t FlowScriptNodeInstance::get_connection_list_count() const
{
	return connection_lists.size();
}


void FlowScriptNodeInstance::set_connection_list_length(const uint8_t p_list, const int64_t p_length)
{
	ERR_FAIL_INDEX(p_list, connection_lists.size());
	int64_t old_length = connection_lists.get(p_list).size();
	connection_lists.get(p_list).resize(p_length);
	for (int64_t i = old_length; i < p_length; i++)
	{
		connection_lists.get(p_list).write[i] = FlowScript::NODE_ID_INVALID;
	}
}


int64_t FlowScriptNodeInstance::get_connection_list_length(const uint8_t p_list) const
{
	ERR_FAIL_INDEX_V(p_list, connection_lists.size(), 0);
	return connection_lists.get(p_list).size();
}


bool FlowScriptNodeInstance::has_connection(const FlowScriptNodeOutputConnection &p_connection) const
{
	return p_connection.list < connection_lists.size() && p_connection.slot > -1 && p_connection.slot < connection_lists[p_connection.list].size();
}


void FlowScriptNodeInstance::set_connection(const FlowScriptNodeOutputConnection &p_connection, const FlowScriptNodeReference p_target)
{
	ERR_FAIL_INDEX(p_connection.list, connection_lists.size());
	ERR_FAIL_INDEX(p_connection.slot, connection_lists.get(p_connection.list).size());
	connection_lists.get(p_connection.list).write[p_connection.slot] = p_target;
}


FlowScriptNodeReference FlowScriptNodeInstance::get_connection(const FlowScriptNodeOutputConnection &p_connection) const
{
	ERR_FAIL_INDEX_V(p_connection.list, connection_lists.size(), FlowScriptNodeReference());
	ERR_FAIL_INDEX_V(p_connection.slot, connection_lists.get(p_connection.list).size(), FlowScriptNodeReference());
	return connection_lists.get(p_connection.list).get(p_connection.slot);
}


void FlowScriptNodeInstance::set_connection_flow_script_id(const FlowScriptNodeOutputConnection &p_connection, const FlowScriptIncludeID p_flow_script_id)
{
	set_connection(p_connection, FlowScriptNodeReference(p_flow_script_id, get_connection(p_connection).node_id));
}


void FlowScriptNodeInstance::set_connection_node_id(const FlowScriptNodeOutputConnection &p_connection, const FlowScriptNodeID p_node_id)
{
	set_connection(p_connection, FlowScriptNodeReference(get_connection(p_connection).flow_script_id, p_node_id));
}
