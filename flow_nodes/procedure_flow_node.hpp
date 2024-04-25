#ifndef PROCEDURE_FLOW_NODE_HPP
#define PROCEDURE_FLOW_NODE_HPP

#include "../core_types/flow_node.hpp"

class ProcedureFlowNode final : public FlowNode
{
	GDCLASS(ProcedureFlowNode, FlowNode);

private:
	FlowNodeID next_flow_node_id = FLOW_NODE_ID_NIL;

protected:
	static void _bind_methods();

	virtual void _execute(FlowNodeExecutionState *p_state) override;
	virtual bool _is_property_flow_node_reference(const StringName &p_name) const override;
	virtual void _on_external_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to) override;

public:
	void set_next_flow_node_id(const FlowNodeID p_flow_node_id);
	FlowNodeID get_next_flow_node_id() const;
};

#endif
