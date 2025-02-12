#include "flow_script_node_multi_branch_execute_sequential.hpp"


void FlowScriptNodeMultiBranchExecuteSequential::_bind_methods()
{
	BIND_ENUM_CONSTANT(VARIABLE_CURRENT_BRANCH);
	BIND_ENUM_CONSTANT(VARIABLE_SAVE_RESTORE);
}


void FlowScriptNodeMultiBranchExecuteSequential::exec_startup(FlowScriptNodeContext *p_context)
{
	p_context->set_variable(VARIABLE_CURRENT_BRANCH, -1);
	p_context->set_variable(VARIABLE_SAVE_RESTORE, false);
}


void FlowScriptNodeMultiBranchExecuteSequential::exec_step(FlowScriptNodeContext *p_context)
{
	if (p_context->get_variable(VARIABLE_SAVE_RESTORE))
	{
		p_context->set_variable(VARIABLE_SAVE_RESTORE, false);
		return;
	}
	int64_t current_branch_idx = p_context->get_variable(VARIABLE_CURRENT_BRANCH);
	current_branch_idx++;
	p_context->set_variable(VARIABLE_CURRENT_BRANCH, current_branch_idx);

	if (current_branch_idx < get_connection_count())
	{
		p_context->add_await_branch(FlowScriptNodeOutputConnection(CONNECTION_LIST_BRANCHES, current_branch_idx));
		p_context->execute_await_branches();
	}
	else
	{
		p_context->advance(FlowScriptNodeOutputConnection(CONNECTION_LIST_ADVANCE, 0));
	}
}


void FlowScriptNodeMultiBranchExecuteSequential::set_runtime_state(FlowScriptNodeContext *p_context, const Dictionary &p_state)
{
	p_context->set_variable(VARIABLE_SAVE_RESTORE, true);
	p_context->set_variable(VARIABLE_CURRENT_BRANCH, p_state.get("current_branch_index", -1));
}


Dictionary FlowScriptNodeMultiBranchExecuteSequential::get_runtime_state(const FlowScriptNodeContext *p_context) const
{
	Dictionary d;
	d["current_branch_index"] = p_context->get_variable(VARIABLE_CURRENT_BRANCH);
	return d;
}
