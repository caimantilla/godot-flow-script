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
	int64_t connection_count = 0;

protected:
	static void _bind_methods();

public:
	virtual void set_json_data(const Dictionary &p_data) override;
	virtual void get_json_data(Dictionary &r_data) const override;

	virtual void get_output_connection_list_lengths(List<int64_t> &r_lengths) const override;

	void set_connection_count(const int64_t p_count);
	int64_t get_connection_count() const;
};


VARIANT_ENUM_CAST(FlowScriptNodeMultiBranchExecute::ConnectionList);


#endif // FLOW_SCRIPT_NODE_MULTI_BRANCH_EXECUTE_HPP
