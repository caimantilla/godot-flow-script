#ifndef FLOW_SCRIPT_NODE_EDITOR_WAIT_DURATION_EXPRESSION_RESULT_HPP
#define FLOW_SCRIPT_NODE_EDITOR_WAIT_DURATION_EXPRESSION_RESULT_HPP


#include "flow_script_node_editor.hpp"
#include "scene/gui/label.h"
#include "scene/gui/rich_text_label.h"


class FlowScriptNodeEditorWaitDurationExpressionResult : public FlowScriptNodeEditor
{
	GDCLASS(FlowScriptNodeEditorWaitDurationExpressionResult, FlowScriptNodeEditor);

private:
	Label *prefix_label;
	Label *no_expression_defined_label;
	RichTextLabel *expression_label;

public:
	virtual void sync() override;
	virtual FlowScriptNodeOutputConnection output_graph_slot_to_connection(const int p_graph_slot) const override;
	virtual int output_connection_to_graph_slot(const FlowScriptNodeOutputConnection &p_connection) const override;
	virtual int get_input_slot() const override;

	FlowScriptNodeEditorWaitDurationExpressionResult();
};


#endif // FLOW_SCRIPT_NODE_EDITOR_WAIT_DURATION_EXPRESSION_RESULT_HPP
