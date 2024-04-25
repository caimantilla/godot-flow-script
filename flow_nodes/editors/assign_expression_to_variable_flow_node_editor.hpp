#ifndef ASSIGN_EXPRESSION_TO_VARIABLE_FLOW_NODE_EDITOR_HPP
#define ASSIGN_EXPRESSION_TO_VARIABLE_FLOW_NODE_EDITOR_HPP

#include "../assign_expression_to_variable_flow_node.hpp"
#include "../../core_types/flow_node_editor.hpp"


class AssignExpressionToVariableFlowNodeEditor final : public FlowNodeEditor
{
	GDCLASS(AssignExpressionToVariableFlowNodeEditor, FlowNodeEditor);

private:
	Label *variable_name_label;
	Label *assignment_label;
	Label *expression_label;

	AssignExpressionToVariableFlowNode *get_aetv_flow_node() const;

protected:
	virtual int _get_input_slot() const override;
	virtual void _set_out_going_connection(const Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection) override;
	virtual TypedArray<FlowNodeEditorOutGoingConnectionParameters> _get_out_going_connections() const override;
	virtual void _flow_node_updated() override;
	virtual void _clean_up() override;

public:
	AssignExpressionToVariableFlowNodeEditor();
};


#endif
