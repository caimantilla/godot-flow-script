#ifndef FLOW_SCRIPT_NODE_TYPE_DB_HPP
#define FLOW_SCRIPT_NODE_TYPE_DB_HPP


#include "flow_script_node_type_info.hpp"
#include "../flow_script_node.hpp"
#include "core/string/ustring.h"
#include "core/object/object.h"
#include "core/object/class_db.h"
#include "core/object/script_language.h"
#include "core/io/resource.h"


class FlowScriptNodeEditor;


class FlowScriptNodeTypeDB final : public Object
{
	GDCLASS(FlowScriptNodeTypeDB, Object);

private:
	static FlowScriptNodeTypeDB *singleton;

	LocalVector<FlowScriptNodeTypeInfo> list_native_types;
	LocalVector<FlowScriptNodeTypeInfo> list_script_types;

	mutable bool has_ever_refreshed_types = false;
	mutable bool cache_types_dirty = true;
	mutable Vector<FlowScriptNodeTypeInfo> cache_complete_type_list;
	mutable TypedArray<Dictionary> cache_complete_type_list_bind;
	mutable HashMap<StringName, int> cache_map_native_class_to_type_idx;
	mutable HashMap<Ref<Script>, int> cache_map_custom_node_script_to_type_idx;
	mutable HashMap<StringName, int> cache_map_custom_node_script_class_name_to_type_idx;

	bool type_list_changed_notification_queued = false;
	List<Ref<Script>> custom_node_script_delete_queue;

	void emit_changed();
	void update_type_cache() const;
	void process_custom_node_script_delete_queue();
	void queue_process_custom_node_script_delete_queue();

	int bind_get_type_count() const;
	TypedArray<Dictionary> bind_get_node_type_list() const;
	Dictionary bind_get_type_by_index(const int p_index) const;

protected:
	static void _bind_methods();

	TypedArray<Dictionary> get_node_type_list_bind() const;

public:
	static FlowScriptNodeTypeDB *get_singleton();

	void init_editor(); // Called by the FlowScript editor plugin...
	void refresh_types();
	int get_type_count() const;
	Vector<FlowScriptNodeTypeInfo> get_node_type_list() const;
	FlowScriptNodeTypeInfo get_type_by_class_name(const StringName &p_class_name) const;
	FlowScriptNodeTypeInfo get_type_by_index(const int p_index) const;
	void add_type(FlowScriptNodeTypeInfo p_type);
	Ref<FlowScriptNode> instantiate_node_for_type(const FlowScriptNodeTypeInfo &p_type);
	FlowScriptNodeEditor *instantiate_editor_for_type(const FlowScriptNodeTypeInfo &p_type);
	FlowScriptNodeTypeInfo get_type_of_node(const Ref<FlowScriptNode> &p_node) const;
	FlowScriptNodeEditor *create_editor_for_node(const Ref<FlowScriptNode> &p_node);

	FlowScriptNodeTypeDB();
	~FlowScriptNodeTypeDB();

private:
	void on_resource_saved(const Ref<Resource> &p_resource);
	void on_resource_removed(const Ref<Resource> &p_resource);
	void on_script_created(const Ref<Script> &p_script);
};


#endif // FLOW_SCRIPT_NODE_TYPE_DB_HPP
