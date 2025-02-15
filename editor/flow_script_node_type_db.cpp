#include "flow_script_node_type_db.hpp"
#include "flow_script_node_type_info.hpp"
#include "nodes/flow_script_node_editor.hpp"
#include "../flow_script_node_custom.hpp"

#include "core/error/error_macros.h"
#include "core/object/script_language.h"
#include "scene/resources/packed_scene.h"
#include "editor/editor_interface.h"
#include "editor/editor_node.h"
#include "editor/filesystem_dock.h"


FlowScriptNodeTypeDB *FlowScriptNodeTypeDB::singleton = nullptr;


void FlowScriptNodeTypeDB::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_type_count"), &FlowScriptNodeTypeDB::bind_get_type_count);
	ClassDB::bind_method(D_METHOD("get_node_type_list"), &FlowScriptNodeTypeDB::bind_get_node_type_list);
	ClassDB::bind_method(D_METHOD("get_type_by_index", "index"), &FlowScriptNodeTypeDB::bind_get_type_by_index);
	ClassDB::bind_method(D_METHOD("refresh_types"), &FlowScriptNodeTypeDB::refresh_types);

	ADD_SIGNAL(MethodInfo("changed"));
}


FlowScriptNodeTypeDB *FlowScriptNodeTypeDB::get_singleton()
{
	return singleton;
}


void FlowScriptNodeTypeDB::refresh_types()
{
	has_ever_refreshed_types = true;
	cache_types_dirty = true;

	list_script_types.clear();

	List<StringName> global_script_name_list;
	ScriptServer::get_global_class_list(&global_script_name_list);

	for (const StringName &global_script_sn : global_script_name_list)
	{
		const String global_script_str = global_script_sn;
		if (ScriptServer::get_global_class_native_base(global_script_str) != SNAME("FlowScriptNodeCustom"))
		{
			continue;
		}
		const String path = ScriptServer::get_global_class_path(global_script_str);
		ERR_CONTINUE(!ResourceLoader::exists(path, "Script"));

		const Ref<Script> script = ResourceLoader::load(path, "Script");
		ERR_CONTINUE(script.is_null());

		FlowScriptNodeTypeInfo::CreateError type_create_err = FlowScriptNodeTypeInfo::CREATE_OK;
		const FlowScriptNodeTypeInfo type = FlowScriptNodeTypeInfo::create_from_script(script, type_create_err);

		if (type_create_err == FlowScriptNodeTypeInfo::CREATE_OK)
		{
			add_type(type);
		}
	}

	emit_changed();
}


int FlowScriptNodeTypeDB::get_type_count() const
{
	update_type_cache();
	return cache_complete_type_list.size();
}


int FlowScriptNodeTypeDB::bind_get_type_count() const
{
	update_type_cache();
	return cache_complete_type_list_bind.size();
}


Vector<FlowScriptNodeTypeInfo> FlowScriptNodeTypeDB::get_node_type_list() const
{
	update_type_cache();
	return cache_complete_type_list;
}


TypedArray<Dictionary> FlowScriptNodeTypeDB::bind_get_node_type_list() const
{
	update_type_cache();
	return cache_complete_type_list_bind;
}


FlowScriptNodeTypeInfo FlowScriptNodeTypeDB::get_type_by_class_name(const StringName &p_class_name) const
{
	update_type_cache();

	if (cache_map_native_class_to_type_idx.has(p_class_name))
	{
		return list_native_types[cache_map_native_class_to_type_idx[p_class_name]];
	}
	else if (cache_map_custom_node_script_class_name_to_type_idx.has(p_class_name))
	{
		return list_script_types[cache_map_custom_node_script_class_name_to_type_idx[p_class_name]];
	}
	else
	{
		return FlowScriptNodeTypeInfo();
	}
}


FlowScriptNodeTypeInfo FlowScriptNodeTypeDB::get_type_by_index(const int p_index) const
{
	update_type_cache();
	ERR_FAIL_INDEX_V_MSG(p_index, cache_complete_type_list.size(), FlowScriptNodeTypeInfo(), vformat(TTR("Index %d is out of range."), p_index));
	return cache_complete_type_list[p_index];
}


Dictionary FlowScriptNodeTypeDB::bind_get_type_by_index(const int p_index) const
{
	update_type_cache();
	ERR_FAIL_INDEX_V_MSG(p_index, cache_complete_type_list_bind.size(), FlowScriptNodeTypeInfo().to_dictionary(), vformat(TTR("Index %d is out of range."), p_index));
	return cache_complete_type_list_bind[p_index];
}


void FlowScriptNodeTypeDB::add_type(FlowScriptNodeTypeInfo p_type)
{
	switch (p_type.impl_type)
	{
		case FlowScriptNodeTypeInfo::IMPL_NATIVE: {
			cache_types_dirty = true;

			p_type.type_id = p_type.node_native_class_name;
			list_native_types.push_back(p_type);

			emit_changed();
		} break;
		case FlowScriptNodeTypeInfo::IMPL_SCRIPTABLE: {
			cache_types_dirty = true;

			p_type.type_id = p_type.node_script_class_name;
			list_script_types.push_back(p_type);

			emit_changed();
		} break;
		default: {
			ERR_FAIL();
		} break;
	}
}


