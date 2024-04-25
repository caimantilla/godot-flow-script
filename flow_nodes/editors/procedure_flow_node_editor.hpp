#ifndef PROCEDURE_FLOW_NODE_EDITOR_HPP
#define PROCEDURE_FLOW_NODE_EDITOR_HPP

#include "../../core_types/flow_node_editor.hpp"
#include "../procedure_flow_node.hpp"

class ProcedureFlowNodeEditor final : public FlowNodeEditor
{
	GDCLASS(ProcedureFlowNodeEditor, FlowNodeEditor);

private:
	ProcedureFlowNode *get_procedure_flow_node() const;

protected:
	virtual void _set_out_going_connection(const Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection) override;
	virtual TypedArray<FlowNodeEditorOutGoingConnectionParameters> _get_out_going_connections() const override;

public:
	ProcedureFlowNodeEditor();
};

#endif
