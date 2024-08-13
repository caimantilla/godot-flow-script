#ifndef FLOW_SCRIPT_NODE_EDITOR_PROCEDURE_HPP
#define FLOW_SCRIPT_NODE_EDITOR_PROCEDURE_HPP


#include "flow_script_node_editor.hpp"


class FlowScriptNodeEditorProcedure : public FlowScriptNodeEditor
{
	GDCLASS(FlowScriptNodeEditorProcedure, FlowScriptNodeEditor);

public:
	virtual void startup() override;
	virtual int get_input_slot() const override;
	virtual FlowScriptNodeOutputConnection output_graph_slot_to_connection(const int p_graph_slot) const override;
	virtual int output_connection_to_graph_slot(const FlowScriptNodeOutputConnection &p_connection) const override;

	FlowScriptNodeEditorProcedure();
};


#endif // FLOW_SCRIPT_NODE_EDITOR_PROCEDURE_HPP
