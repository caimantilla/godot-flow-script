#ifndef FLOW_SCRIPT_NODE_EDITOR_BOOLEAN_BRANCH_EXPRESSION_HPP
#define FLOW_SCRIPT_NODE_EDITOR_BOOLEAN_BRANCH_EXPRESSION_HPP


#include "flow_script_node_editor.hpp"
#include "scene/gui/label.h"
#include "editor/gui/flow_script_editor_expression_display_box.hpp"


class FlowScriptNodeEditorBooleanBranchExpression final : public FlowScriptNodeEditor
{
	GDCLASS(FlowScriptNodeEditorBooleanBranchExpression, FlowScriptNodeEditor);

private:
	Label *lbl_true;
	Label *lbl_false;
	FlowScriptEditorExpressionDisplayBox *expression_box;

	Label *create_result_label(const String &p_text, const Color &p_color);

public:
	virtual void sync() override;
	virtual int get_input_slot() const override;
	virtual FlowScriptNodeOutputConnection output_graph_slot_to_connection(const int p_graph_slot) const override;
	virtual int output_connection_to_graph_slot(const FlowScriptNodeOutputConnection &p_connection) const override;

	FlowScriptNodeEditorBooleanBranchExpression();
};


#endif // FLOW_SCRIPT_NODE_EDITOR_BOOLEAN_BRANCH_EXPRESSION_HPP
