#ifndef FLOW_SCRIPT_HPP
#define FLOW_SCRIPT_HPP


#include "core/variant/typed_array.h"
#include "core/io/resource.h"
#include "flow_script_typedefs.hpp"
#include "flow_script_constants.hpp"
#include "flow_script_node.hpp"
#include "flow_script_node_output_connection.hpp"
#include "flow_script_node_reference.hpp"
#include "flow_script_connection_info.hpp"


class FlowScript final : public Resource
{
	GDCLASS(FlowScript, Resource);

public:
	enum EncodedConnectionDataIndex
	{
		ENCODED_CONNECTION_FROM_NODE = 0,
		ENCODED_CONNECTION_FROM_LIST = 1,
		ENCODED_CONNECTION_FROM_SLOT = 2,
		ENCODED_CONNECTION_TO_INCLUDE = 3,
		ENCODED_CONNECTION_TO_NODE = 4,
		ENCODED_CONNECTION_MAX,
	};

	enum IncludeAddError
	{
		ERR_INCLUDE_OK = 0,
		ERR_INCLUDE_NULL,
		ERR_INCLUDE_CIRCULAR_REFERENCE,
		ERR_INCLUDE_SELF,
		ERR_INCLUDE_ALREADY_PRESENT,
	};

	enum NodeNameAddError
	{
		ERR_NAME_OK = 0,
		ERR_NAME_TAKEN,
		ERR_NAME_INVALID_IDENTIFIER,
		ERR_NAME_EMPTY,
	};

public:
	struct IncludeInstance final
	{
		Ref<FlowScript> flow_script;
		Point2i position;

		void set_flow_script(const Ref<FlowScript> &p_flow_script);
		Ref<FlowScript> get_flow_script() const;
		void set_position(const Point2i &p_pos);
		Point2i get_position() const;

		bool is_valid() const;

		void set_state(const Dictionary &p_state);
		Dictionary get_state() const;
	};

	struct NodeInstance final
	{
		// Reference to the node resource.
		// This is the only Ref<FlowScriptNode> that needs to exist. Most of the code can use direct node pointers instead.
		Ref<FlowScriptNode> data;
		// Maybe this should be a Rect2 instead?
		Rect2i rect;
		// Connections are stored as a map from a connection slot -> node ID list.
		LocalVector<LocalVector<FlowScriptNodeReference, FlowScriptNodeConnectionListSlotNo>, FlowScriptNodeConnectionListNo> connection_lists;

		void set_data(const Ref<FlowScriptNode> &p_data);
		Ref<FlowScriptNode> get_data() const;
		FlowScriptNode *get_data_ptr() const;
		void set_rect(const Rect2i &p_rect);
		Rect2i get_rect() const;
		void set_position(const Point2i &p_pos);
		Point2i get_position() const;
		void set_size(const Size2i &p_size);
		Size2i get_size() const;

		void set_state(const Dictionary &p_state);
		Dictionary get_state() const;

		bool is_valid() const;
		bool should_save_size() const;
		void clear_connections();
		void set_connection_list_count(const FlowScriptNodeConnectionListNo p_count);
		FlowScriptNodeConnectionListNo get_connection_list_count() const;
		void set_connection_list_length(const FlowScriptNodeConnectionListNo p_list, const FlowScriptNodeConnectionListLength p_length);
		FlowScriptNodeConnectionListLength get_connection_list_length(const FlowScriptNodeConnectionListNo p_list) const;
		bool has_connection(const FlowScriptNodeOutputConnection &p_connection) const;
		void set_connection(const FlowScriptNodeOutputConnection &p_connection, const FlowScriptNodeReference &p_target);
		FlowScriptNodeReference get_connection(const FlowScriptNodeOutputConnection &p_connection) const;
		void set_connection_include_id(const FlowScriptNodeOutputConnection &p_connection, const FlowScriptIncludeID p_include_id);
		FlowScriptIncludeID get_connection_include_id(const FlowScriptNodeOutputConnection &p_connection) const;
		void set_connection_node_id(const FlowScriptNodeOutputConnection &p_connection, const FlowScriptNodeID p_node_id);
		FlowScriptNodeID get_connection_node_id(const FlowScriptNodeOutputConnection &p_connection) const;
	};

private:
	HashMap<FlowScriptIncludeID, IncludeInstance> map_includes; // Public for iteration purposes. Do not modify this directly.
	HashMap<FlowScriptNodeID, NodeInstance> map_nodes; // Public for iteration purposes. Do not modify this directly.
	HashMap<StringName, FlowScriptNodeID> map_node_name_to_id;
	HashMap<FlowScriptNodeID, StringName> map_node_id_to_name;

	mutable bool cache_include_id_list_dirty = false;
	mutable PackedFlowScriptIncludeIDArray cache_include_id_list;
	mutable bool cache_node_id_list_dirty = false;
	mutable PackedFlowScriptNodeIDArray cache_node_id_list;
	mutable bool cache_node_name_list_dirty = false;
	mutable Vector<StringName> cache_node_name_list;
	mutable TypedArray<StringName> cache_node_name_list_bind;

