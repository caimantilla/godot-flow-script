#ifndef ASSIGN_EXPRESSION_TO_GLOBAL_VARIABLE_FLOW_NODE_HPP
#define ASSIGN_EXPRESSION_TO_GLOBAL_VARIABLE_FLOW_NODE_HPP

#include "assign_expression_to_variable_flow_node.hpp"


class AssignExpressionToGlobalVariableFlowNode final : public AssignExpressionToVariableFlowNode
{
	GDCLASS(AssignExpressionToGlobalVariableFlowNode, AssignExpressionToVariableFlowNode);

protected:
	virtual void _execute(FlowNodeExecutionState *p_state) override;
};


#endif
