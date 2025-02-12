#ifndef FLOW_SCRIPT_NODE_OUTPUT_CONNECTION_HPP
#define FLOW_SCRIPT_NODE_OUTPUT_CONNECTION_HPP


#include "flow_script_typedefs.hpp"
#include "core/variant/variant.h"
#include "core/variant/dictionary.h"


struct FlowScriptNodeOutputConnection
{
	FlowScriptNodeConnectionListNo list = 0;
	FlowScriptNodeConnectionListSlotNo slot = 0;

	static FlowScriptNodeOutputConnection create_from_dictionary(const Dictionary &p_dictionary);
	static FlowScriptNodeOutputConnection create_connection(const FlowScriptNodeConnectionListNo p_list, const FlowScriptNodeConnectionListSlotNo p_slot);
	Dictionary to_dictionary() const;
	uint32_t hash() const;

	bool operator==(const FlowScriptNodeOutputConnection &p_other) const;
	bool operator!=(const FlowScriptNodeOutputConnection &p_other) const;

	FlowScriptNodeOutputConnection();
	FlowScriptNodeOutputConnection(const FlowScriptNodeConnectionListNo p_list, const FlowScriptNodeConnectionListSlotNo p_slot);
};


struct FlowScriptNodeOutputConnectionHasher final
{
	static _FORCE_INLINE_ uint32_t hash(const FlowScriptNodeOutputConnection &p_output)
	{
		return p_output.hash();
	}
};


#endif // FLOW_SCRIPT_NODE_OUTPUT_CONNECTION_HPP
