#include "simultaneous_branch_execution_flow_node.hpp"


void SimultaneousBranchExecutionFlowNode::on_execution_state_resumed(FlowNodeExecutionState *p_state)
{
	p_state->finish(get_next_flow_node_id(), Variant());
}


void SimultaneousBranchExecutionFlowNode::_execute(FlowNodeExecutionState *p_state)
{
	Callable resumed_callback = callable_mp(this, &SimultaneousBranchExecutionFlowNode::on_execution_state_resumed).bind(p_state);
	p_state->connect(SNAME("resumed"), resumed_callback, CONNECT_ONE_SHOT);
	
	p_state->request_multiple_new_fibers(get_execution_stack(), true);
}
