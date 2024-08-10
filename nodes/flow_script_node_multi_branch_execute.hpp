#ifndef FLOW_SCRIPT_NODE_MULTI_BRANCH_EXECUTE_HPP
#define FLOW_SCRIPT_NODE_MULTI_BRANCH_EXECUTE_HPP


#include "common.hpp"


class FlowScriptNodeMultiBranchExecute : public FlowScriptNode
{
	GDCLASS(FlowScriptNodeMultiBranchExecute, FlowScriptNode);

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


VARIANT_ENUM_CAST(FlowScriptNodeMultiBranchExecute::ConnectionList);


#endif // FLOW_SCRIPT_NODE_MULTI_BRANCH_EXECUTE_HPP
