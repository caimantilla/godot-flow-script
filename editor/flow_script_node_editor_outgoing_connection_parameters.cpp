#include "flow_script_node_editor_outgoing_connection_parameters.hpp"
#include "flow_script.hpp"


FlowScriptNodeEditorOutgoingConnectionParameters FlowScriptNodeEditorOutgoingConnectionParameters::create_from_slot(const int16_t p_slot, const FlowScriptNodeID p_target_node_id)
{
	FlowScriptNodeEditorOutgoingConnectionParameters ret;
	ret.origin_slot = p_slot;
	ret.target_node_id = p_target_node_id;
	return ret;
}


FlowScriptNodeEditorOutgoingConnectionParameters FlowScriptNodeEditorOutgoingConnectionParameters::create_from_port(const int16_t p_port, const FlowScriptNodeID p_target_node_id)
{
	FlowScriptNodeEditorOutgoingConnectionParameters ret;
	ret.origin_port = p_port;
	ret.target_node_id = p_target_node_id;
	return ret;
}


FlowScriptNodeEditorOutgoingConnectionParameters FlowScriptNodeEditorOutgoingConnectionParameters::create_from_dictionary(const Dictionary &p_dictionary)
{
	FlowScriptNodeEditorOutgoingConnectionParameters ret;
	if (dict_is_int_value_valid(p_dictionary, "target_node_id"))
		ret.target_node_id = p_dictionary["target_node_id"];
	if (dict_is_int_value_valid(p_dictionary, "origin_slot"))
		ret.origin_slot = p_dictionary["origin_slot"];
	if (dict_is_int_value_valid(p_dictionary, "origin_port"))
		ret.origin_port = p_dictionary["origin_port"];
	return ret;
}


Dictionary FlowScriptNodeEditorOutgoingConnectionParameters::to_dictionary() const
{
	Dictionary ret;
	ret["target_node_id"] = target_node_id;
	ret["origin_slot"] = origin_slot;
	ret["origin_port"] = origin_port;
	return ret;
}


bool FlowScriptNodeEditorOutgoingConnectionParameters::dict_is_int_value_valid(const Dictionary &p_dictionary, const String &p_key)
{
	if (p_dictionary.has(p_key))
		return p_dictionary[p_key].get_type() == Variant::INT;
	return false;
}
