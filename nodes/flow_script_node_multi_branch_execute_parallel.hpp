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
	virtual String get_type_id() const override { return "multi_branch_execute_parallel"; }
	virtual String get_type_name() const override { return "Execute Parallel Branches"; }
	virtual String get_type_description() const override { return "Triggers execution of a list of branches all at once, then advances once all the branches have finished."; }
	virtual void exec_startup(FlowScriptNodeContext *p_context) override;
	virtual void exec_step(FlowScriptNodeContext *p_context) override;
	virtual void set_state(FlowScriptNodeContext *p_context, const Dictionary &p_state) override;
	virtual void get_state(const FlowScriptNodeContext *p_context, Dictionary &r_state) const override;
};


VARIANT_ENUM_CAST(FlowScriptNodeMultiBranchExecuteParallel::TemporaryVariable);
VARIANT_ENUM_CAST(FlowScriptNodeMultiBranchExecuteParallel::AwaitState);


#endif // FLOW_SCRIPT_NODE_MULTI_BRANCH_EXECUTE_PARALLEL_HPP
