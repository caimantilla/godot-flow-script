#ifndef FLOW_SCRIPT_EDITOR_GRAPH_HPP
#define FLOW_SCRIPT_EDITOR_GRAPH_HPP

#include "../flow_constants.hpp"
#include "scene/gui/graph_edit.h"
#include "scene/gui/view_panner.h"
#include "scene/gui/graph_edit_arranger.h"
#include "../core_types/flow_script.hpp"
#include "../core_types/flow_node.hpp"
#include "../core_types/flow_node_editor.hpp"
#include "../core_types/flow_node_editor_out_going_connection_parameters.hpp"


class FlowScriptEditorGraph final : public GraphEdit
{
	GDCLASS(FlowScriptEditorGraph, GraphEdit);

private:
	bool graph_nodes_redraw_queued = false;
	bool graph_connections_redraw_queued = false;
	Ref<FlowScript> edited_flow_script;
	HashMap<FlowNodeID, FlowNodeEditor *> flow_node_editor_map;

	void init_signals();

	void clear_graph_nodes();
	void clear_graph_connections();
	void redraw_graph_nodes();
	void redraw_graph_connections();
	FlowNodeEditor *instantiate_editor_for_flow_node(const FlowNodeID p_flow_node_id);
	void delete_editor_for_flow_node(const FlowNodeID p_flow_node_id);
	void delete_invalid_flow_node_editors();
	Ref<FlowNodeEditorOutGoingConnectionParameters> create_flow_node_editor_out_going_connection_parameters(const int p_origin_port, const FlowNodeID p_target_node_id);
	void create_connection_between_flow_node_editors(FlowNodeEditor *p_from_editor, const int p_from_port, FlowNodeEditor *p_to_editor);
	void erase_connection_from_flow_node_editor(FlowNodeEditor *p_from_editor, const int p_from_port);
	void update_flow_node_editor_graph_position(FlowNodeEditor *p_editor);

	void emit_flow_node_selected(const FlowNodeID p_flow_node_id);
	// void request_flow_node_rename_dialog(const StringName &p_current_flow_node_id);
	void request_flow_node_creation_dialog(const Vector2 &p_at_position);

	void on_graph_connection_request(const StringName &p_from_node_tree_name, const int p_from_port, const StringName &p_to_node_tree_name, const int p_to_port);
	void on_graph_connection_to_empty(const StringName &p_from_node_tree_name, const int p_from_port, const Vector2 &p_release_position);
	void on_graph_copy_nodes_request();
	void on_graph_delete_nodes_request(const TypedArray<StringName> &p_nodes);
	void on_graph_disconnection_request(const StringName &p_from_node_tree_name, const int p_from_port, const StringName &p_to_node_tree_name, const int p_to_port);
	void on_graph_node_deselected(Node *p_node);
	void on_graph_node_selected(Node *p_node);
	void on_graph_end_node_move();
	void on_graph_popup_request(const Vector2 &p_at_position);

	Callable on_graph_connection_request_callback;
	Callable on_graph_connection_to_empty_callback;
	Callable on_graph_copy_nodes_request_callback;
	Callable on_graph_delete_nodes_request_callback;
	Callable on_graph_disconnection_request_callback;
	Callable on_graph_node_deselected_callback;
	Callable on_graph_node_selected_callback;
	Callable on_graph_end_node_move_callback;
	Callable on_graph_popup_request_callback;

	void on_flow_script_flow_node_added(const FlowNodeID p_flow_node_id);
	void on_flow_script_removing_flow_node(const FlowNodeID p_flow_node_id);
	void on_flow_script_flow_node_removed(const FlowNodeID p_flow_node_id);
	void on_flow_script_flow_node_id_changed(const FlowNodeID p_from_id, const FlowNodeID p_to_id);
	void on_flow_script_multiple_flow_nodes_removed(const FlowNodeIDArray &p_flow_node_ids);

	Callable on_flow_script_flow_node_added_callback;
	Callable on_flow_script_removing_flow_node_callback;
	Callable on_flow_script_flow_node_removed_callback;
	Callable on_flow_script_flow_node_id_changed_callback;
	Callable on_flow_script_multiple_flow_nodes_removed_callback;

	// void on_flow_node_editor_rename_dialog_requested(FlowNodeEditor *p_editor);
	void on_flow_node_editor_slot_updated(const int p_slot_index, FlowNodeEditor *p_editor);
	void on_flow_node_editor_graph_position_update_requested(FlowNodeEditor *p_editor);
	void on_flow_node_editor_delete_request(FlowNodeEditor *p_editor);

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void set_edited_flow_script(const Ref<FlowScript> &p_flow_script);
	Ref<FlowScript> get_edited_flow_script() const;
	bool has_editor_for_flow_node_with_id(const FlowNodeID p_flow_node_id) const;
	FlowNodeEditor *get_editor_for_flow_node_using_id(const FlowNodeID p_flow_node_id) const;
	Vector<FlowNodeEditor *> get_flow_node_editor_list() const;
	FlowNodeEditor *get_flow_node_editor_using_scene_tree_name(const String &p_node_name) const;
	void queue_redraw_graph_nodes();
	void queue_redraw_graph_connections();
	Vector2 display_coordinates_to_graph_position(const Vector2i p_coordinates) const;

	FlowScriptEditorGraph();
};


#endif
