#include "flow_script_node_type_info.hpp"
#include "../flow_script_node_custom.hpp"
#include "editor/editor_node.h"
#include "editor/editor_string_names.h"


bool FlowScriptNodeTypeInfo::operator==(const FlowScriptNodeTypeInfo &p_other) const
{
	return (
			impl_type == p_other.impl_type
			&& enabled == p_other.enabled
			&& type_id == p_other.type_id
			&& type_name == p_other.type_name
			&& type_category == p_other.type_category
			&& type_description == p_other.type_description
			&& editable_name == p_other.editable_name
			&& editable_size == p_other.editable_size
			&& node_native_class_name == p_other.node_native_class_name
			&& editor_native_class_name == p_other.editor_native_class_name
			&& node_script == p_other.node_script
			&& node_script_class_name == p_other.node_script_class_name
			&& editor_script == p_other.editor_script
			&& editor_scene == p_other.editor_scene
	);
}


bool FlowScriptNodeTypeInfo::operator!=(const FlowScriptNodeTypeInfo &p_other) const
{
	return !operator==(p_other);
}


bool FlowScriptNodeTypeInfo::is_valid() const
{
	return enabled;
}


Dictionary FlowScriptNodeTypeInfo::to_dictionary() const
{
	Dictionary d;

	d["impl_type"] = impl_type;
	d["enabled"] = enabled;
	d["type_id"] = type_id;
	d["type_name"] = type_name;
	d["type_category"] = type_category;
	d["type_description"] = type_description;
	d["editable_name"] = editable_name;
	d["editable_size"] = editable_size;
	d["node_native_class_name"] = node_native_class_name;
	d["editor_native_class_name"] = editor_native_class_name;
	d["node_script"] = node_script;
	d["node_script_class_name"] = node_script_class_name;
	d["editor_script"] = editor_script;
	d["editor_scene"] = editor_scene;

	return d;
}


FlowScriptNodeTypeInfo FlowScriptNodeTypeInfo::create_null()
{
	return FlowScriptNodeTypeInfo();
}


FlowScriptNodeTypeInfo FlowScriptNodeTypeInfo::create_from_dictionary(const Dictionary &p_dict)
{
	FlowScriptNodeTypeInfo type;

	type.impl_type = (ImplementType)(int) p_dict["impl_type"];
	type.enabled = p_dict["enabled"];
	type.type_id = p_dict["type_id"];
	type.type_name = p_dict["type_name"];
	type.type_category = p_dict["type_category"];
	type.type_description = p_dict["type_description"];
	type.editable_name = p_dict["editable_name"];
	type.editable_size = p_dict["editable_size"];
	type.node_native_class_name = p_dict["node_native_class_name"];
	type.editor_native_class_name = p_dict["editor_native_class_name"];
	type.node_script = p_dict["node_script"];
	type.node_script_class_name = p_dict["node_script_class_name"];
	type.editor_script = p_dict["editor_script"];
	type.editor_scene = p_dict["editor_scene"];

	return type;
}


Ref<Texture2D> FlowScriptNodeTypeInfo::get_icon()
{
	Ref<Texture2D> icon;

	EditorNode *en = EditorNode::get_singleton();
	ERR_FAIL_NULL_V(en, icon);
	ERR_FAIL_COND_V(!en->is_editor_ready(), icon);

	if (impl_type == IMPL_NATIVE)
	{
		icon = en->get_class_icon(node_native_class_name, "FlowScriptNode");
	}
	else if (impl_type == IMPL_SCRIPTABLE)
	{
		icon = en->get_class_icon(node_script_class_name, "FlowScriptNodeCustom");
	}

	return icon;
}


FlowScriptNodeTypeInfo FlowScriptNodeTypeInfo::create_from_script(const Ref<Script> &p_script, CreateError &r_err)
{
	r_err = CREATE_OK;
	bool type_create_allowed = false;

	FlowScriptNodeTypeInfo type_info = {
		.impl_type = IMPL_SCRIPTABLE,
		.node_native_class_name = SNAME("FlowScriptNodeCustom"),
		.editor_native_class_name = SNAME("FlowScriptNodeEditor"),
	};

	if (p_script.is_null())
	{
		r_err = CREATE_ERR_SCRIPT_NULL;
	}
	else if (!p_script->can_instantiate())
	{
		r_err = CREATE_ERR_SCRIPT_INCOMPLETE;
	}
	else if (p_script->get_instance_base_type() != SNAME("FlowScriptNodeCustom"))
	{
		r_err = CREATE_ERR_INVALID_BASE_CLASS;
	}
	else if (!p_script->is_tool())
	{
		r_err = CREATE_ERR_NOT_TOOL;
	}
	else if (p_script->get_global_name() == StringName())
	{
		r_err = CREATE_ERR_NAME_NOT_GLOBAL;
	}
	else
	{
		Ref<FlowScriptNodeCustom> node;
		node.instantiate();
		node->set_script(p_script);

		if (!node->can_instantiate_type())
		{
			r_err = CREATE_ERR_TYPE_INSTANTIATE_BAN;
		}
		else
		{
			type_create_allowed = node->can_instantiate_type();

			type_info.type_name = node->get_type_name();
			type_info.type_category = node->get_type_category();
			type_info.type_description = node->get_type_description();

			type_info.editable_name = node->can_edit_name();
			type_info.editable_size = node->can_edit_size();

			type_info.node_script = p_script;
			type_info.node_script_class_name = p_script->get_global_name();

			if (!type_info.node_script_class_name.is_empty())
			{
				type_info.type_id = type_info.node_script_class_name;
			}
			else
			{
				type_info.type_id = type_info.node_script->get_path();
			}

			String path_editor = node->get_type_editor();
			bool editor_ok = false;

			// Support relative paths.
			// For users, the recommended order of priority, from greatest to least: Relative Path, UID, res:// Path
			if (!ResourceLoader::exists(path_editor))
			{
				path_editor = p_script->get_path().get_base_dir().path_join(path_editor);
			}
			if (ResourceLoader::exists(path_editor))
			{
				Ref<Resource> editor_res = ResourceLoader::load(path_editor);
				Ref<PackedScene> editor_scn = editor_res;
				if (editor_scn.is_valid())
				{
					if (editor_scn->can_instantiate())
					{
						{
							Node *editor_inst = editor_scn->instantiate();
							if (editor_inst != nullptr)
							{
								if (editor_inst->get_class_name() == type_info.editor_native_class_name)
								{
									type_info.editor_scene = editor_scn;
									editor_ok = true;
								}
								else
								{
									memdelete(editor_inst);
								}
							}
						}
					}
				}
				else
				{
					Ref<Script> editor_scr = editor_res;
					if (editor_scr.is_valid())
					{
						if (editor_scr->can_instantiate() && editor_scr->get_instance_base_type() == SNAME("FlowScriptNodeEditor"))
						{
							type_info.editor_script = editor_scr;
							editor_ok = true;
						}
					}
				}
			}
			if (!editor_ok)
			{
				r_err = CREATE_ERR_NO_EDITOR;
			}
		}
	}

	type_info.enabled = (r_err == CREATE_OK) && type_create_allowed;

	return type_info;
}


FlowScriptNodeTypeInfo FlowScriptNodeTypeInfo::create_from_script_no_check(const Ref<Script> &p_script)
{
	CreateError dummy_err;
	return create_from_script(p_script, dummy_err);
}
