#ifndef FLOW_SCRIPT_NODE_TYPE_DB_HPP
#define FLOW_SCRIPT_NODE_TYPE_DB_HPP


#include "core/string/ustring.h"
#include "core/io/resource.h"
#include "core/object/script_language.h"


class FlowScriptNode;
class FlowScriptNodeEditor;
class FlowScriptNodeTypeInfo;
class FlowScriptEditorPlugin;


class FlowScriptNodeTypeDB final
{
private:
	static FlowScriptNodeTypeDB *singleton;

	HashMap<StringName, int> map_native_class_to_type_idx;
	HashMap<Script *, int> map_custom_node_script_to_type_idx;
	Vector<FlowScriptNodeTypeInfo> native_types;
	List<Ref<Script>> custom_node_script_delete_queue;

	void process_custom_node_script_delete_queue();

	void on_resource_saved(const Ref<Resource> &p_resource);
	void on_resource_removed(const Ref<Resource> &p_resource);
	void on_script_created(const Ref<Script> &p_script);

public:
	static FlowScriptNodeTypeDB *get_singleton();

	void init_editor(FlowScriptEditorPlugin *p_plugin);
	FlowScriptNodeEditor *create_editor_for_node(FlowScriptNode *p_node);

	FlowScriptNodeTypeDB();
	~FlowScriptNodeTypeDB();
};


#endif // FLOW_SCRIPT_NODE_TYPE_DB_HPP
