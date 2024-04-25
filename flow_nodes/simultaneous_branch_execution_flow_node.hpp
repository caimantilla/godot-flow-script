#ifndef SIMULTANEOUS_BRANCH_EXECUTION_FLOW_NODE_HPP
#define SIMULTANEOUS_BRANCH_EXECUTION_FLOW_NODE_HPP

#include "multi_branch_execution_flow_node.hpp"


class SimultaneousBranchExecutionFlowNode final : public MultiBranchExecutionFlowNode
{
	GDCLASS(SimultaneousBranchExecutionFlowNode, MultiBranchExecutionFlowNode);

private:
	void on_execution_state_resumed(FlowNodeExecutionState *p_state);

protected:
	virtual void _execute(FlowNodeExecutionState *p_state) override;
};


#endif
