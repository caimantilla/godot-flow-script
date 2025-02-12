#include "flow_script_node_reference.hpp"


bool FlowScriptNodeReference::operator==(const FlowScriptNodeReference &p_other) const
{
	return include_id == p_other.include_id && node_id == p_other.node_id;
}


bool FlowScriptNodeReference::operator!=(const FlowScriptNodeReference &p_other) const
{
	return include_id != p_other.include_id || node_id != p_other.node_id;
}


bool FlowScriptNodeReference::is_valid() const
{
	return node_id != FlowScriptConstants::NODE_ID_INVALID;
}


Dictionary FlowScriptNodeReference::to_dictionary() const
{
	Dictionary ret;
	ret["include_id"] = include_id;
	ret["node_id"] = node_id;
	return ret;
}


uint32_t FlowScriptNodeReference::hash() const
{
	uint32_t h = hash_murmur3_one_32(include_id);
	h = hash_murmur3_one_32(node_id, h);
	return hash_fmix32(h);
}


FlowScriptNodeReference FlowScriptNodeReference::create_from_dictionary(const Dictionary &p_dictionary)
{
	FlowScriptNodeReference ret;

	ret.include_id = p_dictionary.get("include_id", FlowScriptConstants::INCLUDE_ID_INVALID);
	ret.node_id = p_dictionary.get("node_id", FlowScriptConstants::NODE_ID_INVALID);

	return ret;
}


FlowScriptNodeReference FlowScriptNodeReference::create_null_reference()
{
	FlowScriptNodeReference ret;
	ret.include_id = FlowScriptConstants::INCLUDE_ID_INVALID;
	ret.node_id = FlowScriptConstants::NODE_ID_INVALID;
	return ret;
}


FlowScriptNodeReference FlowScriptNodeReference::create_same_script_reference(const FlowScriptNodeID p_node_id)
{
	FlowScriptNodeReference ret;
	ret.include_id = FlowScriptConstants::INCLUDE_ID_INVALID;
	ret.node_id = p_node_id;
	return ret;
}


FlowScriptNodeReference FlowScriptNodeReference::create_include_script_reference(const FlowScriptIncludeID p_include_id, const FlowScriptNodeID p_node_id)
{
	FlowScriptNodeReference ret;
	ret.include_id = p_include_id;
	ret.node_id = p_node_id;
	return ret;
}


FlowScriptNodeReference::FlowScriptNodeReference()
{
}
