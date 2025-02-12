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
		CONNECTION_LIST_ADVANCE = 0,
		CONNECTION_LIST_BRANCHES = 1,
	};

private:
	int connection_count = 0;

protected:
	static void _bind_methods();

public:
	virtual void set_data_state(const Dictionary &p_data) override;
	virtual Dictionary get_data_state() const override;
	virtual void get_output_connection_list_lengths(List<FlowScriptNodeConnectionListLength> *p_lengths) const override;

	void set_connection_count(const int p_count);
	int get_connection_count() const;
};


VARIANT_ENUM_CAST(FlowScriptNodeMultiBranchExecute::ConnectionList);


#endif // FLOW_SCRIPT_NODE_MULTI_BRANCH_EXECUTE_HPP
