#include "procedure_flow_node.hpp"


void ProcedureFlowNode::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_next_flow_node_id", "node_id"), &ProcedureFlowNode::set_next_flow_node_id);
	ClassDB::bind_method(D_METHOD("get_next_flow_node_id"), &ProcedureFlowNode::get_next_flow_node_id);

	ADD_PROPERTY(PropertyInfo(TYPE_FLOW_NODE_ID, "next_flow_node_id"), "set_next_flow_node_id", "get_next_flow_node_id");
}


void ProcedureFlowNode::set_next_flow_node_id(const FlowNodeID p_flow_node_id)
{
	if (p_flow_node_id == next_flow_node_id)
	{
		return;
	}

	next_flow_node_id = p_flow_node_id;
	emit_changed();
}


FlowNodeID ProcedureFlowNode::get_next_flow_node_id() const
{
	return next_flow_node_id;
}


void ProcedureFlowNode::_execute(FlowNodeExecutionState *p_state)
{
	p_state->finish(next_flow_node_id, Variant());
}


bool ProcedureFlowNode::_is_property_flow_node_reference(const StringName &p_name) const
{
	return p_name == SNAME("next_flow_node_id");
}


void ProcedureFlowNode::_on_external_flow_node_id_changed(const int p_from, const int p_to)
{
	if (next_flow_node_id == p_from)
	{
		next_flow_node_id = p_to;
	}
}
