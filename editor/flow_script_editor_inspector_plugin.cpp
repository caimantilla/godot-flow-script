#include "flow_script_editor_inspector_plugin.hpp"
#include "../flow_script_node.hpp"
#include "../flow_script_node_custom.hpp"


bool FlowScriptEditorInspectorPlugin::can_handle(Object *p_object)
{
	return Object::cast_to<FlowScriptNode>(p_object) != nullptr;
}


bool FlowScriptEditorInspectorPlugin::parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide)
{
	FlowScriptNode *node = Object::cast_to<FlowScriptNode>(p_object);
	if (node == nullptr)
		return false;
	
	if (p_path == "script" && (node->has_editor_dependencies() || Object::cast_to<FlowScriptNodeCustom>(node) == nullptr))
		return true;
	if (p_path == "resource_path" && !node->can_name_node())
		return true;
	return false;
}
