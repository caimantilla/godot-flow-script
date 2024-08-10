#include "flow_script_node_multi_branch_execute.hpp"


void FlowScriptNodeMultiBranchExecute::_bind_methods()
{
	BIND_CONSTANT(MAX_CONNECTIONS);

	BIND_ENUM_CONSTANT(CONNECTION_LIST_PROCEED);
	BIND_ENUM_CONSTANT(CONNECTION_LIST_BRANCHES);

	ClassDB::bind_method(D_METHOD("set_connection_count", "count"), &FlowScriptNodeMultiBranchExecute::set_connection_count);
	ClassDB::bind_method(D_METHOD("get_connection_count"), &FlowScriptNodeMultiBranchExecute::get_connection_count);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "connection_count", PROPERTY_HINT_RANGE, "0," + itos(MAX_CONNECTIONS) + ",1"), "set_connection_count", "get_connection_count");
}


void FlowScriptNodeMultiBranchExecute::set_connection_count(const int64_t p_count)
{
	if (connection_count == p_count)
		return;
	connection_count = p_count;
	emit_changed();
}


int64_t FlowScriptNodeMultiBranchExecute::get_connection_count() const
{
	return connection_count;
}
