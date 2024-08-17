#ifndef FLOW_SCRIPT_NODE_TYPE_DB_HPP
#define FLOW_SCRIPT_NODE_TYPE_DB_HPP


#include "core/string/ustring.h"
#include "core/object/object.h"
#include "core/io/resource.h"
#include "core/object/script_language.h"


class FlowScriptNode;
class FlowScriptNodeEditor;
class FlowScriptNodeTypeInfo;
class FlowScriptEditorPlugin;


class FlowScriptNodeTypeDB final : public Object
{
private:
	static FlowScriptNodeTypeDB *singleton;

	Vector<FlowScriptNodeTypeInfo> native_types;
	Vector<FlowScriptNodeTypeInfo> custom_script_types;

	mutable HashMap<StringName, int> map_native_class_to_type_idx;
	mutable bool native_node_info_map_dirty = true;

	mutable HashMap<Ref<Script>, int> map_custom_node_script_to_type_idx;
	mutable bool script_node_info_map_dirty = true;

	bool type_list_changed_notification_queued = false;
	List<Ref<Script>> custom_node_script_delete_queue;

	void emit_changed();

	void refresh_custom_script_types();

	void update_native_node_info_map() const;
	void update_script_node_info_map() const;

	void process_custom_node_script_delete_queue();
	void queue_process_custom_node_script_delete_queue();

	void on_resource_saved(const Ref<Resource> &p_resource);
	void on_resource_removed(const Ref<Resource> &p_resource);
	void on_script_created(const Ref<Script> &p_script);

protected:
	static void _bind_methods();

public:
	static FlowScriptNodeTypeDB *get_singleton();

	void refresh_types();
	void get_node_type_list(List<FlowScriptNodeTypeInfo> *p_list) const;
	void add_type(const FlowScriptNodeTypeInfo &p_type);
	Ref<FlowScriptNode> instantiate_node_for_type(const FlowScriptNodeTypeInfo &p_type);
	const FlowScriptNodeTypeInfo &get_type_of_node(FlowScriptNode *p_node) const;
	FlowScriptNodeEditor *create_editor_for_node(FlowScriptNode *p_node);

	FlowScriptNodeTypeDB();
	~FlowScriptNodeTypeDB();
};


#endif // FLOW_SCRIPT_NODE_TYPE_DB_HPP
