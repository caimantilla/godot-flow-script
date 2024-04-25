#ifndef SEQUENTIAL_BRANCH_EXECUTION_FLOW_NODE_HPP
#define SEQUENTIAL_BRANCH_EXECUTION_FLOW_NODE_HPP

#include "multi_branch_execution_flow_node.hpp"


class SequentialBranchExecutionFlowNode final : public MultiBranchExecutionFlowNode
{
	GDCLASS(SequentialBranchExecutionFlowNode, MultiBranchExecutionFlowNode);

private:
	void do_next_step(FlowNodeExecutionState *p_state);

protected:
	virtual void _execute(FlowNodeExecutionState *p_state) override;
};


#endif
