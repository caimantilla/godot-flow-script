#include "sequential_branch_execution_flow_node_editor.hpp"


String SequentialBranchExecutionFlowNodeEditor::_get_main_label_text() const
{
	return "Execute each branch in order,\nawait completion before executing the next,\nthen continue.";
}
