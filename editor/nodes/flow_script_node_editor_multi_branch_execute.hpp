#ifndef FLOW_SCRIPT_NODE_EDITOR_MULTI_BRANCH_EXECUTE_HPP
#define FLOW_SCRIPT_NODE_EDITOR_MULTI_BRANCH_EXECUTE_HPP


#include "flow_script_node_editor.hpp"
#include "../../nodes/flow_script_node_multi_branch_execute.hpp"
#include "scene/gui/label.h"
#include "scene/gui/separator.h"


class FlowScriptNodeEditorMultiBranchExecute final : public FlowScriptNodeEditor
{
	GDCLASS(FlowScriptNodeEditorMultiBranchExecute, FlowScriptNodeEditor);

private:
	struct OutputControlData
	{
		HSeparator *separator;
		Label *label;
	};

private:
	OutputControlData output_control_list[FlowScriptNodeMultiBranchExecute::MAX_CONNECTIONS];

	int output_control_get_graph_slot(const int p_idx) const;
	int graph_slot_get_output_control(const int p_graph_slot) const;
	void output_control_show(const int p_idx);
	void output_control_hide(const int p_idx);

public:
	virtual void sync() override;
	virtual FlowScriptNodeOutputConnection output_graph_slot_to_connection(const int p_graph_slot) const override;
	virtual int output_connection_to_graph_slot(const FlowScriptNodeOutputConnection &p_connection) const override;
	virtual int get_input_slot() const override;

	FlowScriptNodeEditorMultiBranchExecute();
};


#endif // FLOW_SCRIPT_NODE_EDITOR_MULTI_BRANCH_EXECUTE_HPP
