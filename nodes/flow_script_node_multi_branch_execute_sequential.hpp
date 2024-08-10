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
	};

protected:
	static void _bind_methods();

public:
	virtual String get_type_id() const override { return "multi_branch_execute_sequential"; }
	virtual String get_type_name() const override { return "Execute Sub-Branches Sequentially"; }
	virtual String get_type_description() const override { return "Executes a list of branches in order, then proceeds."; }
	virtual void exec_startup(FlowScriptNodeContext *p_context) override;
	virtual void exec_step(FlowScriptNodeContext *p_context) override;
	virtual void set_state(FlowScriptNodeContext *p_context, const Dictionary &p_state) override;
	virtual void get_state(const FlowScriptNodeContext *p_context, Dictionary &r_state) const override;
};


VARIANT_ENUM_CAST(FlowScriptNodeMultiBranchExecuteSequential::TemporaryVariable);


#endif // FLOW_SCRIPT_NODE_MULTI_BRANCH_EXECUTE_SEQUENTIAL_HPP
