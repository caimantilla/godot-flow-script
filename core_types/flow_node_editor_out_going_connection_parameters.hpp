#ifndef FLOW_NODE_EDITOR_OUTGOING_CONNECTION_PARAMETERS_HPP
#define FLOW_NODE_EDITOR_OUTGOING_CONNECTION_PARAMETERS_HPP

#include "core/object/ref_counted.h"
#include "../flow_constants.hpp"

class FlowNodeEditorOutGoingConnectionParameters final : public RefCounted
{
	GDCLASS(FlowNodeEditorOutGoingConnectionParameters, RefCounted);

private:
	FlowNodeID target_flow_node_id;
	int origin_slot;
	int origin_port;

protected:
	static void _bind_methods();

public:
	void set_target_flow_node_id(const FlowNodeID p_flow_node_id);
	FlowNodeID get_target_flow_node_id() const;

	void set_origin_slot(int p_origin_slot);
	int get_origin_slot() const;

	void set_origin_port(int p_origin_port);
	int get_origin_port() const;

	static Ref<FlowNodeEditorOutGoingConnectionParameters> create_using_slot(const int p_origin_slot, const FlowNodeID p_target_flow_node_id);
	static Ref<FlowNodeEditorOutGoingConnectionParameters> create_using_port(const int p_origin_port, const FlowNodeID p_target_flow_node_id);
};

#endif
