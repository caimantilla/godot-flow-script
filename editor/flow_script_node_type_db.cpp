#include "flow_script_node_type_db.hpp"
#include "flow_script_node_type_info.hpp"
#include "../flow_script_node.hpp"
#include "../flow_script_node_custom.hpp"
#include "plugins/flow_script_editor_plugin.hpp"
#include "nodes/flow_script_node_editor.hpp"
#include "core/error/error_macros.h"
#include "core/object/script_language.h"
#include "scene/resources/packed_scene.h"
#include "editor/editor_interface.h"
#include "editor/editor_node.h"
#include "editor/filesystem_dock.h"


FlowScriptNodeTypeDB *FlowScriptNodeTypeDB::singleton = nullptr;


void FlowScriptNodeTypeDB::_bind_methods()
{
	ADD_SIGNAL(MethodInfo("changed"));
}


void FlowScriptNodeTypeDB::_notification(int p_what)
{
	if (p_what == NOTIFICATION_READY)
	{
		EditorNode::get_singleton()->connect("resource_saved", callable_mp(this, &FlowScriptNodeTypeDB::on_resource_saved));
		FileSystemDock::get_singleton()->connect("resource_removed", callable_mp(this, &FlowScriptNodeTypeDB::on_resource_removed));
		FileSystemDock::get_singleton()->get_script_create_dialog()->connect("script_created", callable_mp(this, &FlowScriptNodeTypeDB::on_script_created));

		refresh_custom_script_types();
	}
}


FlowScriptNodeTypeDB *FlowScriptNodeTypeDB::get_singleton()
{
	return singleton;
}


void FlowScriptNodeTypeDB::refresh_types()
{
	refresh_custom_script_types();
}


void FlowScriptNodeTypeDB::get_node_type_list(List<FlowScriptNodeTypeInfo> *p_list) const
{
	for (const FlowScriptNodeTypeInfo &native_info : native_types)
	{
		p_list->push_back(native_info);
	}
	for (const FlowScriptNodeTypeInfo &script_info : custom_script_types)
	{
		if (script_info.enabled)
		{
			p_list->push_back(script_info);
		}
	}
}


void FlowScriptNodeTypeDB::add_type(const FlowScriptNodeTypeInfo &p_type)
{
	ERR_FAIL_COND(!p_type.native);
	native_types.push_back(p_type);
	native_node_info_map_dirty = true;
	emit_changed();
}


Ref<FlowScriptNode> FlowScriptNodeTypeDB::instantiate_node_for_type(const FlowScriptNodeTypeInfo &p_type)
{
	ERR_FAIL_COND_V(!p_type.enabled, Ref<FlowScriptNode>());
	Object *obj = ClassDB::instantiate(p_type.node_class);
	ERR_FAIL_NULL_V(obj, Ref<FlowScriptNode>());
	FlowScriptNode *node_ptr = Object::cast_to<FlowScriptNode>(obj);
	if (node_ptr == nullptr)
	{
		memdelete(obj);
		ERR_FAIL_V(Ref<FlowScriptNode>());
	}
	Ref<FlowScriptNode> node_ref = Ref<FlowScriptNode>(node_ptr);
	if (p_type.node_script.is_valid())
	{
		node_ref->set_script(p_type.node_script);
	}
	return node_ref;
}


const FlowScriptNodeTypeInfo &FlowScriptNodeTypeDB::get_type_of_node(FlowScriptNode *p_node) const
{
	ERR_FAIL_NULL_V(p_node, dummy_type_info);

	update_native_node_info_map();
	update_script_node_info_map();

	Ref<Script> script = p_node->get_script();
	FlowScriptNodeCustom *custom_node = Object::cast_to<FlowScriptNodeCustom>(p_node);

	if (custom_node == nullptr || !script.is_valid())
	{
		StringName native_class = p_node->get_class_name();
		ERR_FAIL_COND_V(!map_native_class_to_type_idx.has(native_class), dummy_type_info);
		int idx = map_native_class_to_type_idx[native_class];
		return native_types[idx];
	}
	else
	{
		ERR_FAIL_COND_V(!map_custom_node_script_to_type_idx.has(script), dummy_type_info);
		int idx = map_custom_node_script_to_type_idx[script];
		return custom_script_types[idx];
	}
}


