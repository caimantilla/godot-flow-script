#include "flow_script_node_output_connection.hpp"


FlowScriptNodeOutputConnection FlowScriptNodeOutputConnection::create_from_dictionary(const Dictionary &p_dictionary)
{
	FlowScriptNodeOutputConnection ret;
	if (p_dictionary.has("list"))
	{
		ret.list = p_dictionary["list"];
	}
	if (p_dictionary.has("slot"))
	{
		ret.slot = p_dictionary["slot"];
	}
	return ret;
}


Dictionary FlowScriptNodeOutputConnection::to_dictionary()
{
	Dictionary ret;
	ret["list"] = list;
	ret["slot"] = slot;
	return ret;
}


bool FlowScriptNodeOutputConnection::operator==(const FlowScriptNodeOutputConnection &p_other) const
{
	return list == p_other.list && slot == p_other.slot;
}


bool FlowScriptNodeOutputConnection::operator!=(const FlowScriptNodeOutputConnection &p_other) const
{
	return list != p_other.list || slot != p_other.slot;
}


FlowScriptNodeOutputConnection::FlowScriptNodeOutputConnection()
{
	list = 0;
	slot = 0;
}


FlowScriptNodeOutputConnection::FlowScriptNodeOutputConnection(const uint8_t p_list, const int64_t p_slot)
{
	list = p_list;
	slot = p_slot;
}
