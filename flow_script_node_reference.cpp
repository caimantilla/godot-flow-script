#include "flow_script_node_reference.hpp"
#include "flow_script.hpp"


FlowScriptNodeReference::FlowScriptNodeReference()
{
	flow_script_id = FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID;
	node_id = FlowScript::NODE_ID_INVALID;
}