FlowScriptNodeEditor *FlowScriptNodeTypeDB::create_editor_for_node(FlowScriptNode *p_node)
{
	ERR_FAIL_NULL_V(p_node, nullptr);

	update_native_node_info_map();
	update_script_node_info_map();

	FlowScriptNodeCustom *custom_node = Object::cast_to<FlowScriptNodeCustom>(p_node);
	if (custom_node == nullptr)
	{
		StringName node_class_name = p_node->get_class_name();
		if (!map_native_class_to_type_idx.has(node_class_name))
		{
			CRASH_NOW_MSG(vformat(TTR("Invalid FlowScriptNode class: \"%s\""), node_class_name));
			return nullptr;
		}
		int idx = map_native_class_to_type_idx[node_class_name];
		Object *editor_obj = ClassDB::instantiate(native_types[idx].editor_class);
		FlowScriptNodeEditor *editor = Object::cast_to<FlowScriptNodeEditor>(editor_obj);
		CRASH_COND_MSG(editor == nullptr, vformat(TTR("Failed to instantiate editor for FlowScriptNode class: \"%s\""), node_class_name));
		return editor;
	}
	else
	{
		Ref<Script> script = custom_node->get_script();
		ERR_FAIL_COND_V(!script.is_valid(), nullptr);
		ERR_FAIL_COND_V(!map_custom_node_script_to_type_idx.has(script), nullptr);
		int idx = map_custom_node_script_to_type_idx[script];
		if (custom_script_types[idx].editor_scene.is_valid() && custom_script_types[idx].editor_scene->can_instantiate())
		{
			Node *instance = custom_script_types[idx].editor_scene->instantiate();
			ERR_FAIL_NULL_V(instance, nullptr);
			FlowScriptNodeEditor *editor = Object::cast_to<FlowScriptNodeEditor>(instance);
			if (editor == nullptr)
			{
				memdelete(instance);
				ERR_FAIL_V(nullptr);
			}
			else
			{
				return editor;
			}
		}
		else if (custom_script_types[idx].editor_script->is_valid())
		{
			FlowScriptNodeEditor *editor = memnew(FlowScriptNodeEditor);
			editor->set_script(custom_script_types[idx].editor_script);
			return editor;
		}
		else
		{
			ERR_FAIL_V(nullptr);
		}
	}
}


void FlowScriptNodeTypeDB::refresh_custom_script_types()
{
	script_node_info_map_dirty = true;
	custom_script_types.clear();

	List<StringName> global_script_name_list;
	ScriptServer::get_global_class_list(&global_script_name_list);

	for (const StringName &global_script_sn : global_script_name_list)
	{
		String global_script_str = global_script_sn;
		if (ScriptServer::get_global_class_native_base(global_script_str) != SNAME("FlowScriptNodeCustom"))
		{
			continue;
		}
		String path = ScriptServer::get_global_class_path(global_script_str);
		ERR_CONTINUE(!ResourceLoader::exists(path, "Script"));
		Ref<Script> script = ResourceLoader::load(path, "Script");
		ERR_CONTINUE(!script.is_valid());
		FlowScriptNodeTypeInfo::ScriptCreateResult create_result = FlowScriptNodeTypeInfo::create_script_type(script);
		if (create_result.error != FlowScriptNodeTypeInfo::ScriptCreateResult::OK)
		{
			continue;
		}
		custom_script_types.push_back(create_result.type);
	}
	emit_changed();
}


