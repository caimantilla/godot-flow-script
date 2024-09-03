#include "flow_script_node_procedure.hpp"
#include "scene/gui/label.h"


void FlowScriptNodeProcedure::exec_step(FlowScriptNodeContext *p_context)
{
	p_context->advance(FlowScriptNodeOutputConnection(0, 0));
}


void FlowScriptNodeProcedure::get_output_connection_list_lengths(List<int64_t> &r_lengths) const
{
	r_lengths.push_back(1);
}
