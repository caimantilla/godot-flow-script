#ifndef FLOW_SCRIPT_NODE_REFERENCE_HPP
#define FLOW_SCRIPT_NODE_REFERENCE_HPP


#include "flow_script_typedefs.hpp"
#include "flow_script_constants.hpp"
#include "core/templates/hashfuncs.h"
#include "core/variant/dictionary.h"


struct FlowScriptNodeReference final
{
	FlowScriptIncludeID include_id = FlowScriptConstants::INCLUDE_ID_INVALID;
	FlowScriptNodeID node_id = FlowScriptConstants::NODE_ID_INVALID;

	bool is_valid() const;
	Dictionary to_dictionary() const;
	uint32_t hash() const;

	bool operator==(const FlowScriptNodeReference &p_other) const;
	bool operator!=(const FlowScriptNodeReference &p_other) const;

	static FlowScriptNodeReference create_from_dictionary(const Dictionary &p_dictionary);
	static FlowScriptNodeReference create_null_reference();
	static FlowScriptNodeReference create_same_script_reference(const FlowScriptNodeID p_node_id);
	static FlowScriptNodeReference create_include_script_reference(const FlowScriptIncludeID p_include_id, const FlowScriptNodeID p_node_id);

	FlowScriptNodeReference();
};


struct FlowScriptNodeReferenceHasher final
{
	static _FORCE_INLINE_ uint32_t hash(const FlowScriptNodeReference &p_target)
	{
		return p_target.hash();
	}
};


#endif // FLOW_SCRIPT_NODE_REFERENCE_HPP
