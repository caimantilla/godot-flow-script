#include "flow_script_node.hpp"


void FlowScriptNode::_bind_methods()
{
	ADD_SIGNAL(MethodInfo("output_connection_data_update_requested"));
}


void FlowScriptNode::request_output_connection_data_update()
{
	emit_signal(SNAME("output_connection_data_update_requested"));
}
