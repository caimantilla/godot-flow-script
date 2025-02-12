#include "flow_script_node_multi_branch_execute_parallel.hpp"


void FlowScriptNodeMultiBranchExecuteParallel::_bind_methods()
{
	BIND_ENUM_CONSTANT(VARIABLE_AWAIT_STATE);

	BIND_ENUM_CONSTANT(AWAIT_STATE_INIT);
	BIND_ENUM_CONSTANT(AWAIT_STATE_EXECUTING);
	BIND_ENUM_CONSTANT(AWAIT_STATE_RESTORE_SAVE);
}


void FlowScriptNodeMultiBranchExecuteParallel::exec_startup(FlowScriptNodeContext *p_context)
{
	p_context->set_variable(VARIABLE_AWAIT_STATE, AWAIT_STATE_INIT);
}


void FlowScriptNodeMultiBranchExecuteParallel::exec_step(FlowScriptNodeContext *p_context)
{
	AwaitState await_state = (AwaitState)(int64_t)p_context->get_variable(VARIABLE_AWAIT_STATE);
	switch (await_state)
	{
		case AWAIT_STATE_INIT:
			p_context->set_variable(VARIABLE_AWAIT_STATE, AWAIT_STATE_EXECUTING);
			for (int64_t i = 0; i < get_connection_count(); i++)
			{
				p_context->add_await_branch(FlowScriptNodeOutputConnection(CONNECTION_LIST_BRANCHES, i));
			}
			p_context->execute_await_branches();
			break;
		case AWAIT_STATE_EXECUTING:
			p_context->advance(FlowScriptNodeOutputConnection(CONNECTION_LIST_ADVANCE, 0));
			break;
		case AWAIT_STATE_RESTORE_SAVE:
			p_context->set_variable(VARIABLE_AWAIT_STATE, AWAIT_STATE_EXECUTING);
			break;
	}
}


void FlowScriptNodeMultiBranchExecuteParallel::set_runtime_state(FlowScriptNodeContext *p_context, const Dictionary &p_state)
{
	p_context->set_variable(VARIABLE_AWAIT_STATE, AWAIT_STATE_RESTORE_SAVE);
}


Dictionary FlowScriptNodeMultiBranchExecuteParallel::get_runtime_state(const FlowScriptNodeContext *p_context) const
{
	Dictionary d;
	return d;
}
