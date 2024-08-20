#ifndef FLOW_SCRIPT_NODE_EDITOR_LOOP_WHILE_EXPRESSION_RESULT_TRUE_HPP
#define FLOW_SCRIPT_NODE_EDITOR_LOOP_WHILE_EXPRESSION_RESULT_TRUE_HPP


#include "flow_script_node_editor.hpp"
#include "scene/gui/label.h"
#include "../gui/flow_script_editor_expression_display_box.hpp"


class FlowScriptNodeEditorLoopWhileExpressionResultTrue final : public FlowScriptNodeEditor
{
	GDCLASS(FlowScriptNodeEditorLoopWhileExpressionResultTrue, FlowScriptNodeEditor);

private:
	Label *expression_undefined_label;
	FlowScriptEditorExpressionDisplayBox *expression_box;

public:
	virtual void sync() override;
	virtual FlowScriptNodeOutputConnection output_graph_slot_to_connection(const int p_graph_slot) const override;
	virtual int output_connection_to_graph_slot(const FlowScriptNodeOutputConnection &p_connection) const override;
	virtual int get_input_slot() const override;

	FlowScriptNodeEditorLoopWhileExpressionResultTrue();
};


#endif // FLOW_SCRIPT_NODE_EDITOR_LOOP_WHILE_EXPRESSION_RESULT_TRUE_HPP
