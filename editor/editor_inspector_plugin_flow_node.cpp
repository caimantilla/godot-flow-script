#include "editor_inspector_plugin_flow_node.hpp"
#include "../core_types/flow_node.hpp"


bool EditorInspectorPluginFlowNode::can_handle(Object *p_object)
{
	return Object::cast_to<FlowNode>(p_object) != nullptr;
}


bool EditorInspectorPluginFlowNode::parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide)
{
	FlowNode *node = Object::cast_to<FlowNode>(p_object);
	if (node == nullptr)
	{
		return false;
	}
	// ERR_FAIL_NULL_V_MSG(node, false, "Tried parsing the property of a non-FlowNode object. Shouldn't this be stopped by can_handle?");

	StringName property_sn = p_path;

	// Hide the script property of FlowNodes to prevent the user from breaking anything by detaching a non-native FlowNode's script.
	if (property_sn == SNAME("script"))
	{
		return true;
	}
	else if (node->is_property_flow_node_reference(property_sn))
	{
		// TODO: Add a custom EditorProperty with a Button for picking a FlowNode in the graph, and make its selectability be synchronized with the visibility of the FlowScript editor.
		return true;
	}
	else if (node->is_property_flow_node_reference_list(property_sn))
	{
		// TODO: Add a custom EditorProperty GUI for FlowNode lists as well.
		return true;
	}

	return false;
}
