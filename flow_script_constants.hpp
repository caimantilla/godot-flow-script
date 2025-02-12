#ifndef FLOW_SCRIPT_CONSTANTS_HPP
#define FLOW_SCRIPT_CONSTANTS_HPP


#include <limits>
#include "flow_script_typedefs.hpp"
#include "core/object/object.h"
#include "core/object/class_db.h"


class FlowScriptConstants final : public Object
{
	GDCLASS(FlowScriptConstants, Object);

public:
	enum Type
	{
		PACKED_INCLUDE_ID_ARRAY = Variant::PACKED_INT32_ARRAY,
		PACKED_NODE_ID_ARRAY = Variant::PACKED_INT32_ARRAY,
		PACKED_EXECUTION_FIBER_ID_ARRAY = Variant::PACKED_INT32_ARRAY,
	};

public:
	enum : FlowScriptIncludeID
	{
		INCLUDE_ID_INVALID = -1,
		INCLUDE_ID_MIN = 1,
		INCLUDE_ID_MAX = std::numeric_limits<FlowScriptIncludeID>::max() - 1,
	};

	enum : FlowScriptNodeID
	{
		NODE_ID_INVALID = -1,
		NODE_ID_MIN = 1,
		NODE_ID_MAX = std::numeric_limits<FlowScriptNodeID>::max() - 1,
	};

	enum : FlowScriptExecutionFiberID
	{
		FIBERS_MAX = 31,
		FIBER_ID_MAX = FIBERS_MAX - 1,
		FIBER_ID_INVALID = -1,
	};

	enum : FlowScriptNodeConnectionListNo
	{
		NODE_CONNECTION_LISTS_MAX = std::numeric_limits<FlowScriptNodeConnectionListNo>::max(),
		NODE_CONNECTION_LIST_INDEX_MAX = NODE_CONNECTION_LISTS_MAX - 1,
	};

	enum : FlowScriptNodeConnectionListSlotNo
	{
		NODE_CONNECTION_LIST_SLOTS_MAX = std::numeric_limits<FlowScriptNodeConnectionListSlotNo>::max(),
		NODE_CONNECTION_LIST_SLOT_NO_MAX = NODE_CONNECTION_LIST_SLOTS_MAX - 1,
	};

	enum : FlowScriptNodeConnectionListLength
	{
		NODE_CONNECTION_LIST_LENGTH_MAX = std::numeric_limits<FlowScriptNodeConnectionListLength>::max(),
	};

protected:
	static void _bind_methods();

public:
	static bool is_include_id_valid(const FlowScriptIncludeID p_include_id);
	static bool is_node_id_valid(const FlowScriptNodeID p_node_id);
};


#endif // FLOW_SCRIPT_CONSTANTS_HPP
