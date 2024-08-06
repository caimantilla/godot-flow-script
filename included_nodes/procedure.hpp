#ifndef FLOW_SCRIPT_NODE_PROCEDURE_HPP
#define FLOW_SCRIPT_NODE_PROCEDURE_HPP


#include "common.hpp"


class FlowScriptNodeProcedure : public FlowScriptNode
{
	GDCLASS(FlowScriptNodeProcedure, FlowScriptNode);

public:
	virtual void exec_step(FlowScriptNodeContext *p_context) override;
	virtual String get_editor() const override;
	virtual bool can_name_node() const override;
};


class FlowScriptNodeEditorProcedure : public FlowScriptNodeEditor
{
	GDCLASS(FlowScriptNodeEditorProcedure, FlowScriptNodeEditor);

public:
	FlowScriptNodeEditorProcedure();
};


#endif // FLOW_SCRIPT_NODE_PROCEDURE_HPP
