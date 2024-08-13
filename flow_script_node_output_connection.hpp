#ifndef FLOW_SCRIPT_NODE_OUTPUT_CONNECTION_HPP
#define FLOW_SCRIPT_NODE_OUTPUT_CONNECTION_HPP


#include "typedefs.hpp"
#include "core/variant/variant.h"
#include "core/variant/dictionary.h"


class FlowScriptNodeOutputConnection
{
public:
	uint8_t list;
	int64_t slot;

	static FlowScriptNodeOutputConnection create_from_dictionary(const Dictionary &p_dictionary);
	Dictionary to_dictionary();

	bool operator==(const FlowScriptNodeOutputConnection &p_other) const;
	bool operator!=(const FlowScriptNodeOutputConnection &p_other) const;

	FlowScriptNodeOutputConnection();
	FlowScriptNodeOutputConnection(const uint8_t p_list, const int64_t p_slot);
};


#endif // FLOW_SCRIPT_NODE_OUTPUT_CONNECTION_HPP

