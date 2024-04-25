#ifndef EXECUTE_EXPRESSION_FLOW_NODE_EDITOR_HPP
#define EXECUTE_EXPRESSION_FLOW_NODE_EDITOR_HPP

#include "../execute_expression_flow_node.hpp"
#include "../../core_types/flow_node_editor.hpp"


class ExecuteExpressionFlowNodeEditor final : public FlowNodeEditor
{
	GDCLASS(ExecuteExpressionFlowNodeEditor, FlowNodeEditor);

private:
	Label *expression_label;

	ExecuteExpressionFlowNode *get_ee_flow_node() const;

protected:
	virtual void _flow_node_updated() override;
	virtual void _clean_up() override;
	virtual int _get_input_slot() const override;
	virtual TypedArray<FlowNodeEditorOutGoingConnectionParameters> _get_out_going_connections() const override;
	virtual void _set_out_going_connection(const Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection) override;

public:
	ExecuteExpressionFlowNodeEditor();
};


#endif
