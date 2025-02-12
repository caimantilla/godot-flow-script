#include "flow_script_connection_info.hpp"
#include "core/templates/hashfuncs.h"


FlowScriptConnectionInfo FlowScriptConnectionInfo::create(const FlowScriptNodeID p_from_node_id, const FlowScriptNodeOutputConnection &p_from_output, const FlowScriptNodeReference &p_target)
{
	FlowScriptConnectionInfo info;
	info.from_node_id = p_from_node_id;
	info.from_output = p_from_output;
	info.target = p_target;
	return info;
}


FlowScriptConnectionInfo FlowScriptConnectionInfo::create_from_dictionary(const Dictionary &p_dict)
{
	FlowScriptConnectionInfo info;

	info.from_node_id = p_dict.get("from_node_id", FlowScriptConstants::NODE_ID_INVALID);
	info.from_output = FlowScriptNodeOutputConnection::create_from_dictionary(p_dict.get("from_output", Dictionary()));
	info.target = FlowScriptNodeReference::create_from_dictionary(p_dict.get("target", Dictionary()));

	return info;
}


Dictionary FlowScriptConnectionInfo::to_dictionary() const
{
	Dictionary d;

	d["from_node_id"] = from_node_id;
	d["from_output"] = from_output.to_dictionary();
	d["target"] = target.to_dictionary();

	return d;
}


uint32_t FlowScriptConnectionInfo::hash() const
{
	uint32_t h = hash_murmur3_one_32(from_node_id);
	h = hash_murmur3_one_32(from_output.list, h);
	h = hash_murmur3_one_32(from_output.slot, h);
	h = hash_murmur3_one_32(target.include_id, h);
	h = hash_murmur3_one_32(target.node_id, h);
	return hash_fmix32(h);
}


bool FlowScriptConnectionInfo::operator==(const FlowScriptConnectionInfo &p_other) const
{
	return (
			p_other.from_node_id == from_node_id
			&& p_other.from_output == from_output
			&& p_other.target == target
	);
}


bool FlowScriptConnectionInfo::operator!=(const FlowScriptConnectionInfo &p_other) const
{
	return (
		p_other.from_node_id != from_node_id
		|| p_other.from_output != from_output
		|| p_other.target != target
	);
}


FlowScriptConnectionInfo::FlowScriptConnectionInfo()
{
}


FlowScriptConnectionInfo::~FlowScriptConnectionInfo()
{
}
