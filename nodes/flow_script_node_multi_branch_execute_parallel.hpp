#ifndef FLOW_SCRIPT_NODE_MULTI_BRANCH_EXECUTE_PARALLEL_HPP
#define FLOW_SCRIPT_NODE_MULTI_BRANCH_EXECUTE_PARALLEL_HPP


#include "flow_script_node_multi_branch_execute.hpp"


class FlowScriptNodeMultiBranchExecuteParallel final : public FlowScriptNodeMultiBranchExecute
{
	GDCLASS(FlowScriptNodeMultiBranchExecuteParallel, FlowScriptNodeMultiBranchExecute);

public:
	enum TemporaryVariable
	{
		VARIABLE_AWAIT_STATE = 0,
	};

	enum AwaitState
	{
		AWAIT_STATE_INIT = 0,
		AWAIT_STATE_EXECUTING = 1,
		AWAIT_STATE_RESTORE_SAVE = 2,
	};

protected:
	static void _bind_methods();

public:
	virtual void exec_startup(FlowScriptNodeContext *p_context) override;
	virtual void exec_step(FlowScriptNodeContext *p_context) override;
	virtual void set_runtime_state(FlowScriptNodeContext *p_context, const Dictionary &p_state) override;
	virtual Dictionary get_runtime_state(const FlowScriptNodeContext *p_context) const override;
};


VARIANT_ENUM_CAST(FlowScriptNodeMultiBranchExecuteParallel::TemporaryVariable);
VARIANT_ENUM_CAST(FlowScriptNodeMultiBranchExecuteParallel::AwaitState);


#endif // FLOW_SCRIPT_NODE_MULTI_BRANCH_EXECUTE_PARALLEL_HPP