Ref<FlowScriptNode> FlowScriptNodeTypeDB::instantiate_node_for_type(const FlowScriptNodeTypeInfo &p_type)
{
	ERR_FAIL_COND_V(!p_type.enabled, Ref<FlowScriptNode>());
	ERR_FAIL_COND_V(!ClassDB::class_exists(p_type.node_native_class_name), Ref<FlowScriptNode>());

	Ref<FlowScriptNode> node = Ref<FlowScriptNode>(ClassDB::instantiate(p_type.node_native_class_name));
	ERR_FAIL_COND_V(node.is_null(), Ref<FlowScriptNode>());

	if (p_type.node_script.is_valid())
	{
		node->set_script(p_type.node_script);
	}

	return node;
}


FlowScriptNodeEditor *FlowScriptNodeTypeDB::instantiate_editor_for_type(const FlowScriptNodeTypeInfo &p_type)
{
	ERR_FAIL_COND_V(!p_type.enabled, nullptr);

	if (p_type.editor_scene.is_valid())
	{
		if (p_type.editor_scene->can_instantiate())
		{
			Node *inst = p_type.editor_scene->instantiate();
			ERR_FAIL_NULL_V(inst, nullptr);

			FlowScriptNodeEditor *editor = Object::cast_to<FlowScriptNodeEditor>(inst);
			if (editor == nullptr)
			{
				memdelete(inst);
				ERR_FAIL_V(nullptr);
			}
			else
			{
				return editor;
			}
		}
	}
	else if (p_type.editor_script.is_valid())
	{
		if (p_type.editor_script->can_instantiate() && p_type.editor_script->get_instance_base_type() == SNAME("FlowScriptNodeEditor"))
		{
			FlowScriptNodeEditor *editor = memnew(FlowScriptNodeEditor);
			editor->set_script(p_type.editor_script);
			return editor;
		}
		else
		{
			ERR_FAIL_V(nullptr);
		}
	}
	else if (ClassDB::class_exists(p_type.editor_native_class_name))
	{
		FlowScriptNodeEditor *editor = Object::cast_to<FlowScriptNodeEditor>(ClassDB::instantiate(p_type.editor_native_class_name));
		DEV_ASSERT(editor != nullptr); // A native type should never extend something other than FlowScriptNodeEditor.
		return editor;
	}

	ERR_FAIL_V(nullptr);
}


FlowScriptNodeTypeInfo FlowScriptNodeTypeDB::get_type_of_node(const Ref<FlowScriptNode> &p_node) const
{
	ERR_FAIL_COND_V(p_node.is_null(), FlowScriptNodeTypeInfo());

	update_type_cache();

	const Ref<Script> script = p_node->get_script();
	const FlowScriptNodeCustom *custom_node = Object::cast_to<FlowScriptNodeCustom>(p_node.ptr());

	if (custom_node == nullptr || script.is_null())
	{
		const StringName native_class = p_node->get_class_name();

		ERR_FAIL_COND_V(!cache_map_native_class_to_type_idx.has(native_class), FlowScriptNodeTypeInfo());
		const int idx = cache_map_native_class_to_type_idx[native_class];
		return list_native_types[idx];
	}
	else
	{
		ERR_FAIL_COND_V(!cache_map_custom_node_script_to_type_idx.has(script), FlowScriptNodeTypeInfo());
		const int idx = cache_map_custom_node_script_to_type_idx[script];
		return list_script_types[idx];
	}
}


FlowScriptNodeEditor *FlowScriptNodeTypeDB::create_editor_for_node(const Ref<FlowScriptNode> &p_node)
{
	ERR_FAIL_COND_V(p_node.is_null(), nullptr);

	const FlowScriptNodeTypeInfo type = get_type_of_node(p_node);
	ERR_FAIL_COND_V(!type.is_valid(), nullptr);

	FlowScriptNodeEditor *editor = instantiate_editor_for_type(type);
	return editor;
}


