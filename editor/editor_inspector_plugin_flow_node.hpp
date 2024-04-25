#ifndef EDITOR_INSPECTOR_PLUGIN_FLOW_NODE_HPP
#define EDITOR_INSPECTOR_PLUGIN_FLOW_NODE_HPP

#include "editor/editor_inspector.h"


class EditorInspectorPluginFlowNode final : public EditorInspectorPlugin
{
	GDCLASS(EditorInspectorPluginFlowNode, EditorInspectorPlugin);

public:
	virtual bool can_handle(Object *p_object) override;
	virtual bool parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide = false) override;
};


#endif
