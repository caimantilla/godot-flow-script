#include "sequential_branch_execution_flow_node.hpp"


void SequentialBranchExecutionFlowNode::do_next_step(FlowNodeExecutionState *p_state)
{
	int current_step = p_state->get_temporary_variable(SNAME("current_step"));
	current_step += 1;
	p_state->set_temporary_variable(SNAME("current_step"), current_step);

	if (current_step >= get_execution_stack_size())
	{
		p_state->finish(get_next_flow_node_id(), Variant());
	}
	else
	{
		FlowNodeID next_step_flow_node_id = get_execution_stack_entry_at(current_step);
		p_state->request_new_fiber(next_step_flow_node_id, true);
	}
}


void SequentialBranchExecutionFlowNode::_execute(FlowNodeExecutionState *p_state)
{
	p_state->set_temporary_variable(SNAME("current_step"), -1);

	Callable do_next_step_callback = callable_mp(this, &SequentialBranchExecutionFlowNode::do_next_step).bind(p_state);
	p_state->connect(SNAME("resumed"), do_next_step_callback);

	do_next_step_callback.call();
}
