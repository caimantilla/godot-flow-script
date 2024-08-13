#include "flow_script_node_type_db.hpp"
#include "core/error/error_macros.h"
#include "scene/resources/packed_scene.h"
#include "core/object/script_language.h"
#include "flow_script_node.hpp"

#ifdef TOOLS_ENABLED
#include "editor/editor_interface.h"
#include "editor/editor_node.h"
#include "editor/filesystem_dock.h"
#include "flow_script_node_editor.hpp"
#include "editor/plugins/flow_script_editor_plugin.hpp"
#endif // TOOLS_ENABLED


FlowScriptNodeTypeDB *FlowScriptNodeTypeDB::singleton = nullptr;


FlowScriptNodeTypeDB *FlowScriptNodeTypeDB::get_singleton()
{
	return singleton;
}


#ifdef TOOLS_ENABLED


void FlowScriptNodeTypeDB::init_editor(FlowScriptEditorPlugin *p_plugin)
{
	EditorNode::get_singleton()->connect("resource_saved", callable_mp(this, &FlowScriptNodeTypeDB::on_resource_saved));
	FileSystemDock::get_singleton()->connect("resource_removed", callable_mp(this, &FlowScriptNodeTypeDB::on_resource_removed));
	FileSystemDock::get_singleton()->get_script_create_dialog()->connect("script_created", callable_mp(this, &FlowScriptNodeTypeDB::on_script_created));
}


FlowScriptNodeEditor *FlowScriptNodeTypeDB::create_editor_for_node(FlowScriptNode *p_node)
{
	ERR_FAIL_NULL_V(p_node, nullptr);
	
	String editor_str = p_node->get_type_editor();
	StringName editor_sn = editor_str;

	if (ClassDB::class_exists(editor_sn) && ClassDB::can_instantiate(editor_sn))
	{
		Object *obj = ClassDB::instantiate(editor_sn);
		ERR_FAIL_NULL_V(obj, nullptr);
		FlowScriptNodeEditor *editor = Object::cast_to<FlowScriptNodeEditor>(obj);
		if (editor == nullptr)
		{
			memdelete(obj);
			ERR_FAIL_V(nullptr);
		}
		else
		{
			return editor;
		}
	}
	else if (ResourceLoader::exists(editor_str))
	{
		Ref<Resource> res = ResourceLoader::load(editor_str);
		ERR_FAIL_COND_V(!res.is_valid(), nullptr);
		Ref<PackedScene> scene = res;
		if (scene.is_valid() && scene->can_instantiate())
		{
			Node *instance = scene->instantiate();
			ERR_FAIL_NULL_V(instance, nullptr);
			FlowScriptNodeEditor *editor = Object::cast_to<FlowScriptNodeEditor>(instance);
			if (editor == nullptr)
			{
				memdelete(instance);
				ERR_FAIL_V(nullptr);
			}
		}
		else
		{
			Ref<Script> script = res;
			if (script.is_valid() && script->can_instantiate() && script->get_instance_base_type() == SNAME("FlowScriptNodeEditor"))
			{
				FlowScriptNodeEditor *editor = memnew(FlowScriptNodeEditor);
				editor->set_script(Ref<Script>(script));
				return editor;
			}
		}
	}
	ERR_FAIL_V(nullptr);
}


void FlowScriptNodeTypeDB::on_resource_saved(const Ref<Resource> &p_resource)
{
}


void FlowScriptNodeTypeDB::on_resource_removed(const Ref<Resource> &p_resource)
{
	Ref<Script> script = p_resource;
	if (!script.is_valid() || script->get_instance_base_type() != SNAME("FlowScriptNodeCustom"))
	{
		return;
	}
	if (custom_node_script_delete_queue.is_empty())
	{
		callable_mp(this, &FlowScriptNodeTypeDB::process_custom_node_script_delete_queue).call_deferred();
	}
	custom_node_script_delete_queue.push_back(script);
}


void FlowScriptNodeTypeDB::on_script_created(const Ref<Script> &p_script)
{
}


#endif // TOOLS_ENABLED


FlowScriptNodeTypeDB::FlowScriptNodeTypeDB()
{
	CRASH_COND_MSG(singleton != nullptr, "FlowScriptNodeTypeDB is a singleton. Do not instantiate it multiple times.");
	singleton = this;
}


FlowScriptNodeTypeDB::~FlowScriptNodeTypeDB()
{
	singleton = nullptr;
}
