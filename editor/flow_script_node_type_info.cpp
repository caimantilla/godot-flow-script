#include "flow_script_node_type_info.hpp"
#include "flow_script_node_custom.hpp"


FlowScriptNodeTypeInfo FlowScriptNodeTypeInfo::create_native_type(const String &p_id, const StringName &p_node_class, const StringName &p_editor_class, const bool p_name_assignable, const String &p_name, const String &p_category, const String &p_description)
{
	FlowScriptNodeTypeInfo ret;

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
			ret.type.name_assignable = false;
			ret.type.name = node->get_type_name();
			ret.type.category = node->get_type_category();
			ret.type.description = node->get_type_category();

			ret.type.node_script = p_script;

			String editor_path = node->get_type_editor();
			if (ResourceLoader::exists(editor_path))
			{
				Ref<Resource> editor_res = ResourceLoader::load(editor_path);
				Ref<PackedScene> editor_scn = editor_res;
				if (editor_scn.is_valid())
				{
					if (editor_scn->can_instantiate())
					{
						ret.type.editor_scene = editor_scn;
						return ret;
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
							return ret;
						}
					}
				}
			}
			ret.error = ScriptCreateResult::ERR_NO_EDITOR;
		}
	}
	return ret;

	ERR_FAIL_COND_V(!p_script.is_valid(), ret);
	ERR_FAIL_COND_V(!p_script->can_instantiate(), ret);
	ERR_FAIL_COND_V(p_script->get_instance_base_type() != SNAME("FlowScriptNodeCustom"), ret);
	ERR_FAIL_COND_V(!p_script->is_tool(), ret);

	Ref<FlowScriptNodeCustom> node;
	node.instantiate();
	node->set_script(p_script);

	ret.native = false;
	ret.custom = true;

	ret.id = node->get_type_id();
	ret.node_class = "FlowScriptNodeCustom";
	ret.editor_class = node->get_type_editor();
	ret.name_assignable = false;
	ret.name = node->get_type_name();
	ret.category = node->get_type_category();
	ret.description = node->get_type_description();
	ret.node_script = p_script;

	return ret;
}


FlowScriptNodeTypeInfo::FlowScriptNodeTypeInfo()
{
}
