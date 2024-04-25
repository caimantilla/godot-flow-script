#ifndef RETURN_EXPRESSION_FLOW_NODE_EDITOR_HPP
#define RETURN_EXPRESSION_FLOW_NODE_EDITOR_HPP

#include "../return_expression_flow_node.hpp"
#include "../../core_types/flow_node_editor.hpp"


class ReturnExpressionFlowNodeEditor final : public FlowNodeEditor
{
	GDCLASS(ReturnExpressionFlowNodeEditor, FlowNodeEditor);

private:
	Label *expression_label;

	ReturnExpressionFlowNode *get_re_flow_node() const;

protected:
	virtual void _flow_node_updated() override;
	virtual void _clean_up() override;

	virtual int _get_input_slot() const override;

public:
	ReturnExpressionFlowNodeEditor();
};


#endif
