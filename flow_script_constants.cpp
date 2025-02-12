#include "flow_script_constants.hpp"


void FlowScriptConstants::_bind_methods()
{
	BIND_CONSTANT(NODE_ID_INVALID);
	BIND_CONSTANT(NODE_ID_MIN);
	BIND_CONSTANT(NODE_ID_MAX);

	BIND_CONSTANT(INCLUDE_ID_INVALID);
	BIND_CONSTANT(INCLUDE_ID_MIN);
	BIND_CONSTANT(INCLUDE_ID_MAX);

	BIND_CONSTANT(FIBERS_MAX);
	BIND_CONSTANT(FIBER_ID_MAX);
	BIND_CONSTANT(FIBER_ID_INVALID);

	BIND_CONSTANT(NODE_CONNECTION_LISTS_MAX);
	BIND_CONSTANT(NODE_CONNECTION_LIST_INDEX_MAX);

	BIND_CONSTANT(NODE_CONNECTION_LIST_SLOTS_MAX);
	BIND_CONSTANT(NODE_CONNECTION_LIST_SLOT_NO_MAX);

	BIND_CONSTANT(NODE_CONNECTION_LIST_LENGTH_MAX);

	ClassDB::bind_static_method("FlowScriptConstants", D_METHOD("is_include_id_valid", "include_id"), &FlowScriptConstants::is_include_id_valid);
	ClassDB::bind_static_method("FlowScriptConstants", D_METHOD("is_node_id_valid", "node_id"), &FlowScriptConstants::is_node_id_valid);
}


bool FlowScriptConstants::is_include_id_valid(const FlowScriptIncludeID p_include_id)
{
	return p_include_id >= INCLUDE_ID_MIN && p_include_id <= INCLUDE_ID_MAX;
}


bool FlowScriptConstants::is_node_id_valid(const FlowScriptNodeID p_node_id)
{
	return p_node_id >= NODE_ID_MIN && p_node_id <= NODE_ID_MAX;
}
