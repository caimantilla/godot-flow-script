#ifndef FLOW_NODE_EDITOR_HPP
#define FLOW_NODE_EDITOR_HPP

#include "scene/gui/button.h"
#include "scene/gui/graph_node.h"
#include "flow_node.hpp"
#include "flow_node_editor_out_going_connection_parameters.hpp"
#include "editor/themes/editor_scale.h"
#include "../flow_constants.hpp"


class FlowNodeEditor : public GraphNode
{
	GDCLASS(FlowNodeEditor, GraphNode);

	friend class FlowFactory;
	friend class FlowScriptEditorGraph;

private:
	FlowNode *flow_node = nullptr;
	Button *btn_rename;
	Button *btn_close;

	bool has_initialized = false;
	bool edit_safe = false;
	bool flow_node_update_queued = false;

	void initialize();
	void clean_up();
	void update_editor_title();
	void update_editor_tooltip_text();

	void queue_flow_node_update();

	void set_safe_to_edit(bool p_edit_safe);
	bool is_safe_to_edit() const;

	void request_rename();
	void request_deletion();
	void request_graph_position_update();

	bool is_flow_node_instance_valid() const;
	bool is_flow_node_instance_invalid() const;


	void on_flow_node_name_changed(const String &p_from, const String &p_to);

protected:
	void _notification(int p_what);
	static void _bind_methods();

	int input_get_slot_from_port(int p_port);
	int output_get_slot_from_port(int p_port);
	int input_get_port_from_slot(int p_slot) const;
	int output_get_port_from_slot(int p_slot) const;

	Ref<FlowNodeEditorOutGoingConnectionParameters> create_out_going_connection(const FlowNodeID p_target_node_id, const int p_output_slot) const;

	virtual void _editor_init();

	virtual void _initialize();
	virtual void _clean_up();
	virtual void _flow_node_updated();
	virtual void _update_style();
	virtual String _get_editor_title() const;
	virtual String _get_editor_tooltip_text() const;

	virtual void _set_out_going_connection(Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection);
	virtual int _get_input_slot() const;
	virtual TypedArray<FlowNodeEditorOutGoingConnectionParameters> _get_out_going_connections() const;

	GDVIRTUAL0(_initialize);
	GDVIRTUAL0(_clean_up);
	GDVIRTUAL0(_flow_node_updated);
	GDVIRTUAL0(_update_style);
	GDVIRTUAL0RC(String, _get_editor_title);
	GDVIRTUAL0RC(String, _get_editor_tooltip_text);

	GDVIRTUAL1(_set_out_going_connection, Dictionary);
	GDVIRTUAL0RC(int, _get_input_slot);
	GDVIRTUAL0RC(TypedArray<Dictionary>, _get_out_going_connections);

public:
	void set_flow_node(FlowNode * p_flow_node);
	FlowNode *get_flow_node() const;
	FlowNodeID get_flow_node_id() const;
	String get_flow_node_name() const;
	String get_flow_type_id() const;
	String get_flow_type_name() const;
	bool is_flow_node_nameable() const;

	float get_editor_scale() const;
	void silent_copy_graph_position_to_flow_node();
	
	int get_input_slot();
	int get_input_port();

	void flow_node_updated();
	void update_style();

	void set_out_going_connection(Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection);
	TypedArray<FlowNodeEditorOutGoingConnectionParameters> get_out_going_connections() const;

	FlowNodeEditor();
	~FlowNodeEditor();
};


#endif
