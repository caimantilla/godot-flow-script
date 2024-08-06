#ifndef FLOW_SCRIPT_HPP
#define FLOW_SCRIPT_HPP


#include "core/templates/local_vector.h"
#include "core/io/resource.h"
#include "typedefs.hpp"


class FlowScriptNode;
class FlowScriptNodeInstance;


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

private:
	mutable PackedInt32Array cache_connection_data;
	mutable bool cache_connection_data_dirty = true;
	mutable Vector<FlowScriptNodeID> cache_node_id_array;
	Vector<FlowScriptNodeID> *cache_node_id_array_ptr;
	mutable bool cache_node_id_array_dirty = true;
	mutable bool cache_next_available_node_id_dirty = true;
	mutable FlowScriptNodeID cache_next_available_node_id = NODE_ID_MIN;

	void update_cache_connection_data() const;
	void update_cache_node_id_array() const;
	void update_cache_next_available_node_id() const;
	PackedInt32Array bind_get_node_ids() const;

protected:
	static void _bind_methods();
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;

public:
	HashMap<FlowScriptNodeID, FlowScriptNodeInstance> node_map;

	void set_graph_connection_data(const PackedInt32Array &p_data);
	PackedInt32Array get_graph_connection_data() const;

	bool is_node_slot_available(const FlowScriptNodeID p_node_id) const;
	Vector<FlowScriptNodeID> *get_node_id_list() const;
	bool has_node(const FlowScriptNodeID p_node_id) const;
	Ref<FlowScriptNode> get_node_ref(const FlowScriptNodeID p_node_id) const;
	FlowScriptNode *get_node_ptr(const FlowScriptNodeID p_node_id) const;
	void set_node_position(const FlowScriptNodeID p_node_id, const Point2i &p_position);
	Point2i get_node_position(const FlowScriptNodeID p_node_id) const;
	void set_node_connection_list_length(const FlowScriptNodeID p_node_id, const uint8_t p_list, const int64_t p_length);
	int64_t get_node_connection_list_length(const FlowScriptNodeID p_node_id, const uint8_t p_list) const;
	void set_node_connection(const FlowScriptNodeID p_node_id, const uint8_t p_list, const int64_t p_slot, const FlowScriptNodeID p_target_node_id);
	FlowScriptNodeID get_node_connection(const FlowScriptNodeID p_node_id, const uint8_t p_list, const int64_t p_slot) const;
	bool copy_node_instance_to(const FlowScriptNodeID p_target_node_id, const FlowScriptNodeInstance &p_node_instance);
	FlowScriptNodeInstance get_copy_of_node_instance(const FlowScriptNodeID p_node_id) const;
	bool remove_node(const FlowScriptNodeID p_node_id);
	FlowScriptNodeID get_node_id_by_name(const String &p_node_name) const;
	FlowScriptNodeID add_node_to_first_available_slot(const Ref<FlowScriptNode> &p_node);

	FlowScript();
};


#endif // FLOW_SCRIPT_HPP