	// NOTE: If node or include IDs are changed to be 64-bit, this must be changed to a PackedInt64Array.
	// How is it encoded?
	// Each connection is composed of 8 numbers:
	// From Node ID, From List No., From Slot No., Target Include ID, Target Node ID, Target List No., Target Slot No., Dummy.
	// Currently, Target List No. and Target Slot No. are unused and will always be 0. However, they are included for future-proofing.
	// The same applies to the "dummy" number.
	mutable Vector<FlowScriptConnectionInfo> cache_node_connection_list;
	mutable TypedArray<Dictionary> cache_node_connection_list_bind;
	mutable PackedInt32Array cache_encoded_node_connection_list;
	mutable bool cache_node_connections_dirty = false;

	static void internal_get_every_node_resource_recursive(const FlowScript *p_current_level, List<Ref<FlowScriptNode>> *p_node_list);
	static void internal_get_every_node_resource_connected_to_recursive(const FlowScript *p_current_script, const FlowScriptNodeID p_current_origin_node_id, List<Ref<FlowScriptNode>> *p_node_list);

	void update_cache_include_id_list() const;
	void update_cache_node_id_list() const;
	void update_cache_node_name_list() const;
	void update_cache_node_connections() const;

	void update_connection_outputs_for_node(const FlowScriptNodeID p_node_id);
	void internal_clear_node_name(const FlowScriptNodeID p_node_id, const bool p_emit);

	void on_node_changed(FlowScriptNodeID p_node_id);
	void on_include_changed(FlowScriptIncludeID p_include_id);

