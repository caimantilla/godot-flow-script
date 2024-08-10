#ifndef FLOW_SCRIPT_NODE_EDITOR_BOOLEAN_BRANCH_EXPRESSION_HPP
#define FLOW_SCRIPT_NODE_EDITOR_BOOLEAN_BRANCH_EXPRESSION_HPP


#include "flow_script_node_editor.hpp"
#include "scene/gui/label.h"
#include "scene/gui/rich_text_label.h"


class FlowScriptNodeEditorBooleanBranchExpression final : public FlowScriptNodeEditor
{
	GDCLASS(FlowScriptNodeEditorBooleanBranchExpression, FlowScriptNodeEditor);

private:
	Label *lbl_true;
	Label *lbl_false;
	RichTextLabel *lbl_condition;

	Label *create_result_label(const String &p_text, const Color &p_color);

public:
	virtual void sync() override;

	FlowScriptNodeEditorBooleanBranchExpression();
};


#endif // FLOW_SCRIPT_NODE_EDITOR_BOOLEAN_BRANCH_EXPRESSION_HPP
