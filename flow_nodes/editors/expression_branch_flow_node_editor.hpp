#ifndef EXPRESSION_BRANCH_FLOW_NODE_EDITOR_HPP
#define EXPRESSION_BRANCH_FLOW_NODE_EDITOR_HPP


#include "../../core_types/flow_node_editor.hpp"
#include "../expression_branch_flow_node.hpp"
#include "scene/gui/label.h"


class ExpressionBranchFlowNodeEditor final : public FlowNodeEditor
{
	GDCLASS(ExpressionBranchFlowNodeEditor, FlowNodeEditor);

private:
	Label *expression_label;

	ExpressionBranchFlowNode *get_eb_flow_node() const;

protected:
	virtual void _clean_up() override;
	virtual void _flow_node_updated() override;
	virtual void _set_out_going_connection(const Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection) override;
	virtual int _get_input_slot() const override;
	virtual TypedArray<FlowNodeEditorOutGoingConnectionParameters> _get_out_going_connections() const override;

public:
	ExpressionBranchFlowNodeEditor();
};


#endif