	void bind_set_node_connection(const FlowScriptNodeID p_from_node_id, const FlowScriptNodeConnectionListNo p_list, const FlowScriptNodeConnectionListSlotNo p_slot, const FlowScriptIncludeID p_to_include_id, const FlowScriptNodeID p_to_node_id);
	Dictionary bind_get_node_connection(const FlowScriptNodeID p_from_node_id, const FlowScriptNodeConnectionListNo p_list, const FlowScriptNodeConnectionListSlotNo p_slot) const;
	TypedArray<FlowScriptNode> bind_get_every_node_resource_recursive() const;
	TypedArray<FlowScriptNode> bind_get_every_node_resource_connected_to_node(const FlowScriptNodeID p_origin_node_id, const bool p_include_origin = true) const;
	void bind_clear_references_to_node(const FlowScriptIncludeID p_include_id, const FlowScriptNodeID p_node_id);
	bool bind_has_target(const FlowScriptIncludeID p_include_id, const FlowScriptNodeID p_node_id) const;
	TypedArray<StringName> bind_get_node_name_list() const;
	bool bind_has_node_connection(const Dictionary &p_connection_info) const;
	void bind_add_node_connection(const Dictionary &p_connection_info);
	void bind_remove_node_connection(const FlowScriptNodeID p_from_node_id, const FlowScriptNodeConnectionListNo p_list, const FlowScriptNodeConnectionListSlotNo p_slot);
	void bind_set_node_connection_list(const TypedArray<Dictionary> &p_connections);
	TypedArray<Dictionary> bind_get_node_connection_list() const;
	TypedArray<Dictionary> bind_get_connections_to_target(const FlowScriptIncludeID p_include_id, const FlowScriptNodeID p_node_id) const;

protected:
	static void _bind_methods();
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	virtual void _resource_path_changed() override;

public:
	void clear_references_to_include(const FlowScriptIncludeID p_include_id);
	void clear_references_to_node(const FlowScriptNodeReference &p_target);
	bool has_target(const FlowScriptNodeReference &p_target) const;
	FlowScriptIncludeID get_include_count() const;
	bool contains_include_flow_script_recursive(const Ref<FlowScript> &p_flow_script) const;
	bool can_include_flow_script(const Ref<FlowScript> &p_flow_script) const; // DO NOT ALLOW CIRCULAR DEPENDENCIES.
	IncludeAddError get_can_include_flow_script_error(const Ref<FlowScript> &p_flow_const) const; // AGAIN, DO NOT ALLOW CIRCULAR DEPENDENCIES.
	bool has_include(const FlowScriptIncludeID p_include_id) const;
	FlowScriptIncludeID get_first_available_include_slot(const FlowScriptIncludeID p_from = FlowScriptConstants::INCLUDE_ID_MIN) const;
	FlowScriptIncludeID add_include_to_first_available_slot(const Ref<FlowScript> &p_flow_script);
	bool remove_include_list(const PackedFlowScriptIncludeIDArray &p_include_id_list);
	bool remove_include(const FlowScriptIncludeID p_include_id);
	IncludeInstance &get_include_instance(const FlowScriptIncludeID p_include_id);
	const IncludeInstance &get_include_instance_const(const FlowScriptIncludeID p_include_id) const;
	FlowScriptNodeID get_node_count() const;
	bool is_node_slot_available(const FlowScriptNodeID p_node_id) const;
	bool has_node(const FlowScriptNodeID p_node_id) const;
	NodeInstance &get_node_instance(const FlowScriptNodeID p_node_id);
	const NodeInstance &get_node_instance_const(const FlowScriptNodeID p_node_id) const;
	bool can_add_node_data(const Ref<FlowScriptNode> &p_data) const;
	void set_node_data(const FlowScriptNodeID p_node_id, const Ref<FlowScriptNode> &p_data);
	Ref<FlowScriptNode> get_node_data(const FlowScriptNodeID p_node_id) const;
	FlowScriptNode *get_node_data_ptr(const FlowScriptNodeID p_node_id) const;
	void set_node_name(const FlowScriptNodeID p_node_id, const StringName &p_name);
	StringName get_node_name(const FlowScriptNodeID p_node_id) const;
	void set_node_rect(const FlowScriptNodeID p_node_id, const Rect2i &p_rect);
	Rect2i get_node_rect(const FlowScriptNodeID p_node_id) const;
	void set_node_position(const FlowScriptNodeID p_node_id, const Point2i &p_position);
	Point2i get_node_position(const FlowScriptNodeID p_node_id) const;
	void set_node_size(const FlowScriptNodeID p_node_id, const Size2i &p_size);
	Size2i get_node_size(const FlowScriptNodeID p_node_id) const;
	FlowScriptNodeConnectionListNo get_node_connection_list_count(const FlowScriptNodeID p_node_id) const;
	FlowScriptNodeConnectionListLength get_node_connection_list_length(const FlowScriptNodeID p_node_id, const FlowScriptNodeConnectionListNo p_list) const;
	void set_node_connection(const FlowScriptNodeID p_node_id, const FlowScriptNodeOutputConnection &p_connection, const FlowScriptNodeReference &p_target_node);
	FlowScriptNodeReference get_node_connection(const FlowScriptNodeID p_node_id, const FlowScriptNodeOutputConnection &p_connection) const;
	void remove_node_connection(const FlowScriptNodeID p_node_id, const FlowScriptNodeOutputConnection &p_connection);
	bool insert_node_copy(const FlowScriptNodeID p_node_id, const NodeInstance p_node_copy);
	NodeInstance create_node_copy(const FlowScriptNodeID p_node_id);
	bool remove_node_list(const PackedFlowScriptNodeIDArray &p_node_id_list);
	bool remove_node(const FlowScriptNodeID p_node_id);
	FlowScriptNodeID get_first_available_node_slot(const FlowScriptNodeID p_from = FlowScriptConstants::NODE_ID_MIN) const;
	FlowScriptNodeID add_node_to_first_available_slot(const Ref<FlowScriptNode> &p_data);
	void set_include_flow_script(const FlowScriptIncludeID p_include_id, const Ref<FlowScript> &p_flow_script);
	Ref<FlowScript> get_include_flow_script(const FlowScriptIncludeID p_include_id) const;
	void set_include_position(const FlowScriptIncludeID p_include_id, const Point2i &p_position);
	Point2i get_include_position(const FlowScriptIncludeID p_include_id) const;
	void get_every_node_resource_recursive(List<Ref<FlowScriptNode>> *p_list) const;
	void get_every_node_resource_connected_to_node(List<Ref<FlowScriptNode>> *p_list, const FlowScriptNodeID p_origin_node_id, const bool p_include_origin) const;
	PackedFlowScriptIncludeIDArray get_include_id_list() const;
	PackedFlowScriptNodeIDArray get_node_id_list() const;
	Vector<StringName> get_node_name_list() const;
	FlowScriptNodeID get_node_id_by_name(const StringName &p_name) const;
	bool has_node_with_name(const StringName &p_name) const;
	bool can_add_node_name(const StringName &p_name) const;
	NodeNameAddError get_can_add_node_name_error(const StringName &p_name) const;
	void clear_node_name(const FlowScriptNodeID p_node_id);
	void clear_all_node_connections();
	bool has_node_connection(const FlowScriptConnectionInfo &p_connection_info) const;
	void add_node_connection(const FlowScriptConnectionInfo &p_connection_info);
	void set_node_connection_list(const Vector<FlowScriptConnectionInfo> &p_connections);
	Vector<FlowScriptConnectionInfo> get_node_connection_list() const;
	void set_encoded_node_connection_list(const PackedInt32Array &p_connections);
	PackedInt32Array get_encoded_node_connection_list() const;
	void get_connections_to_target(const FlowScriptNodeReference &p_target, List<FlowScriptConnectionInfo> *p_connection_list) const;

	FlowScript();
};


VARIANT_ENUM_CAST(FlowScript::EncodedConnectionDataIndex);
VARIANT_ENUM_CAST(FlowScript::IncludeAddError);
VARIANT_ENUM_CAST(FlowScript::NodeNameAddError);


#endif // FLOW_SCRIPT_HPP
