#include "flow_script_node_type_info.hpp"
#include "../flow_script_node_custom.hpp"


FlowScriptNodeTypeInfo FlowScriptNodeTypeInfo::create_native_type(const String &p_id, const StringName &p_node_class, const StringName &p_editor_class, const bool p_name_assignable, const String &p_name, const String &p_category, const String &p_description)
{
	FlowScriptNodeTypeInfo ret;

	ret.enabled = true;
	ret.native = true;
	ret.custom = false;

	ret.id = p_id;
	ret.node_class = p_node_class;
	ret.editor_class = p_editor_class;
	ret.name_assignable = p_name_assignable;
	ret.name = p_name;
	ret.category = p_category;
	ret.description = p_description;

	return ret;
}


FlowScriptNodeTypeInfo::ScriptCreateResult FlowScriptNodeTypeInfo::create_script_type(const Ref<Script> &p_script)
{
	ScriptCreateResult ret;
	ret.error = ScriptCreateResult::OK;

	if (!p_script.is_valid())
	{
		ret.error = ScriptCreateResult::ERR_SCRIPT_NULL;
	}
	else if (!p_script->can_instantiate())
	{
		ret.error = ScriptCreateResult::ERR_SCRIPT_INCOMPLETE;
	}
	else if (p_script->get_instance_base_type() != SNAME("FlowScriptNodeCustom"))
	{
		ret.error = ScriptCreateResult::ERR_INVALID_BASE_CLASS;
	}
	else if (!p_script->is_tool())
	{
		ret.error = ScriptCreateResult::ERR_NOT_TOOL;
	}
	else if (p_script->get_global_name() == StringName())
	{
		ret.error = ScriptCreateResult::ERR_NAME_NOT_GLOBAL;
	}
	else
	{
		Ref<FlowScriptNodeCustom> node;
		node.instantiate();
		node->set_script(p_script);

		if (!node->can_instantiate_type())
		{
			ret.error = ScriptCreateResult::ERR_TYPE_INSTANTIATE_BAN;
		}
		else
		{
			ret.type.native = false;
			ret.type.custom = true;

			ret.type.id = node->get_type_id();
			ret.type.node_class = SNAME("FlowScriptNodeCustom");
			ret.type.editor_class = SNAME("FlowScriptNodeEditor");
			ret.type.name_assignable = node->can_edit_name();
			ret.type.name = node->get_type_name();
			ret.type.category = node->get_type_category();
			ret.type.validate_category(ret.type.category);
			ret.type.description = node->get_type_category();

			ret.type.node_script = p_script;
			ret.type.node_script_class_name = p_script->get_global_name();

			String editor_path = node->get_type_editor();
			bool editor_ok = false;
			// Support relative path
			if (!ResourceLoader::exists(editor_path))
			{
				editor_path = p_script->get_path().get_base_dir().path_join(editor_path);
			}
			if (ResourceLoader::exists(editor_path))
			{
				Ref<Resource> editor_res = ResourceLoader::load(editor_path);
				Ref<PackedScene> editor_scn = editor_res;
				if (editor_scn.is_valid())
				{
					if (editor_scn->can_instantiate())
					{
						ret.type.editor_scene = editor_scn;
						editor_ok = true;
					}
				}
				else
				{
					Ref<Script> editor_script = editor_res;
					if (editor_script.is_valid())
					{
						if (editor_script->can_instantiate() && editor_script->get_instance_base_type() == SNAME("FlowScriptNodeEditor"))
						{
							ret.type.editor_script = editor_script;
							editor_ok = true;
						}
					}
				}
			}
			if (!editor_ok)
			{
				ret.error = ScriptCreateResult::ERR_NO_EDITOR;
			}
		}
	}
	ret.type.enabled = ret.error == ScriptCreateResult::OK;
	return ret;
}


void FlowScriptNodeTypeInfo::validate_category(String &r_category)
{
	// Clean up double-slashes.
	for (int char_idx = r_category.length() - 1; char_idx > 0; char_idx--)
	{
		if (r_category[char_idx] == '/' && r_category[char_idx - 1] == '/')
		{
			r_category.remove_at(char_idx);
		}
	}
}


FlowScriptNodeTypeInfo::FlowScriptNodeTypeInfo()
{
}
