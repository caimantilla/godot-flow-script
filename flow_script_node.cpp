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


bool FlowScriptNode::can_translate_text() const
{
	return false;
}


void FlowScriptNode::init_text_translation(const FlowScriptNodeID p_node_id, FlowScriptNodeTranslation *p_translation)
{
}


void FlowScriptNode::set_state(FlowScriptNodeContext *p_context, const Dictionary &p_state)
{
}


void FlowScriptNode::get_state(const FlowScriptNodeContext *p_context, Dictionary &r_state) const
{
}


void FlowScriptNode::set_json_data(const Dictionary &p_data)
{
}


void FlowScriptNode::get_json_data(Dictionary &r_data) const
{
}


void FlowScriptNode::get_output_connection_list_lengths(List<int64_t> &r_lengths) const
{
}
