#ifndef SEQUENTIAL_BRANCH_EXECUTION_FLOW_NODE_EDITOR_HPP
#define SEQUENTIAL_BRANCH_EXECUTION_FLOW_NODE_EDITOR_HPP

#include "multi_branch_execution_flow_node_editor.hpp"


class SequentialBranchExecutionFlowNodeEditor final : public MultiBranchExecutionFlowNodeEditor
{
	GDCLASS(SequentialBranchExecutionFlowNodeEditor, MultiBranchExecutionFlowNodeEditor);

protected:
	virtual String _get_main_label_text() const override;
};


#endif // SEQUENTIAL_BRANCH_EXECUTION_FLOW_NODE_EDITOR_HPP