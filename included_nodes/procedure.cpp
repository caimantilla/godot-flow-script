#include "procedure.hpp"


void FlowScriptNodeProcedure::exec_step(FlowScriptNodeContext *p_context)
{
	p_context->finish(p_context->get_flow_script_ptr()->get_node_connection(p_context->get_node_id(), 0, 0));
}


String FlowScriptNodeProcedure::get_editor() const
{
	return "FlowScriptNodeEditorProcedure";
}


bool FlowScriptNodeProcedure::can_name_node() const
{
	return true;
}


FlowScriptNodeEditorProcedure::FlowScriptNodeEditorProcedure()
{
}
