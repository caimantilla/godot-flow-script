#ifndef FLOW_SCRIPT_NODE_MULTI_BRANCH_EXECUTE_SEQUENTIAL_HPP
#define FLOW_SCRIPT_NODE_MULTI_BRANCH_EXECUTE_SEQUENTIAL_HPP


#include "flow_script_node_multi_branch_execute.hpp"


class FlowScriptNodeMultiBranchExecuteSequential final : public FlowScriptNodeMultiBranchExecute
{
	GDCLASS(FlowScriptNodeMultiBranchExecuteSequential, FlowScriptNodeMultiBranchExecute);

public:
	enum TemporaryVariable
	{
		VARIABLE_CURRENT_BRANCH = 0,
		VARIABLE_SAVE_RESTORE = 1,
	};

protected:
	static void _bind_methods();

public:
	virtual void exec_startup(FlowScriptNodeContext *p_context) override;
	virtual void exec_step(FlowScriptNodeContext *p_context) override;
	virtual void set_runtime_state(FlowScriptNodeContext *p_context, const Dictionary &p_state) override;
	virtual Dictionary get_runtime_state(const FlowScriptNodeContext *p_context) const override;
};


VARIANT_ENUM_CAST(FlowScriptNodeMultiBranchExecuteSequential::TemporaryVariable);


#endif // FLOW_SCRIPT_NODE_MULTI_BRANCH_EXECUTE_SEQUENTIAL_HPP
