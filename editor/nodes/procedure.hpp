#ifndef FLOW_SCRIPT_NODE_EDITOR_PROCEDURE_HPP
#define FLOW_SCRIPT_NODE_EDITOR_PROCEDURE_HPP


#include "flow_script_node_editor.hpp"


class FlowScriptNodeEditorProcedure : public FlowScriptNodeEditor
{
	GDCLASS(FlowScriptNodeEditorProcedure, FlowScriptNodeEditor);

public:
	virtual void startup() override;
	virtual int get_input_slot() const override;
	virtual void set_outgoing_connection(const FlowScriptNodeEditorOutgoingConnectionParameters &p_connection) override;
	virtual void get_outgoing_connections(List<FlowScriptNodeEditorOutgoingConnectionParameters> *p_list) const override;

	FlowScriptNodeEditorProcedure();
};


#endif // FLOW_SCRIPT_NODE_EDITOR_PROCEDURE_HPP
