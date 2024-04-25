#ifndef WHILE_EXPRESSION_TRUE_LOOP_FLOW_NODE_HPP
#define WHILE_EXPRESSION_TRUE_LOOP_FLOW_NODE_HPP

#include "../core_types/flow_node.hpp"


class WhileExpressionTrueLoopFlowNode final : public FlowNode
{
	GDCLASS(WhileExpressionTrueLoopFlowNode, FlowNode);

private:
	String expression;
	FlowNodeID loop_flow_node_id = FLOW_NODE_ID_NIL;
	FlowNodeID finished_flow_node_id = FLOW_NODE_ID_NIL;

	void do_loop_iteration(FlowNodeExecutionState *p_state);

protected:
	static void _bind_methods();

	virtual void _execute(FlowNodeExecutionState *p_state) override;
	virtual bool _is_property_flow_node_reference(const StringName &p_name) const override;
	virtual void _on_external_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to) override;

public:
	void set_expression(const String &p_expression);
	String get_expression() const;
	void set_loop_flow_node_id(const FlowNodeID p_flow_node_id);
	FlowNodeID get_loop_flow_node_id() const;
	void set_finished_flow_node_id(const FlowNodeID p_flow_node_id);
	FlowNodeID get_finished_flow_node_id() const;
};


#endif
