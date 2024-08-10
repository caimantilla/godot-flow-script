#include "flow_script_node_multi_branch_execute_sequential.hpp"


void FlowScriptNodeMultiBranchExecuteSequential::_bind_methods()
{
	BIND_ENUM_CONSTANT(VARIABLE_CURRENT_BRANCH);
}


void FlowScriptNodeMultiBranchExecuteSequential::exec_startup(FlowScriptNodeContext *p_context)
{
	p_context->set_variable(VARIABLE_CURRENT_BRANCH, -1);
}


void FlowScriptNodeMultiBranchExecuteSequential::exec_step(FlowScriptNodeContext *p_context)
{
	int64_t current_branch_idx = p_context->get_variable(VARIABLE_CURRENT_BRANCH);
	current_branch_idx++;
	p_context->set_variable(VARIABLE_CURRENT_BRANCH, current_branch_idx);

	if (current_branch_idx < p_context->get_flow_script_ptr()->get_node_connection_list_length(p_context->get_current_node_id(), CONNECTION_LIST_BRANCHES))
	{
		bool await = p_context->await_branch_solo(p_context->get_flow_script_ptr()->get_node_connection(p_context->get_current_node_id(), CONNECTION_LIST_BRANCHES, current_branch_idx));
		if (!await)
		{
			p_context->invoke_step();
		}
	}
	else
	{
		p_context->advance_to_node(p_context->get_flow_script_ptr()->get_node_connection(p_context->get_current_node_id(), CONNECTION_LIST_PROCEED, 0));
	}
}


void FlowScriptNodeMultiBranchExecuteSequential::set_state(FlowScriptNodeContext *p_context, const Dictionary &p_state)
{
	if (p_state.has("current_branch_index"))
	{
		p_context->set_variable(VARIABLE_CURRENT_BRANCH, p_state["current_branch_index"]);
	}
}


void FlowScriptNodeMultiBranchExecuteSequential::get_state(const FlowScriptNodeContext *p_context, Dictionary &r_state) const
{
	r_state["current_branch_index"] = p_context->get_variable(VARIABLE_CURRENT_BRANCH);
}
