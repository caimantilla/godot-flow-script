#ifndef RETURN_EXPRESSION_FLOW_NODE_HPP
#define RETURN_EXPRESSION_FLOW_NODE_HPP

#include "../core_types/flow_node.hpp"


class ReturnExpressionFlowNode final : public FlowNode
{
	GDCLASS(ReturnExpressionFlowNode, FlowNode);

private:
	String expression;

protected:
	static void _bind_methods();

	void _execute(FlowNodeExecutionState *p_state) override;

public:
	void set_expression(const String &p_expression);
	String get_expression() const;
};


#endif
