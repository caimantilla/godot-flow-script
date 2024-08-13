#include "flow_script_node_reference.hpp"
#include "flow_script.hpp"


bool FlowScriptNodeReference::is_valid() const
{
	return flow_script_id != FlowScript::NODE_ID_INVALID;
}


bool FlowScriptNodeReference::operator==(const FlowScriptNodeReference &p_other) const
{
	return flow_script_id == p_other.flow_script_id && node_id == p_other.node_id;
}


bool FlowScriptNodeReference::operator!=(const FlowScriptNodeReference &p_other) const
{
	return flow_script_id != p_other.flow_script_id || node_id != p_other.node_id;
}


FlowScriptNodeReference::FlowScriptNodeReference()
{
	flow_script_id = FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID;
	node_id = FlowScript::NODE_ID_INVALID;
}


FlowScriptNodeReference::FlowScriptNodeReference(const FlowScriptNodeID p_node_id)
{
	flow_script_id = FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID;
	node_id = p_node_id;
}



FlowScriptNodeReference::FlowScriptNodeReference(const FlowScriptIncludeID p_flow_script_id, const FlowScriptNodeID p_node_id)
{
	flow_script_id = p_flow_script_id;
	node_id = p_node_id;
}