void FlowScriptNodeTypeDB::update_native_node_info_map() const
{
	if (!native_node_info_map_dirty)
	{
		return;
	}
	native_node_info_map_dirty = false;
	map_native_class_to_type_idx.clear();
	for (int i = 0; i < native_types.size(); i++)
	{
		map_native_class_to_type_idx.insert(native_types[i].node_class, i);
	}
}


void FlowScriptNodeTypeDB::update_script_node_info_map() const
{
	if (!script_node_info_map_dirty)
	{
		return;
	}
	script_node_info_map_dirty = false;
	map_custom_node_script_to_type_idx.clear();
	for (int i = 0; i < custom_script_types.size(); i++)
	{
		map_custom_node_script_to_type_idx.insert(custom_script_types[i].node_script, i);
	}
}


void FlowScriptNodeTypeDB::process_custom_node_script_delete_queue()
{
	// i don't feel like writing this code better so basically resize the type idx vector all zeroed
	// and store the type idx as +1 to accommodate that
	// when actually removing the types just subtract 1 for the actual type idx
	update_script_node_info_map();
	Vector<int> delete_idx_list;
	delete_idx_list.resize_zeroed(custom_node_script_delete_queue.size());
	int script_ok_counter = 0;
	for (const Ref<Script> &script : custom_node_script_delete_queue)
	{
		if (map_custom_node_script_to_type_idx.has(script))
		{
			delete_idx_list.write[script_ok_counter] = map_custom_node_script_to_type_idx[script] + 1;
			script_ok_counter++;
		}
	}
	custom_node_script_delete_queue.clear();
	if (!delete_idx_list.is_empty())
	{
		script_node_info_map_dirty = true;
		delete_idx_list.sort();
		bool any_deleted = false;
		for (int i = delete_idx_list.size() - 1; i > -1; i--)
		{
			int type_idx = delete_idx_list[i] - 1;
			if (type_idx != -1)
			{
				custom_script_types.remove_at(delete_idx_list[i] - 1);
				any_deleted = true;
			}
		}
		if (any_deleted)
		{
			emit_changed();
		}
	}
}


void FlowScriptNodeTypeDB::queue_process_custom_node_script_delete_queue()
{
	if (custom_node_script_delete_queue.is_empty())
	{
		callable_mp(this, &FlowScriptNodeTypeDB::process_custom_node_script_delete_queue).call_deferred();
	}
}


void FlowScriptNodeTypeDB::on_resource_saved(const Ref<Resource> &p_resource)
{
	Ref<Script> script = p_resource;
	if (!script.is_valid())
	{
		return;
	}
	update_script_node_info_map();
	if (map_custom_node_script_to_type_idx.has(script))
	{
		FlowScriptNodeTypeInfo::ScriptCreateResult create_result = FlowScriptNodeTypeInfo::create_script_type(script);
		custom_script_types.write[map_custom_node_script_to_type_idx[script]] = create_result.type;
		emit_changed();
	}
	else
	{
		if (script->get_instance_base_type() != SNAME("FlowScriptNodeCustom"))
		{
			return;
		}
		script_node_info_map_dirty = true;
		FlowScriptNodeTypeInfo::ScriptCreateResult create_result = FlowScriptNodeTypeInfo::create_script_type(script);
		custom_script_types.push_back(create_result.type);
		emit_changed();
	}
}


void FlowScriptNodeTypeDB::on_resource_removed(const Ref<Resource> &p_resource)
{
	Ref<Script> script = p_resource;
	if (!script.is_valid() || script->get_instance_base_type() != SNAME("FlowScriptNodeCustom"))
	{
		return;
	}
	queue_process_custom_node_script_delete_queue();
	custom_node_script_delete_queue.push_back(script);
}


void FlowScriptNodeTypeDB::on_script_created(const Ref<Script> &p_script)
{
	if (!p_script.is_valid() || p_script->get_instance_base_type() != SNAME("FlowScriptNodeCustom"))
	{
		return;
	}
	script_node_info_map_dirty = true;
	FlowScriptNodeTypeInfo::ScriptCreateResult create_result = FlowScriptNodeTypeInfo::create_script_type(p_script);
	custom_script_types.push_back(create_result.type);
	emit_changed();
}


