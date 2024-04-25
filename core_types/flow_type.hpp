#ifndef FLOW_TYPE_HPP
#define FLOW_TYPE_HPP

#include "core/io/resource.h"
#include "scene/resources/texture.h"
#include "core/string/ustring.h"
#include "scene/resources/texture.h"


class FlowType final : public Resource
{
	GDCLASS(FlowType, Resource);

	friend class FlowTypeDB;

private:
	bool native; // native types are initialized by a module, should not be serialized
	String id;
	// String name;
	String category;
	String description;
	String icon; // icon name from EditorIcons, or a texture path
	String node; // can be class name, script path...
	String editor; // can be class name, script path, scene...
	bool nameable; // user will be prompted to name new instances of the type, and be allowed to rename existing ones

	void set_native(const bool p_enabled);
	void set_id(const String &p_id);

protected:
	static void _bind_methods();

public:
	static Ref<FlowType> create_native_type(const String &p_id, const String &p_name, const String &p_category, const String &p_description, const String &p_icon, const String &p_node, const String &p_editor, const bool p_nameable);

	bool is_native() const;
	String get_id() const;
	// void set_name(const String &p_name);
	// String get_name() const;
	void set_category(const String &p_category);
	String get_category() const;
	void set_description(const String &p_description);
	String get_description() const;
	void set_icon(const String &p_icon);
	String get_icon() const;
	void set_node(const String &p_node);
	String get_node() const;
	void set_editor(const String &p_editor);
	String get_editor() const;
	void set_nameable(const bool p_enabled);
	bool is_nameable() const;

	Ref<Texture2D> get_icon_texture() const;

	FlowType();
};


#endif
