#ifndef FLOW_SCRIPT_NODE_REFERENCE_HPP
#define FLOW_SCRIPT_NODE_REFERENCE_HPP


#include "typedefs.hpp"


class FlowScriptNodeReference
{
public:
	FlowScriptIncludeID flow_script_id;
	FlowScriptNodeID node_id;

	bool is_valid() const;

	bool operator==(const FlowScriptNodeReference &p_other) const;
	bool operator!=(const FlowScriptNodeReference &p_other) const;

	FlowScriptNodeReference();
	FlowScriptNodeReference(const FlowScriptNodeID p_node_id);
	FlowScriptNodeReference(const FlowScriptIncludeID p_flow_script_id, const FlowScriptNodeID p_node_id);
};


#endif // FLOW_SCRIPT_NODE_REFERENCE_HPP
