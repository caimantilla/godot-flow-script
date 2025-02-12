#ifndef FLOW_SCRIPT_NODE_TYPE_INFO_HPP
#define FLOW_SCRIPT_NODE_TYPE_INFO_HPP


#include "core/string/ustring.h"
#include "core/object/script_language.h"
#include "scene/resources/packed_scene.h"
#include "scene/resources/texture.h"


struct FlowScriptNodeTypeInfo final
{
	enum ImplementType
	{
		IMPL_NATIVE,
		IMPL_SCRIPTABLE,
	};

	enum CreateError
	{
		CREATE_OK = 0,
		CREATE_ERR_SCRIPT_NULL,
		CREATE_ERR_SCRIPT_INCOMPLETE,
		CREATE_ERR_INVALID_BASE_CLASS,
		CREATE_ERR_NOT_TOOL,
		CREATE_ERR_TYPE_INSTANTIATE_BAN,
		CREATE_ERR_NO_EDITOR,
		CREATE_ERR_NAME_NOT_GLOBAL,
		CREATE_MAX,
	};

	ImplementType impl_type = IMPL_NATIVE;
	bool enabled = false;
	String type_id, type_name, type_category, type_description;
	bool editable_name = false;
	bool editable_size = false;
	StringName node_native_class_name, editor_native_class_name;
	Ref<Script> node_script;
	StringName node_script_class_name;
	Ref<Script> editor_script;
	Ref<PackedScene> editor_scene;

	bool operator==(const FlowScriptNodeTypeInfo &p_other) const;
	bool operator!=(const FlowScriptNodeTypeInfo &p_other) const;

	bool is_valid() const;
	Dictionary to_dictionary() const;
	Ref<Texture2D> get_icon();

	static FlowScriptNodeTypeInfo create_null();
	static FlowScriptNodeTypeInfo create_from_dictionary(const Dictionary &p_dict);
	static FlowScriptNodeTypeInfo create_from_script(const Ref<Script> &p_script, CreateError &r_err);
	static FlowScriptNodeTypeInfo create_from_script_no_check(const Ref<Script> &p_script);
};


#endif // FLOW_SCRIPT_NODE_TYPE_INFO_HPP
