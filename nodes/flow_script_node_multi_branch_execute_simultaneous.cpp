#include "flow_script_node_multi_branch_execute_simultaneous.hpp"


void FlowScriptNodeMultiBranchExecuteSimultaneous::_bind_methods()
{
	BIND_ENUM_CONSTANT(VARIABLE_STEP_COUNT);
}


void FlowScriptNodeMultiBranchExecuteSimultaneous::exec_startup(FlowScriptNodeContext *p_context)
{
	p_context->set_variable(VARIABLE_STEP_COUNT, -1);
}


void FlowScriptNodeMultiBranchExecuteSimultaneous::exec_step(FlowScriptNodeContext *p_context)
{
	int64_t step_count = p_context->get_variable(VARIABLE_STEP_COUNT);
	step_count++;
	p_context->set_variable(VARIABLE_STEP_COUNT, step_count);

	if (step_count != 0)
	{
		p_context->advance_to_node(p_context->get_flow_script_ptr()->get_node_connection(p_context->get_current_node_id(), CONNECTION_LIST_PROCEED, 0));
		return;
	}

	List<FlowScriptNodeID> node_ids;
	for (int64_t i = 0; i < p_context->get_flow_script_ptr()->get_node_connection_list_length(p_context->get_current_node_id(), CONNECTION_LIST_BRANCHES))
	{
		FlowScriptNodeID curr_id = p_context->get_flow_script_ptr()->get_node_connection(p_context->get_current_node_id(), CONNECTION_LIST_BRANCHES, i);
		if (!p_context->get_flow_script_ptr()->has_node(curr_id))
			continue;
		node_ids.push_back(curr_id);
	}
	if (node_ids.is_empty() || !p_context->await_branch_list(node_ids))
	{
		p_context->invoke_step();
	}
}


void FlowScriptNodeMultiBranchExecuteSimultaneous::set_state(FlowScriptNodeContext *p_context, const Dictionary &p_state)
{
	if (p_state.has("step_count"))
	{
		int64_t step_count = p_state["step_count"];
		p_context->set_variable(VARIABLE_STEP_COUNT, step_count);
	}
}


void FlowScriptNodeMultiBranchExecuteSimultaneous::get_state(const FlowScriptNodeContext *p_context, Dictionary &r_state) const
{
	r_state["step_count"] = p_context->get_variable(VARIABLE_STEP_COUNT);
}
