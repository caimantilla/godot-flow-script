#include "flow_script_node.hpp"


void FlowScriptNode::exec_startup(FlowScriptNodeContext *p_context)
{
}


void FlowScriptNode::exec_cleanup(FlowScriptNodeContext *p_context)
{
}


void FlowScriptNode::exec_step(FlowScriptNodeContext *p_context)
{
}


void FlowScriptNode::set_runtime_state(FlowScriptNodeContext *p_context, const Dictionary &p_state)
{
}


Dictionary FlowScriptNode::get_runtime_state(const FlowScriptNodeContext *p_context) const
{
	return Dictionary();
}


void FlowScriptNode::set_data_state(const Dictionary &p_data)
{
}


Dictionary FlowScriptNode::get_data_state() const
{
	return Dictionary();
}


void FlowScriptNode::get_output_connection_list_lengths(List<FlowScriptNodeConnectionListLength> *p_lengths) const
{
}
