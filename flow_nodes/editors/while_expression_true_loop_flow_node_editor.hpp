#ifndef WHILE_EXPRESSION_TRUE_LOOP_FLOW_NODE_EDITOR_HPP
#define WHILE_EXPRESSION_TRUE_LOOP_FLOW_NODE_EDITOR_HPP

#include "../while_expression_true_loop_flow_node.hpp"
#include "../../core_types/flow_node_editor.hpp"


class WhileExpressionTrueLoopFlowNodeEditor final : public FlowNodeEditor
{
	GDCLASS(WhileExpressionTrueLoopFlowNodeEditor, FlowNodeEditor);

private:
	Label *expression_label;

	WhileExpressionTrueLoopFlowNode *get_wetl_flow_node() const;

protected:
	virtual int _get_input_slot() const override;
	virtual void _set_out_going_connection(const Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection) override;
	virtual TypedArray<FlowNodeEditorOutGoingConnectionParameters> _get_out_going_connections() const override;

	virtual void _flow_node_updated() override;
	virtual void _clean_up() override;

public:
	WhileExpressionTrueLoopFlowNodeEditor();
};


#endif
