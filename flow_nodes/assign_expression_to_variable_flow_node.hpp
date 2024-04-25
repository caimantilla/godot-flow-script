#ifndef ASSIGN_EXPRESSION_TO_VARIABLE_FLOW_NODE_HPP
#define ASSIGN_EXPRESSION_TO_VARIABLE_FLOW_NODE_HPP

#include "../core_types/flow_node.hpp"


class AssignExpressionToVariableFlowNode : public FlowNode
{
	GDCLASS(AssignExpressionToVariableFlowNode, FlowNode);

private:
	FlowNodeID next_flow_node_id = FLOW_NODE_ID_NIL;
	String variable_name;
	String expression;

protected:
	static void _bind_methods();

	Variant evaluate_expression(FlowNodeExecutionState *p_state);

	virtual void _on_external_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to) override;
	virtual bool _is_property_flow_node_reference(const StringName &p_name) const override;

public:
	void set_next_flow_node_id(const FlowNodeID p_id);
	FlowNodeID get_next_flow_node_id() const;
	void set_variable_name(const String &p_variable_name);
	StringName get_variable_name() const;
	void set_expression(const String &p_expression);
	String get_expression() const;
};


#endif
