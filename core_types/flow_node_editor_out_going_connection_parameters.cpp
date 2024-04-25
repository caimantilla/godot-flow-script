#include "flow_node_editor_out_going_connection_parameters.hpp"


void FlowNodeEditorOutGoingConnectionParameters::_bind_methods()
{
	// ClassDB::bind_method(D_METHOD("set_target_node_id", "target_node_id"), &FlowNodeEditorOutGoingConnectionParameters::set_target_node_id);
	ClassDB::bind_method(D_METHOD("get_target_flow_node_id"), &FlowNodeEditorOutGoingConnectionParameters::get_target_flow_node_id);

	// ClassDB::bind_method(D_METHOD("set_origin_slot", "origin_slot"), &FlowNodeEditorOutGoingConnectionParameters::set_origin_slot);
	ClassDB::bind_method(D_METHOD("get_origin_slot"), &FlowNodeEditorOutGoingConnectionParameters::get_origin_slot);

	// ClassDB::bind_method(D_METHOD("set_origin_port", "origin_port"), &FlowNodeEditorOutGoingConnectionParameters::set_origin_port);
	ClassDB::bind_method(D_METHOD("get_origin_port"), &FlowNodeEditorOutGoingConnectionParameters::get_origin_port);

	ClassDB::bind_static_method("FlowNodeEditorOutGoingConnectionParameters", D_METHOD("create_using_slot", "origin_slot", "target_flow_node_id"), &FlowNodeEditorOutGoingConnectionParameters::create_using_slot);

	ADD_PROPERTY(PropertyInfo(TYPE_FLOW_NODE_ID, "target_flow_node_id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "", "get_target_flow_node_id");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "origin_slot", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "", "get_origin_slot");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "origin_port", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "", "get_origin_port");
}


void FlowNodeEditorOutGoingConnectionParameters::set_target_flow_node_id(const int p_flow_node_id)
{
	target_flow_node_id = p_flow_node_id;
}


int FlowNodeEditorOutGoingConnectionParameters::get_target_flow_node_id() const
{
	return target_flow_node_id;
}


void FlowNodeEditorOutGoingConnectionParameters::set_origin_slot(int p_origin_slot)
{
	origin_slot = p_origin_slot;
}


int FlowNodeEditorOutGoingConnectionParameters::get_origin_slot() const
{
	return origin_slot;
}


void FlowNodeEditorOutGoingConnectionParameters::set_origin_port(int p_origin_port)
{
	origin_port = p_origin_port;
}


int FlowNodeEditorOutGoingConnectionParameters::get_origin_port() const
{
	return origin_port;
}


Ref<FlowNodeEditorOutGoingConnectionParameters> FlowNodeEditorOutGoingConnectionParameters::create_using_slot(const int p_origin_slot, const FlowNodeID p_target_flow_node_id)
{
	Ref<FlowNodeEditorOutGoingConnectionParameters> ret;
	ret.instantiate();

	ret->set_origin_slot(p_origin_slot);
	ret->set_target_flow_node_id(p_target_flow_node_id);

	return ret;
}


Ref<FlowNodeEditorOutGoingConnectionParameters> FlowNodeEditorOutGoingConnectionParameters::create_using_port(const int p_origin_port, const FlowNodeID p_target_flow_node_id)
{
	Ref<FlowNodeEditorOutGoingConnectionParameters> ret;
	ret.instantiate();

	ret->set_origin_port(p_origin_port);
	ret->set_target_flow_node_id(p_target_flow_node_id);

	return ret;
}
