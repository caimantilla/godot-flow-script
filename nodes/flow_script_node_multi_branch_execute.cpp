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


void FlowScriptNodeMultiBranchExecute::set_data_state(const Dictionary &p_data)
{
	set_connection_count(p_data.get("connection_count", connection_count));
}


Dictionary FlowScriptNodeMultiBranchExecute::get_data_state() const
{
	Dictionary d;
	d["connection_count"] = connection_count;
	return d;
}


void FlowScriptNodeMultiBranchExecute::get_output_connection_list_lengths(List<FlowScriptNodeConnectionListLength> *p_lengths) const
{
	p_lengths->push_back(1);
	p_lengths->push_back(connection_count);
}


void FlowScriptNodeMultiBranchExecute::set_connection_count(const int p_count)
{
	const int new_count = CLAMP(p_count, 0, MAX_CONNECTIONS);
	if (connection_count == new_count)
	{
		return;
	}
	connection_count = new_count;
	emit_changed();
}


int FlowScriptNodeMultiBranchExecute::get_connection_count() const
{
	return connection_count;
}
