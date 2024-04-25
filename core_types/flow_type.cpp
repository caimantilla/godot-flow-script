#include "flow_type.hpp"
#include "core/io/resource_loader.h"
#include "scene/resources/theme.h"
#include "editor/editor_interface.h"
#include "editor/editor_string_names.h"


static String TYPE_NAME_TEXTURE_2D = "Texture2D";


void FlowType::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("is_native"), &FlowType::is_native);
	ClassDB::bind_method(D_METHOD("get_id"), &FlowType::get_id);
	// ClassDB::bind_method(D_METHOD("set_name", "name"), &FlowType::set_name);
	// ClassDB::bind_method(D_METHOD("get_name"), &FlowType::get_name);
	ClassDB::bind_method(D_METHOD("set_category", "category"), &FlowType::set_category);
	ClassDB::bind_method(D_METHOD("get_category"), &FlowType::get_category);
	ClassDB::bind_method(D_METHOD("set_description", "description"), &FlowType::set_description);
	ClassDB::bind_method(D_METHOD("get_description"), &FlowType::get_description);
	ClassDB::bind_method(D_METHOD("set_icon", "icon"), &FlowType::set_icon);
	ClassDB::bind_method(D_METHOD("get_icon"), &FlowType::get_icon);
	ClassDB::bind_method(D_METHOD("set_node", "node"), &FlowType::set_node);
	ClassDB::bind_method(D_METHOD("get_node"), &FlowType::get_node);
	ClassDB::bind_method(D_METHOD("set_editor", "editor"), &FlowType::set_editor);
	ClassDB::bind_method(D_METHOD("get_editor"), &FlowType::get_editor);
	ClassDB::bind_method(D_METHOD("set_nameable", "enabled"), &FlowType::set_nameable);
	ClassDB::bind_method(D_METHOD("is_nameable"), &FlowType::is_nameable);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "native", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), "", "is_native");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), "", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "name", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR), "set_name", "get_name");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "category"), "set_category", "get_category");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "description", PROPERTY_HINT_MULTILINE_TEXT), "set_description", "get_description");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "icon", PROPERTY_HINT_FILE, "*.png,*.svg,*.webp,*.jpg,*.jpeg"), "set_icon", "get_icon");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "node", PROPERTY_HINT_FILE, "*.gd,*.cs"), "set_node", "get_node");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "editor", PROPERTY_HINT_FILE, "*.tscn,*.scn"), "set_editor", "get_editor");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "nameable"), "set_nameable", "is_nameable");
}


void FlowType::set_native(const bool p_enabled)
{
	if (p_enabled == native)
	{
		return;
	}

	native = p_enabled;
	emit_changed();
}


bool FlowType::is_native() const
{
	return native;
}


void FlowType::set_id(const String &p_id)
{
	ERR_FAIL_COND_MSG(!native, "ID can only be set for native FlowTypes.");

	id = p_id;
	emit_changed();
}


String FlowType::get_id() const
{
	if (native)
	{
		return id;
	}
	else
	{
		return get_path().get_file().get_basename();
	}
}


// void FlowType::set_name(const String &p_name)
// {
// 	if (p_name == name)
// 	{
// 		return;
// 	}

// 	name = p_name;
// 	emit_changed();
// }


// String FlowType::get_name() const
// {
// 	return name;
// }


void FlowType::set_category(const String &p_category)
{
	if (p_category == category)
	{
		return;
	}

	category = p_category;
	emit_changed();
}


String FlowType::get_category() const
{
	return category;
}


void FlowType::set_description(const String &p_description)
{
	if (p_description == description)
	{
		return;
	}

	description = p_description;
	emit_changed();
}


String FlowType::get_description() const
{
	return description;
}


void FlowType::set_icon(const String &p_icon)
{
	if (p_icon == icon)
	{
		return;
	}

	icon = p_icon;
	emit_changed();
}


String FlowType::get_icon() const
{
	return icon;
}


void FlowType::set_node(const String &p_node)
{
	if (p_node == node)
	{
		return;
	}

	node = p_node;
	emit_changed();
}


String FlowType::get_node() const
{
	return node;
}


void FlowType::set_editor(const String &p_editor)
{
	if (p_editor == editor)
	{
		return;
	}

	editor = p_editor;
	emit_changed();
}


String FlowType::get_editor() const
{
	return editor;
}


void FlowType::set_nameable(const bool p_enabled)
{
	if (p_enabled == nameable)
	{
		return;
	}

	nameable = p_enabled;
	emit_changed();
}


bool FlowType::is_nameable() const
{
	return nameable;
}



Ref<Texture2D> FlowType::get_icon_texture() const
{
	Ref<Texture2D> tex = nullptr;

#ifdef TOOLS_ENABLED
	if (Engine::get_singleton()->is_editor_hint())
	{
		if (EditorInterface::get_singleton()->get_editor_theme()->has_icon(icon, EditorStringNames::get_singleton()->EditorIcons))
		{
			tex = EditorInterface::get_singleton()->get_editor_theme()->get_icon(icon, EditorStringNames::get_singleton()->EditorIcons);
		}
	}
#endif

	if (tex == nullptr && ResourceLoader::exists(icon, TYPE_NAME_TEXTURE_2D))
	{
		Ref<Resource> res = ResourceLoader::load(icon, TYPE_NAME_TEXTURE_2D);
		if (!res.is_null() && res.is_valid())
		{
			tex = Ref<Texture2D>(res);
		}
	}

	return tex;
}


Ref<FlowType> FlowType::create_native_type(const String &p_id, const String &p_name, const String &p_category, const String &p_description, const String &p_icon, const String &p_node, const String &p_editor, const bool p_nameable)
{
	Ref<FlowType> type;
	type.instantiate();

	type->native = true;
	type->id = p_id;
	type->set_name(p_name);
	type->category = p_category;
	type->description = p_description;
	type->icon = p_icon;
	type->node = p_node;
	type->editor = p_editor;
	type->nameable = p_nameable;

	return type;
}


FlowType::FlowType()
{
}
