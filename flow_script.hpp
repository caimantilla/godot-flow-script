#ifndef FLOW_SCRIPT_HPP
#define FLOW_SCRIPT_HPP


#include "core/templates/local_vector.h"
#include "core/io/resource.h"
#include "typedefs.hpp"
#include "flow_script_include_instance.hpp"
#include "flow_script_node_output_connection.hpp"


class FlowScriptNode;
class FlowScriptNodeInstance;
class FlowScriptNodeReference;


class FlowScript final : public Resource
{
	GDCLASS(FlowScript, Resource);

public:
	enum : FlowScriptNodeID
	{
		NODE_ID_INVALID = -1,
		NODE_ID_MIN = 1,
		NODE_ID_MAX = 9999,
	};
	enum : FlowScriptIncludeID
	{
		INCLUDE_FLOW_SCRIPT_ID_INVALID = -1,
		INCLUDE_FLOW_SCRIPT_MAX = 20,
	};

private:
	FlowScriptIncludeInstance script_includes[INCLUDE_FLOW_SCRIPT_MAX];
	mutable Vector<FlowScriptNodeID> cache_node_id_array;
	Vector<FlowScriptNodeID> *cache_node_id_array_ptr;
	mutable bool cache_node_id_array_dirty = true;
	mutable bool cache_next_available_node_id_dirty = true;
	mutable FlowScriptNodeID cache_next_available_node_id = NODE_ID_MIN;

	void update_cache_next_available_node_id() const;
	void update_connection_outputs_for_node(FlowScriptNodeID p_node_id);
	void init_node(FlowScriptNodeID p_node_id);
	void uninit_node(FlowScriptNodeID p_node_id);
	void on_node_changed(FlowScriptNodeID p_node_id);

protected:
	static void _bind_methods();
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;

public:
	HashMap<FlowScriptNodeID, FlowScriptNodeInstance> node_map;

	bool includes_flow_script(const Ref<FlowScript> &p_other_flow_script) const; // DO NOT ALLOW CIRCULAR DEPENDENCIES.
	bool has_include_flow_script_instance(const FlowScriptIncludeID p_id) const;
	FlowScriptIncludeInstance get_include_flow_script_instance(const FlowScriptIncludeID p_id) const;
	bool is_node_slot_available(const FlowScriptNodeID p_node_id) const;
	bool has_node(const FlowScriptNodeID p_node_id) const;
	Ref<FlowScriptNode> get_node_ref(const FlowScriptNodeID p_node_id) const;
	FlowScriptNode *get_node_ptr(const FlowScriptNodeID p_node_id) const;
	void set_node_position(const FlowScriptNodeID p_node_id, const Point2i &p_position);
	Point2i get_node_position(const FlowScriptNodeID p_node_id) const;
	void set_node_connection_list_count(const FlowScriptNodeID, const uint8_t p_count);
	uint8_t get_node_connection_list_count(const FlowScriptNodeID p_node_id) const;
	void set_node_connection_list_length(const FlowScriptNodeID p_node_id, const uint8_t p_list, const int64_t p_length);
	int64_t get_node_connection_list_length(const FlowScriptNodeID p_node_id, const uint8_t p_list) const;
	void set_node_connection(const FlowScriptNodeID p_node_id, const FlowScriptNodeOutputConnection &p_connection, const FlowScriptNodeReference p_target_node);
	FlowScriptNodeReference get_node_connection(const FlowScriptNodeID p_node_id, const FlowScriptNodeOutputConnection &p_connection) const;
	bool copy_node_instance_to(const FlowScriptNodeID p_target_node_id, const FlowScriptNodeInstance &p_node_instance);
	FlowScriptNodeInstance get_copy_of_node_instance(const FlowScriptNodeID p_node_id) const;
	bool remove_node_list(const List<FlowScriptNodeID> &p_node_id_list);
	bool remove_node(const FlowScriptNodeID p_node_id);
	FlowScriptNodeID get_node_id_by_name(const String &p_node_name) const;
	FlowScriptNodeID add_node_to_first_available_slot(const Ref<FlowScriptNode> &p_node);
	void set_include_flow_script(const FlowScriptIncludeID p_include_id, const Ref<FlowScript> &p_flow_script);
	Ref<FlowScript> get_include_flow_script(const FlowScriptIncludeID p_include_id) const;
	void set_include_flow_script_position(const FlowScriptIncludeID p_include_id, const Point2i &p_position);
	Point2i get_include_flow_script_position(const FlowScriptIncludeID p_include_id) const;

	FlowScript();
};


#endif // FLOW_SCRIPT_HPP
