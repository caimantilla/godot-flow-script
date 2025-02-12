#ifndef FLOW_SCRIPT_CONNECTION_INFO_HPP
#define FLOW_SCRIPT_CONNECTION_INFO_HPP


#include "flow_script_typedefs.hpp"
#include "flow_script_constants.hpp"
#include "flow_script_node_output_connection.hpp"
#include "flow_script_node_reference.hpp"
#include "core/templates/hashfuncs.h"


struct FlowScriptConnectionInfo final
{
	FlowScriptNodeID from_node_id = FlowScriptConstants::NODE_ID_INVALID;
	FlowScriptNodeOutputConnection from_output = FlowScriptNodeOutputConnection::create_connection(0, 0);
	FlowScriptNodeReference target = FlowScriptNodeReference::create_null_reference();

	static FlowScriptConnectionInfo create(const FlowScriptNodeID p_from_node_id, const FlowScriptNodeOutputConnection &p_from_output, const FlowScriptNodeReference &p_target);
	static FlowScriptConnectionInfo create_from_dictionary(const Dictionary &p_dict);
	Dictionary to_dictionary() const;
	uint32_t hash() const;

	bool operator==(const FlowScriptConnectionInfo &p_other) const;
	bool operator!=(const FlowScriptConnectionInfo &p_other) const;

	FlowScriptConnectionInfo();
	~FlowScriptConnectionInfo();
};


struct FlowScriptConnectionInfoHasher final
{
	static _FORCE_INLINE_ uint32_t hash(const FlowScriptConnectionInfo &p_connection_info)
	{
		return p_connection_info.hash();
	}
};


#endif // FLOW_SCRIPT_CONNECTION_INFO_HPP
