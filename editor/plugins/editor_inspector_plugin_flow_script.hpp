#ifndef EDITOR_INSPECTOR_PLUGIN_FLOW_SCRIPT_HPP
#define EDITOR_INSPECTOR_PLUGIN_FLOW_SCRIPT_HPP


#include "editor/editor_inspector.h"


class EditorInspectorPluginFlowScript : public EditorInspectorPlugin
{
	GDCLASS(EditorInspectorPluginFlowScript, EditorInspectorPlugin);

public:
	virtual bool can_handle(Object *p_object) override;
	virtual bool parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide = false) override;
};


#endif // EDITOR_INSPECTOR_PLUGIN_FLOW_SCRIPT_HPP
