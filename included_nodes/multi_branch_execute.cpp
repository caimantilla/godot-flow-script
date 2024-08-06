#include "multi_branch_execute.hpp"


void FlowScriptNodeMultiBranchExecuteBase::_bind_methods()
{
	BIND_CONSTANT(MAX_CONNECTIONS);

	BIND_ENUM_CONSTANT(CONNECTION_LIST_PROCEED);
	BIND_ENUM_CONSTANT(CONNECTION_LIST_BRANCHES);

	ClassDB::bind_method(D_METHOD("set_connection_count", "count"), &FlowScriptNodeMultiBranchExecuteBase::set_connection_count);
	ClassDB::bind_method(D_METHOD("get_connection_count"), &FlowScriptNodeMultiBranchExecuteBase::get_connection_count);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "connection_count", PROPERTY_HINT_RANGE, "0," + itos(MAX_CONNECTIONS) + ",1"), "set_connection_count", "get_connection_count");
}


void FlowScriptNodeMultiBranchExecuteBase::set_connection_count(const int64_t p_count)
{
	if (connection_count == p_count)
		return;
	connection_count = p_count;
	emit_changed();
}


int64_t FlowScriptNodeMultiBranchExecuteBase::get_connection_count() const
{
	return connection_count;
}


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
