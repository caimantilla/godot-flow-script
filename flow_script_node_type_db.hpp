#ifndef FLOW_SCRIPT_NODE_TYPE_DB_HPP
#define FLOW_SCRIPT_NODE_TYPE_DB_HPP


#include "core/string/ustring.h"
#include "core/io/resource.h"
#include "core/object/script_language.h"


class FlowScriptNode;
class FlowScriptNodeEditor;
class FlowScriptEditorPlugin;


class FlowScriptNodeTypeDB final
{
private:
	static FlowScriptNodeTypeDB *singleton;

public:
	static FlowScriptNodeTypeDB *get_singleton();

	FlowScriptNode *create_node_using_type_id(const String &p_type_id);

	FlowScriptNodeTypeDB();
	~FlowScriptNodeTypeDB();

#ifdef TOOLS_ENABLED

private:
	List<Ref<Script>> custom_node_script_delete_queue;

	void process_custom_node_script_delete_queue();
	
	void on_resource_saved(const Ref<Resource> &p_resource);
	void on_resource_removed(const Ref<Resource> &p_resource);
	void on_script_created(const Ref<Script> &p_script);

public:
	void init_editor(FlowScriptEditorPlugin *p_plugin);
	FlowScriptNodeEditor *create_editor_for_node(FlowScriptNode *p_node);

#endif // TOOLS_ENABLED
};


#endif // FLOW_SCRIPT_NODE_TYPE_DB_HPP
