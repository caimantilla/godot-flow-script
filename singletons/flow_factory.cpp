#include "flow_factory.hpp"
#include "core/io/resource_loader.h"
#include "flow_type_db.hpp"
#include "scene/resources/packed_scene.h"
#include "core/object/script_language.h"


FlowFactory *FlowFactory::singleton = nullptr;


FlowFactory *FlowFactory::get_singleton()
{
	return singleton;
}





FlowNode *FlowFactory::create_flow_node_of_flow_type(const String &p_type_id)
{
	ERR_FAIL_COND_V(!FlowTypeDB::get_singleton()->has_type(p_type_id), nullptr);

	FlowNode *node = nullptr;
	Ref<FlowType> type = FlowTypeDB::get_singleton()->get_type(p_type_id);

	String node_type_id_str = type->get_node();
	StringName node_type_id_sn = StringName(node_type_id_str);

	if (ClassDB::class_exists(node_type_id_sn)) 
	{
		Object *inst_obj = ClassDB::instantiate(node_type_id_sn);
		node = Object::cast_to<FlowNode>(inst_obj);
	}
	else if (ResourceLoader::exists(node_type_id_str, "Script"))
	{
		Ref<Resource> res = ResourceLoader::load(node_type_id_str, "Script");
		Ref<Script> script = res;
		StringName native_type_name = script->get_instance_base_type();

		Object *inst_obj = ClassDB::instantiate(native_type_name);
		node = Object::cast_to<FlowNode>(inst_obj);
		if (node != nullptr)
		{
			node->set_script(script);
		}
	}

	ERR_FAIL_NULL_V(node, nullptr);

	node->flow_type_id = type->get_id();

	return node;
}


FlowNodeEditor *FlowFactory::create_flow_node_editor_for_flow_type(const String &p_type_id)
{
	ERR_FAIL_COND_V(!FlowTypeDB::get_singleton()->has_type(p_type_id), nullptr);

	FlowNodeEditor *editor = nullptr;

	Ref<FlowType> type = FlowTypeDB::get_singleton()->get_type(p_type_id);
	String editor_str = type->get_editor();
	StringName editor_sn = StringName(editor_str);

	if (ClassDB::class_exists(editor_sn))
	{
		Object *inst_obj = ClassDB::instantiate(editor_sn);
		editor = Object::cast_to<FlowNodeEditor>(inst_obj);
	}
	else if (ResourceLoader::exists(editor_str))
	{
		Ref<Resource> res = ResourceLoader::load(editor_str, "PackedScene");
		Ref<PackedScene> scn = Ref<PackedScene>(Object::cast_to<PackedScene>(*res));
		Ref<Script> script = Ref<Script>(Object::cast_to<Script>(*res));

		if (scn != nullptr && scn->can_instantiate())
		{
			Node *instance = scn->instantiate();
			editor = Object::cast_to<FlowNodeEditor>(instance);

			if (editor == nullptr)
			{
				instance->queue_free();
				ERR_FAIL_V(nullptr);
			}
		}
		else if (script != nullptr)
		{
			String native_type_name = script->get_instance_base_type();
			Object *inst_obj = ClassDB::instantiate(native_type_name);
			editor = Object::cast_to<FlowNodeEditor>(inst_obj);

			ERR_FAIL_NULL_V(editor, nullptr);

			editor->set_script(script);
		}
	}

	return editor;
}


FlowFactory::FlowFactory()
{
	CRASH_COND_MSG(singleton != nullptr, "FlowFactory is a singleton and may not be instantiated multiple times.");
	singleton = this;
}


FlowFactory::~FlowFactory()
{
	singleton = nullptr;
}
