#ifndef FLOW_SCRIPT_NODE_INSTANCE_HPP
#define FLOW_SCRIPT_NODE_INSTANCE_HPP


#include "core/math/vector2i.h"
#include "core/templates/hash_map.h"
#include "core/variant/callable.h"
#include "flow_script_node.hpp"


class FlowScriptNodeInstance
{
public:
	// Reference to the node.
	// This is the only Ref<FlowScriptNode> that needs to exist. Most of the code can use direct node pointers instead.
	Ref<FlowScriptNode> node;
	// Maybe position should be stored as Point2 instead?
	Point2i position;
	// Connections are stored as a map from a connection slot -> node ID list
	HashMap<uint8_t, Vector<FlowScriptNodeID>> connections;

	void set_node(const Ref<FlowScriptNode> &p_node);
	void set_state_json(const Dictionary &p_state);
	void get_state_json(Dictionary &r_state);
	bool is_valid() const;
	void clear_connections();
	void set_connection_list_length(const uint8_t p_list, const int64_t p_length);
	int64_t get_connection_list_length(const uint8_t p_list) const;
	bool has_connection_at(const uint8_t p_list, const int64_t p_slot) const;
	void set_connection_at(const uint8_t p_list, const int64_t p_slot, const FlowScriptNodeID p_target_node_id);
	FlowScriptNodeID get_connection_at(const uint8_t p_list, const int64_t p_slot) const;
	void add_connection(const uint8_t p_list, const FlowScriptNodeID p_node_id);
};


#endif // FLOW_SCRIPT_NODE_INSTANCE_HPP
