#ifndef FLOW_SCRIPT_NODE_EDITOR_RETURN_EXPRESSION_RESULT_HPP
#define FLOW_SCRIPT_NODE_EDITOR_RETURN_EXPRESSION_RESULT_HPP


#include "flow_script_node_editor.hpp"
#include "scene/gui/label.h"
#include "editor/gui/flow_script_editor_expression_display_box.hpp"


class FlowScriptNodeEditorReturnExpressionResult final : public FlowScriptNodeEditor
{
	GDCLASS(FlowScriptNodeEditorReturnExpressionResult, FlowScriptNodeEditor);

private:
	FlowScriptEditorExpressionDisplayBox *expression_box;

public:
	virtual void sync() override;
	virtual int get_input_slot() const override;

	FlowScriptNodeEditorReturnExpressionResult();
};


#endif // FLOW_SCRIPT_NODE_EDITOR_RETURN_EXPRESSION_RESULT_HPP
