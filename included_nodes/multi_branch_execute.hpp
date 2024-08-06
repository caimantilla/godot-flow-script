#ifndef FLOW_SCRIPT_NODE_MULTI_BRANCH_EXECUTE_HPP
#define FLOW_SCRIPT_NODE_MULTI_BRANCH_EXECUTE_HPP


#include "common.hpp"


class FlowScriptNodeMultiBranchExecuteBase : public FlowScriptNode
{
	GDCLASS(FlowScriptNodeMultiBranchExecuteBase, FlowScriptNode);

public:
	enum
	{
		MAX_CONNECTIONS = 8,
	};
	enum ConnectionList
	{
		CONNECTION_LIST_PROCEED = 0,
		CONNECTION_LIST_BRANCHES = 1,
	};

private:
	int64_t connection_count = 0;

protected:
	static void _bind_methods();

public:
	virtual String get_type_editor() const override { return "FlowScriptNodeEditorMultiBranchExecute"; }

	void set_connection_count(const int64_t p_count);
	int64_t get_connection_count() const;
};


VARIANT_ENUM_CAST(FlowScriptNodeMultiBranchExecuteBase::ConnectionList);


class FlowScriptNodeMultiBranchExecuteSequential : public FlowScriptNodeMultiBranchExecuteBase
{
	GDCLASS(FlowScriptNodeMultiBranchExecuteSequential, FlowScriptNodeMultiBranchExecuteBase);

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


class FlowScriptNodeMultiBranchExecuteSimultaneous : public FlowScriptNodeMultiBranchExecuteBase
{
	GDCLASS(FlowScriptNodeMultiBranchExecuteSimultaneous, FlowScriptNodeMultiBranchExecuteBase);

public:
	enum TemporaryVariable
	{
		VARIABLE_STEP_COUNT = 0,
	};

protected:
	static void _bind_methods();

public:
	virtual String get_type_id() const override { return "multi_branch_execute_simultaneous"; }
	virtual String get_type_name() const override { return "Execute Sub-Branches Simultaneously"; }
	virtual String get_type_description() const override { return "Executes a list of branches simultaneously, then proceeds once all have finished."; }
	virtual void exec_startup(FlowScriptNodeContext *p_context) override;
	virtual void exec_step(FlowScriptNodeContext *p_context) override;
	virtual void set_state(FlowScriptNodeContext *p_context, const Dictionary &p_state) override;
	virtual void get_state(const FlowScriptNodeContext *p_context, Dictionary &r_state) const override;
};


#endif // FLOW_SCRIPT_NODE_MULTI_BRANCH_EXECUTE_HPP