void FlowScriptNodeTypeDB::update_type_cache() const
{
	if (!has_ever_refreshed_types)
	{
		WARN_PRINT(TTR("The FlowScriptNode type list has never been refreshed, so script types will likely be missing."));
	}

	if (!cache_types_dirty)
	{
		return;
	}

	cache_types_dirty = false;

	cache_complete_type_list.resize(list_native_types.size() + list_script_types.size());
	cache_complete_type_list_bind.resize(cache_complete_type_list.size());

	int curr_complete_type_idx = 0;

	for (const FlowScriptNodeTypeInfo &type : list_native_types)
	{
		cache_complete_type_list.write[curr_complete_type_idx] = type;
		cache_complete_type_list_bind[curr_complete_type_idx] = type.to_dictionary();
		curr_complete_type_idx++;
	}
	for (const FlowScriptNodeTypeInfo &type : list_script_types)
	{
		cache_complete_type_list.write[curr_complete_type_idx] = type;
		cache_complete_type_list_bind[curr_complete_type_idx] = type.to_dictionary();
		curr_complete_type_idx++;
	}

	cache_map_native_class_to_type_idx.clear();
	cache_map_custom_node_script_to_type_idx.clear();
	cache_map_custom_node_script_class_name_to_type_idx.clear();

	for (int i = 0; i < list_native_types.size(); i++)
	{
		cache_map_native_class_to_type_idx.insert(list_native_types[i].node_native_class_name, i);
	}
	for (int i = 0; i < list_script_types.size(); i++)
	{
		const FlowScriptNodeTypeInfo &type = list_script_types[i];

		cache_map_custom_node_script_to_type_idx.insert(type.node_script, i);
		if (type.node_script_class_name != StringName())
		{
			cache_map_custom_node_script_class_name_to_type_idx.insert(type.node_script_class_name, i);
		}
	}
}


void FlowScriptNodeTypeDB::process_custom_node_script_delete_queue()
{
	update_type_cache();

	LocalVector<int> delete_idx_list;

	for (const Ref<Script> &script : custom_node_script_delete_queue)
	{
		if (cache_map_custom_node_script_to_type_idx.has(script))
		{
			delete_idx_list.push_back(cache_map_custom_node_script_to_type_idx[script]);
		}
	}

	custom_node_script_delete_queue.clear();

	if (!delete_idx_list.is_empty())
	{
		cache_types_dirty = true;
		delete_idx_list.sort();

		for (int i = delete_idx_list.size() - 1; i > -1; i--)
		{
			const int type_idx = delete_idx_list[i];
			list_script_types.remove_at(type_idx);
		}

		emit_changed();
	}
}


void FlowScriptNodeTypeDB::queue_process_custom_node_script_delete_queue()
{
	if (custom_node_script_delete_queue.is_empty())
	{
		callable_mp(this, &FlowScriptNodeTypeDB::process_custom_node_script_delete_queue).call_deferred();
	}
}


void FlowScriptNodeTypeDB::init_editor()
{
	DEV_ASSERT(Engine::get_singleton()->is_editor_hint());

	EditorNode::get_singleton()->connect("resource_saved", callable_mp(this, &FlowScriptNodeTypeDB::on_resource_saved));
	FileSystemDock::get_singleton()->connect("resource_removed", callable_mp(this, &FlowScriptNodeTypeDB::on_resource_removed));
	FileSystemDock::get_singleton()->get_script_create_dialog()->connect("script_created", callable_mp(this, &FlowScriptNodeTypeDB::on_script_created));

	refresh_types();
}


void FlowScriptNodeTypeDB::on_resource_saved(const Ref<Resource> &p_resource)
{
	Ref<Script> script = p_resource;

	if (script.is_null())
	{
		return;
	}

	update_type_cache();

	if (cache_map_custom_node_script_to_type_idx.has(script))
	{
		FlowScriptNodeTypeInfo::CreateError type_err;
		const int type_idx = cache_map_custom_node_script_to_type_idx[script];
		const FlowScriptNodeTypeInfo type = FlowScriptNodeTypeInfo::create_from_script(script, type_err);

		list_script_types[type_idx] = type;

		emit_changed();
	}
	else
	{
		if (script->get_instance_base_type() != SNAME("FlowScriptNodeCustom"))
		{
			return;
		}

		const FlowScriptNodeTypeInfo type = FlowScriptNodeTypeInfo::create_from_script_no_check(script);
		add_type(type);
	}
}


void FlowScriptNodeTypeDB::on_resource_removed(const Ref<Resource> &p_resource)
{
	Ref<Script> script = p_resource;

	if (script.is_null() || script->get_instance_base_type() != SNAME("FlowScriptNodeCustom"))
	{
		return;
	}

	queue_process_custom_node_script_delete_queue();
	custom_node_script_delete_queue.push_back(script);
}


void FlowScriptNodeTypeDB::on_script_created(const Ref<Script> &p_script)
{
	if (p_script.is_null() || p_script->get_instance_base_type() != SNAME("FlowScriptNodeCustom"))
	{
		return;
	}

	const FlowScriptNodeTypeInfo type = FlowScriptNodeTypeInfo::create_from_script_no_check(p_script);
	add_type(type);
}


void FlowScriptNodeTypeDB::emit_changed()
{
	emit_signal(CoreStringName(changed));
}


FlowScriptNodeTypeDB::FlowScriptNodeTypeDB()
{
	CRASH_COND_MSG(singleton != nullptr, "FlowScriptNodeTypeDB is a singleton. Do not instantiate it multiple times.");
	singleton = this;
}


FlowScriptNodeTypeDB::~FlowScriptNodeTypeDB()
{
	singleton = nullptr;
}
