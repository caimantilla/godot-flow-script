#ifndef FLOW_SCRIPT_NODE_INSTANCE_HPP
#define FLOW_SCRIPT_NODE_INSTANCE_HPP


#include "flow_script_node.hpp"
#include "flow_script_node_reference.hpp"
#include "flow_script_node_output_connection.hpp"
#include "core/math/vector2i.h"
#include "core/templates/hash_map.h"
#include "core/variant/callable.h"
#include "core/variant/dictionary.h"


class FlowScriptNodeInstance final
{
public:
	// Reference to the node.
	// This is the only Ref<FlowScriptNode> that needs to exist. Most of the code can use direct node pointers instead.
	Ref<FlowScriptNode> node;
	// Maybe position should be stored as Point2 instead?
	Point2i position;
	// Connections are stored as a map from a connection slot -> node ID list
	Vector<Vector<FlowScriptNodeReference>> connection_lists;

	void set_node(const Ref<FlowScriptNode> &p_node);
	void set_state_json(const Dictionary &p_state);
	void get_state_json(Dictionary &r_state);
	bool is_valid() const;
	void clear_connections();
	void set_connection_list_count(const uint8_t p_count);
	uint8_t get_connection_list_count() const;
	void set_connection_list_length(const uint8_t p_list, const int64_t p_length);
	int64_t get_connection_list_length(const uint8_t p_list) const;
	bool has_connection(const FlowScriptNodeOutputConnection &p_connection) const;
	void set_connection(const FlowScriptNodeOutputConnection &p_connection, const FlowScriptNodeReference p_target);
	FlowScriptNodeReference get_connection(const FlowScriptNodeOutputConnection &p_connection) const;
	void set_connection_flow_script_id(const FlowScriptNodeOutputConnection &p_connection, const FlowScriptIncludeID p_flow_script_id);
	void set_connection_node_id(const FlowScriptNodeOutputConnection &p_connection, const FlowScriptNodeID p_node_id);
};


#endif // FLOW_SCRIPT_NODE_INSTANCE_HPP
