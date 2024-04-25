#ifndef EXPRESSION_BRANCH_FLOW_NODE_HPP
#define EXPRESSION_BRANCH_FLOW_NODE_HPP

#include "../core_types/flow_node.hpp"


class ExpressionBranchFlowNode final : public FlowNode
{
	GDCLASS(ExpressionBranchFlowNode, FlowNode);

private:
	String expression;
	FlowNodeID true_flow_node_id = FLOW_NODE_ID_NIL;
	FlowNodeID false_flow_node_id = FLOW_NODE_ID_NIL;

protected:
	static void _bind_methods();

	virtual void _execute(FlowNodeExecutionState *p_state) override;
	virtual bool _is_property_flow_node_reference(const StringName &p_name) const override;
	virtual void _on_external_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to) override;

public:
	void set_expression(const String &p_expression);
	String get_expression() const;
	void set_true_flow_node_id(const FlowNodeID p_flow_node_id);
	int get_true_flow_node_id() const;
	void set_false_flow_node_id(const FlowNodeID p_flow_node_id);
	int get_false_flow_node_id() const;
};


#endif