void FlowScriptNodeTypeDB::emit_changed()
{
	emit_signal(CoreStringName(changed));
}


FlowScriptNodeTypeDB::FlowScriptNodeTypeDB()
{
	CRASH_COND_MSG(singleton != nullptr, "FlowScriptNodeTypeDB is a singleton. Do not instantiate it multiple times.");
	singleton = this;

	add_type(FlowScriptNodeTypeInfo::create_native_type("procedure", "FlowScriptNodeProcedure", "FlowScriptNodeEditorProcedure", true, "Procedure", "", "A named entrypoint into the FlowScript."));
	add_type(FlowScriptNodeTypeInfo::create_native_type("text_comment", "FlowScriptNodeTextComment", "FlowScriptNodeEditorTextComment", false, "Comment", "", "A box to take notes in."));

	add_type(FlowScriptNodeTypeInfo::create_native_type("return_expression_result", "FlowScriptNodeReturnExpressionResult", "FlowScriptNodeEditorReturnExpressionResult", false, "Evaluate and Return Expression", "Logic", "Returns the result of an expression evaluation to the caller."));
	add_type(FlowScriptNodeTypeInfo::create_native_type("loop_while_expression_result_true", "FlowScriptNodeLoopWhileExpressionResultTrue", "FlowScriptNodeEditorLoopWhileExpressionResultTrue", false, "While Loop Expression", "Logic/Loops", "Loops while an expression result is true."));
	add_type(FlowScriptNodeTypeInfo::create_native_type("boolean_branch_expression", "FlowScriptNodeBooleanBranchExpression", "FlowScriptNodeEditorBooleanBranchExpression", false, "Branch Expression", "Logic/Branching", "Branches based on the result of a list of expression evaluations."));

	add_type(FlowScriptNodeTypeInfo::create_native_type("multi_branch_execute_sequential", "FlowScriptNodeMultiBranchExecuteSequential", "FlowScriptNodeEditorMultiBranchExecute", false, "Execute Sequential Branches", "Concurrency", "Executes a list of branches in order, then advances."));
	add_type(FlowScriptNodeTypeInfo::create_native_type("multi_branch_execute_parallel", "FlowScriptNodeMultiBranchExecuteParallel", "FlowScriptNodeEditorMultiBranchExecute", false, "Execute Parallel Branches", "Concurrency", "Triggers execution of a list of branches all at once, then advances once all the branches have finished execution."));

	add_type(FlowScriptNodeTypeInfo::create_native_type("set_expression_result_to_variable_local", "FlowScriptNodeSetExpressionResultToVariableLocal", "FlowScriptNodeEditorSetExpressionResultToVariable", false, "Assign Local Variable", "Variables", "Evaluates an expression, then assigns the result to a local variable."));
	add_type(FlowScriptNodeTypeInfo::create_native_type("set_expression_result_to_variable_global", "FlowScriptNodeSetExpressionResultToVariableGlobal", "FlowScriptNodeEditorSetExpressionResultToVariable", false, "Assign Global Variable", "Variables", "Evaluates an expression, then assigns the result to a global variable."));

	add_type(FlowScriptNodeTypeInfo::create_native_type("wait_duration_fixed_seconds", "FlowScriptNodeWaitDurationFixedSeconds", "FlowScriptNodeEditorWaitDurationFixedSeconds", false, "Wait Fixed Seconds", "Timing", "Waits a defined amount of time."));
	add_type(FlowScriptNodeTypeInfo::create_native_type("wait_duration_expression_result", "FlowScriptNodeWaitDurationExpressionResult", "FlowScriptNodeEditorWaitDurationExpressionResult", false, "Wait Expression", "Timing", "Waits the number of seconds evaluated from an expression."));
}


FlowScriptNodeTypeDB::~FlowScriptNodeTypeDB()
{
	singleton = nullptr;
}
