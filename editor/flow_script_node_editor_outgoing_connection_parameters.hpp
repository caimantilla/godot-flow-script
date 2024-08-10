#ifndef FLOW_SCRIPT_NODE_EDITOR_OUTGOING_CONNECTION_PARAMETERS_HPP
#define FLOW_SCRIPT_NODE_EDITOR_OUTGOING_CONNECTION_PARAMETERS_HPP


#include "typedefs.hpp"
#include "flow_script.hpp"


class FlowScriptNodeEditorOutgoingConnectionParameters final
{
private:
	static bool dict_is_int_value_valid(const Dictionary &p_dictionary, const String &p_key);
public:
	FlowScriptNodeID target_node_id = FlowScript::NODE_ID_INVALID;
	int16_t origin_slot = -1;
	int16_t origin_port = -1;

	static FlowScriptNodeEditorOutgoingConnectionParameters create_from_slot(const int16_t p_slot, const FlowScriptNodeID p_target_node_id);
	static FlowScriptNodeEditorOutgoingConnectionParameters create_from_port(const int16_t p_port, const FlowScriptNodeID p_target_node_id);
	static FlowScriptNodeEditorOutgoingConnectionParameters create_from_dictionary(const Dictionary &p_dictionary);
	Dictionary to_dictionary() const;
};


#endif // FLOW_SCRIPT_NODE_EDITOR_OUTGOING_CONNECTION_PARAMETERS_HPP
