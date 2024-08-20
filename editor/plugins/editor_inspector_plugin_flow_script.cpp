#include "editor_inspector_plugin_flow_script.hpp"
#include "flow_script_editor_plugin.hpp"
#include "../../flow_script_node.hpp"
#include "../../flow_script_node_custom.hpp"
#include "../flow_script_node_type_db.hpp"
#include "../flow_script_node_type_info.hpp"


bool EditorInspectorPluginFlowScript::can_handle(Object *p_object)
{
	return Object::cast_to<FlowScriptNode>(p_object) != nullptr;
}


bool EditorInspectorPluginFlowScript::parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide)
{
	FlowScriptNode *node = Object::cast_to<FlowScriptNode>(p_object);
	if (node != nullptr)
	{
		const FlowScriptNodeTypeInfo &node_type = FlowScriptNodeTypeDB::get_singleton()->get_type_of_node(node);
		if (!node_type.enabled)
		{
			return false;
		}
		if (
			(p_path == "script" && (node->has_editor_dependencies() || Object::cast_to<FlowScriptNodeCustom>(node) == nullptr))
			|| (p_path == "resource_path" && !node_type.name_assignable)
		)
		{
			return true;
		}
	}
	else
	{
		if (p_type == Variant::OBJECT)
		{
			Variant property_value = p_object->get(p_path);
			if (property_value.get_type() == Variant::OBJECT)
			{
				FlowScript *script = Object::cast_to<FlowScript>(property_value);
				if (script != nullptr)
				{
					plugin->edit_flow_script_if_not_open(script);
				}
			}
		}
	}
	return false;
}


EditorInspectorPluginFlowScript::EditorInspectorPluginFlowScript(FlowScriptEditorPlugin *p_plugin)
{
	plugin = p_plugin;
}
