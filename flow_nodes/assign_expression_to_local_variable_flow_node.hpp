#ifndef ASSIGN_EXPRESSION_TO_LOCAL_VARIABLE_FLOW_NODE_HPP
#define ASSIGN_EXPRESSION_TO_LOCAL_VARIABLE_FLOW_NODE_HPP

#include "assign_expression_to_variable_flow_node.hpp"


class AssignExpressionToLocalVariableFlowNode final : public AssignExpressionToVariableFlowNode
{
	GDCLASS(AssignExpressionToLocalVariableFlowNode, AssignExpressionToVariableFlowNode);

protected:
	virtual void _execute(FlowNodeExecutionState *p_state) override;
};


#endif
