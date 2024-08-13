#ifndef FLOW_SCRIPT_NODE_EDITOR_SET_EXPRESSION_RESULT_TO_VARIABLE_HPP
#define FLOW_SCRIPT_NODE_EDITOR_SET_EXPRESSION_RESULT_TO_VARIABLE_HPP


#include "flow_script_node_editor.hpp"


class FlowScriptNodeEditorSetExpressionResultToVariable : public FlowScriptNodeEditor
{
	GDCLASS(FlowScriptNodeEditorSetExpressionResultToVariable, FlowScriptNodeEditor);

private:
	Label *lbl_variable;
	Label *lbl_expression;
	Label *lbl_whitespace_warning;

public:
	virtual void sync() override;
	virtual FlowScriptNodeOutputConnection output_graph_slot_to_connection(const int p_graph_slot) const override;
	virtual int output_connection_to_graph_slot(const FlowScriptNodeOutputConnection &p_connection) const override;
	virtual int get_input_slot() const override;

	FlowScriptNodeEditorSetExpressionResultToVariable();
};


#endif // FLOW_SCRIPT_NODE_EDITOR_SET_EXPRESSION_RESULT_TO_VARIABLE_HPP
