#include "flow_script_node_output_connection.hpp"
#include "core/templates/hashfuncs.h"


FlowScriptNodeOutputConnection FlowScriptNodeOutputConnection::create_from_dictionary(const Dictionary &p_dictionary)
{
	FlowScriptNodeOutputConnection ret;
	ret.list = p_dictionary.get("list", 0);
	ret.slot = p_dictionary.get("slot", 0);
	return ret;
}


FlowScriptNodeOutputConnection FlowScriptNodeOutputConnection::create_connection(const FlowScriptNodeConnectionListNo p_list, const FlowScriptNodeConnectionListSlotNo p_slot)
{
	FlowScriptNodeOutputConnection ret;
	ret.list = p_list;
	ret.slot = p_slot;
	return ret;
}


Dictionary FlowScriptNodeOutputConnection::to_dictionary() const
{
	Dictionary ret;
	ret["list"] = list;
	ret["slot"] = slot;
	return ret;
}


uint32_t FlowScriptNodeOutputConnection::hash() const
{
	uint32_t h = hash_murmur3_one_32(list);
	h = hash_murmur3_one_32(slot, h);
	return hash_fmix32(h);
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
}


FlowScriptNodeOutputConnection::FlowScriptNodeOutputConnection(const FlowScriptNodeConnectionListNo p_list, const FlowScriptNodeConnectionListSlotNo p_slot)
{
	list = p_list;
	slot = p_slot;
}
