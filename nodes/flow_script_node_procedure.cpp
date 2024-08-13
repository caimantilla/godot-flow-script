#include "flow_script_node_procedure.hpp"
#include "scene/gui/label.h"


void FlowScriptNodeProcedure::exec_step(FlowScriptNodeContext *p_context)
{
	p_context->advance(FlowScriptNodeOutputConnection(0, 0));
}


bool FlowScriptNodeProcedure::can_name_node() const
{
	return true;
}
