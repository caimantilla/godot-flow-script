#ifndef EXECUTE_EXTERNAL_FLOW_SCRIPT_FLOW_NODE_EDITOR_HPP
#define EXECUTE_EXTERNAL_FLOW_SCRIPT_FLOW_NODE_EDITOR_HPP

#include "../../core_types/flow_node_editor.hpp"
#include "../execute_external_flow_script_flow_node.hpp"


class ExecuteExternalFlowScriptFlowNodeEditor : public FlowNodeEditor
{
	GDCLASS(ExecuteExternalFlowScriptFlowNodeEditor, FlowNodeEditor);

private:
	Label *desc_label;

	ExecuteExternalFlowScriptFlowNode *get_eefs_node() const;

protected:
	virtual void _set_out_going_connection(Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection) override;
	virtual int _get_input_slot() const override;
	virtual TypedArray<FlowNodeEditorOutGoingConnectionParameters> _get_out_going_connections() const override;
	virtual void _flow_node_updated() override;

public:
	ExecuteExternalFlowScriptFlowNodeEditor();
};


#endif
