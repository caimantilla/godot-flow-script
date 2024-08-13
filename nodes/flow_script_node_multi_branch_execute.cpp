#include "flow_script_node_multi_branch_execute.hpp"


void FlowScriptNodeMultiBranchExecute::_bind_methods()
{
	BIND_CONSTANT(MAX_CONNECTIONS);

	BIND_ENUM_CONSTANT(CONNECTION_LIST_ADVANCE);
	BIND_ENUM_CONSTANT(CONNECTION_LIST_BRANCHES);

	ClassDB::bind_method(D_METHOD("set_connection_count", "count"), &FlowScriptNodeMultiBranchExecute::set_connection_count);
	ClassDB::bind_method(D_METHOD("get_connection_count"), &FlowScriptNodeMultiBranchExecute::get_connection_count);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "connection_count", PROPERTY_HINT_RANGE, "0," + itos(MAX_CONNECTIONS) + ",1"), "set_connection_count", "get_connection_count");
}


void FlowScriptNodeMultiBranchExecute::set_json_data(const Dictionary &p_data)
{
	if (p_data.has("connection_count"))
	{
		set_connection_count(p_data["connection_count"]);
	}
}


void FlowScriptNodeMultiBranchExecute::get_json_data(Dictionary &r_data) const
{
	r_data["connection_count"] = connection_count;
}


void FlowScriptNodeMultiBranchExecute::get_output_connection_list_lengths(List<int64_t> &r_lengths) const
{
	r_lengths.push_back(1);
	r_lengths.push_back(connection_count);
}


void FlowScriptNodeMultiBranchExecute::set_connection_count(const int64_t p_count)
{
	int64_t new_count = CLAMP(p_count, 0, MAX_CONNECTIONS);
	if (connection_count == new_count)
		return;
	connection_count = new_count;
	request_output_connection_data_update();
	emit_changed();
}


int64_t FlowScriptNodeMultiBranchExecute::get_connection_count() const
{
	return connection_count;
}
