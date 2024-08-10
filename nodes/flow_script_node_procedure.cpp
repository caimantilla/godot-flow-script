#include "procedure.hpp"
#include "scene/gui/label.h"


void FlowScriptNodeProcedure::exec_step(FlowScriptNodeContext *p_context)
{
	p_context->advance_to_node(p_context->get_flow_script_ptr()->get_node_connection(p_context->get_current_node_id(), 0, 0));
}


bool FlowScriptNodeProcedure::can_name_node() const
{
	return true;
}
