#ifndef FLOW_SCRIPT_HPP
#define FLOW_SCRIPT_HPP

#include "scene/main/node.h"
#include "flow_node.hpp"
#include "../utilities/random_id_generator.hpp"

class FlowScript final : public Node
{
	GDCLASS(FlowScript, Node);

	friend class FlowFiber;
	friend class FlowController;
	friend class FlowScriptEditorGraph;

private:
	HashMap<FlowNodeID, FlowNode *> flow_node_map;
	FlowNodeID next_flow_node_id = FLOW_NODE_ID_MIN;
	
	void on_flow_node_property_list_changed();
	void on_flow_node_list_changed(const bool p_emit);
	void on_flow_node_id_changed(const FlowNodeID p_old_id, const FlowNodeID p_new_id, const bool p_emit);
	void clear_references_to_flow_node(const FlowNodeID p_flow_node_id);
	void clear_references_to_multiple_flow_nodes(const FlowNodeIDArray &p_flow_node_ids);
	bool parse_flow_node_property(const StringName &p_in_property, StringName &p_out_property, FlowNodeID &p_out_flow_node_id) const;
	bool update_next_flow_node_id();

	FlowNode *_create_new_flow_node(const FlowNodeID p_flow_node_id, const String &p_flow_type_id, const bool p_emit);
	bool _remove_flow_node(const FlowNodeID p_flow_node_id, const bool p_emit);
	bool _change_flow_node_id(const FlowNodeID p_from, const FlowNodeID p_to, bool p_emit);
	FlowNodeIDArray _remove_multiple_flow_nodes(const FlowNodeIDArray &p_flow_node_ids, const bool p_emit);

protected:
	static void _bind_methods();

	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	bool _property_can_revert(const StringName &p_name) const;
	bool _property_get_revert(const StringName &p_name, Variant &r_property) const;

public:
	void clear();

	bool is_new_flow_node_id_valid(const FlowNodeID p_new_id) const;
	bool has_flow_node(const FlowNodeID p_flow_node_id) const;
	bool has_flow_node_with_name(const String &p_flow_node_name) const;
	FlowNode *get_flow_node(const FlowNodeID p_flow_node_id) const;
	FlowNode *get_flow_node_by_name(const String &p_flow_node_name) const;
	FlowNodeIDArray get_flow_node_id_list() const;
	PackedStringArray get_flow_node_name_list() const;
	int get_flow_node_count() const;

	FlowNode *create_new_flow_node(const String &p_flow_type_id);
	bool remove_flow_node(const FlowNodeID p_flow_node_id);
	bool change_flow_node_id(const FlowNodeID p_from_id, const FlowNodeID p_to_id);
	FlowNodeIDArray remove_multiple_flow_nodes(const FlowNodeIDArray &p_flow_node_ids);

	FlowScript();
	~FlowScript();
};

#endif
