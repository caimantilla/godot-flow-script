#ifndef SIMULTANEOUS_BRANCH_EXECUTION_FLOW_NODE_EDITOR_HPP
#define SIMULTANEOUS_BRANCH_EXECUTION_FLOW_NODE_EDITOR_HPP

#include "multi_branch_execution_flow_node_editor.hpp"


class SimultaneousBranchExecutionFlowNodeEditor final : public MultiBranchExecutionFlowNodeEditor
{
	GDCLASS(SimultaneousBranchExecutionFlowNodeEditor, MultiBranchExecutionFlowNodeEditor);

protected:
	virtual String _get_main_label_text() const override;
};


#endif // SIMULTANEOUS_BRANCH_EXECUTION_FLOW_NODE_EDITOR_HPP
