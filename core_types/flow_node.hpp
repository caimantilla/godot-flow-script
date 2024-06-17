#ifndef FLOW_NODE_HPP
#define FLOW_NODE_HPP

#include "core/io/resource.h"
#include "flow_node_execution_state.hpp"
#include "flow_type.hpp"
#include "../flow_constants.hpp"

class FlowNode : public Resource
{
	GDCLASS(FlowNode, Resource);

	friend class FlowFiber;
	friend class FlowFactory;
	friend class FlowScript;

private:
	FlowScript *flow_script; // Internally, pointer to FlowScript should be stored rather than Ref. Memory leaks!!1!
	FlowNodeID flow_node_id = FLOW_NODE_ID_NIL; // Used internally
	String flow_node_name; // Used to name procedures or any other label-style nodes
	String flow_type_id; // The ID of the type used, this is not directly related to the FlowNode class
	Vector2i flow_graph_position; // The node's position in the editor graph, without influence from DPI scaling

	void set_flow_node_id(const FlowNodeID p_new_id);

protected:
	static void _bind_methods();
	void _validate_property(PropertyInfo &p_property) const;

	void execute(FlowNodeExecutionState *p_state);
	void on_external_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to);

	virtual String _get_flow_type_name() const;
	virtual String _get_flow_type_category() const;
	virtual String _get_flow_type_description() const;
	virtual String _get_flow_type_editor() const;
	virtual Ref<Texture2D> _get_flow_type_icon() const;

	virtual void _execute(FlowNodeExecutionState *p_state);
	virtual bool _is_property_flow_node_reference(const StringName &p_name) const;
	virtual bool _is_property_flow_node_reference_list(const StringName &p_name) const;
	virtual void _on_external_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to);

	GDVIRTUAL0RC(String, _get_flow_type_name);
	GDVIRTUAL0RC(String, _get_flow_type_category);
	GDVIRTUAL0RC(String, _get_flow_type_description);
	GDVIRTUAL0RC(String, _get_flow_type_editor);
	GDVIRTUAL0RC(Ref<Texture2D>, _get_flow_type_icon);
	GDVIRTUAL1(_execute, FlowNodeExecutionState *)
	GDVIRTUAL1RC(bool, _is_property_flow_node_reference, const StringName &);
	GDVIRTUAL1RC(bool, _is_property_flow_node_reference_list, const StringName &);
	GDVIRTUAL2(_on_external_flow_node_id_changed, const FlowNodeID &, const FlowNodeID &);

public:
	void emit_changed();

	Ref<FlowScript> get_flow_script() const;
	FlowNodeID get_flow_node_id() const;
	void set_flow_node_name(const String &p_name);
	String get_flow_node_name() const;

	String get_flow_type_name() const;
	String get_flow_type_category() const;
	String get_flow_type_description() const;
	String get_flow_type_editor() const;
	Ref<Texture2D> get_flow_type_icon() const;
	
	void set_flow_graph_position_no_signal(Vector2i p_position);
	void set_flow_graph_position(Vector2i p_position);
	Vector2i get_flow_graph_position() const;

	bool is_flow_node_nameable() const;
	bool is_property_flow_node_reference(const StringName &p_name) const;
	bool is_property_flow_node_reference_list(const StringName &p_name) const;
};

#endif
