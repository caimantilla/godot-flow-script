#ifndef FLOW_SCRIPT_NODE_TYPE_INFO_HPP
#define FLOW_SCRIPT_NODE_TYPE_INFO_HPP


#include "core/string/ustring.h"
#include "core/object/script_language.h"
#include "scene/resources/packed_scene.h"


class FlowScriptNodeTypeInfo final
{
public:
	class ScriptCreateResult;

private:
	void validate_category(String &r_category);

public:
	bool enabled = false;
	bool custom = false;
	bool native = false;
	String id;
	StringName node_class;
	StringName editor_class;
	bool name_assignable = false;
	String name;
	String category;
	String description;
	Ref<Script> node_script;
	StringName node_script_class_name;
	Ref<Script> editor_script;
	Ref<PackedScene> editor_scene;

	static FlowScriptNodeTypeInfo create_native_type(const String &p_id, const StringName &p_node_class, const StringName &p_editor_class, const bool p_name_assignable, const String &p_name, const String &p_category, const String &p_description);
	static ScriptCreateResult create_script_type(const Ref<Script> &p_script);

	FlowScriptNodeTypeInfo();
};


class FlowScriptNodeTypeInfo::ScriptCreateResult
{
public:
	enum CreateError
	{
		OK = 0,
		ERR_SCRIPT_NULL = 1,
		ERR_SCRIPT_INCOMPLETE = 2,
		ERR_INVALID_BASE_CLASS = 3,
		ERR_NOT_TOOL = 4,
		ERR_TYPE_INSTANTIATE_BAN = 5,
		ERR_NO_EDITOR = 6,
		ERR_NAME_NOT_GLOBAL = 7,
	};

public:
	CreateError error;
	FlowScriptNodeTypeInfo type;
};


#endif // FLOW_SCRIPT_NODE_TYPE_INFO_HPP
