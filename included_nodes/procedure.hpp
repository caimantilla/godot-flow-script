#ifndef FLOW_SCRIPT_NODE_PROCEDURE_HPP
#define FLOW_SCRIPT_NODE_PROCEDURE_HPP


#include "common.hpp"


class FlowScriptNodeProcedure final : public FlowScriptNode
{
	GDCLASS(FlowScriptNodeProcedure, FlowScriptNode);

public:
	virtual String get_type_id() const override { return "procedure"; }
	virtual String get_type_name() const override { return "Procedure"; }
	virtual String get_type_description() const override { return "A named entrypoint into the script."; }
	virtual String get_type_editor() const override { return "FlowScriptNodeEditorProcedure"; }
	virtual void exec_step(FlowScriptNodeContext *p_context) override;
	virtual bool can_name_node() const override;
};


class FlowScriptNodeEditorProcedure : public FlowScriptNodeEditor
{
	GDCLASS(FlowScriptNodeEditorProcedure, FlowScriptNodeEditor);

public:
	FlowScriptNodeEditorProcedure();
};


#endif // FLOW_SCRIPT_NODE_PROCEDURE_HPP
