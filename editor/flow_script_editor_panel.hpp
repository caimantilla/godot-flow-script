#ifndef FLOW_SCRIPT_EDITOR_PANEL_HPP
#define FLOW_SCRIPT_EDITOR_PANEL_HPP

#include "flow_script_editor_graph.hpp"
#include "flow_node_name_dialog.hpp"
#include "flow_node_creation_dialog.hpp"
#include "scene/gui/margin_container.h"
#include "scene/gui/split_container.h"
#include "scene/gui/dialogs.h"


class FlowScriptEditorPanel final : public MarginContainer
{
	GDCLASS(FlowScriptEditorPanel, MarginContainer);

private:
	Ref<FlowScript> edited_flow_script;

	VBoxContainer *main_vbox;
	HBoxContainer *top_menu;
	MarginContainer *no_flow_script_container;
	MarginContainer *editor_container;
	// HSplitContainer *editor_half_split_container;
	FlowScriptEditorGraph *flow_script_editor_graph;

	FlowNodeCreationDialog *dlg_flow_node_creation_menu;
	FlowNodeNameDialog *dlg_name_new_flow_node;
	FlowNodeNameDialog *dlg_rename_existing_flow_node;
	AcceptDialog *dlg_flow_node_creation_failed;

	Vector2 next_flow_node_editor_position;

	void update_style();

	int get_editor_margin() const;
	void show_flow_node_creation_dialog();
	// void show_flow_node_rename_dialog(const FlowNodeID p_for_flow_node_id);
	// void show_new_flow_node_naming_dialog(const FlowNodeID p_flow_type_id);
	void update_next_flow_node_editor_position();
	void update_menu_visibility();
	void initialize_position_of_flow_node_editor_to_mouse_cursor(FlowNodeEditor *p_editor);
	void create_new_flow_node(const String &p_flow_type_id);
	void delete_multiple_flow_nodes(const FlowNodeIDArray &p_flow_node_id_list);
	bool change_flow_node_id(const FlowNodeID p_from, const FlowNodeID p_to);
	
	void on_graph_flow_node_selected(const FlowNodeID p_flow_node_id);
	void on_graph_flow_node_creation_dialog_requested(const Vector2 &p_at_position);
	void on_graph_flow_nodes_deletion_requested(const FlowNodeIDArray &p_flow_node_id_list);
	// void on_graph_flow_node_rename_dialog_requested(const FlowNodeID p_for_flow_node_id);

	Callable on_graph_flow_node_selected_callback;
	Callable on_graph_flow_node_creation_dialog_requested_callback;
	Callable on_graph_flow_nodes_deletion_requested_callback;
	// Callable on_graph_flow_node_rename_dialog_requested_callback;

	void on_dlg_flow_node_creation_menu_flow_type_chosen(const String &p_flow_type_id);
	// void on_dlg_name_new_flow_node_confirmed();
	// void on_dlg_rename_existing_flow_node_confirmed();

	Callable on_dlg_flow_node_creation_menu_flow_type_chosen_callback;
	// Callable on_dlg_name_new_flow_node_confirmed_callback;
	// Callable on_dlg_rename_existing_flow_node_confirmed_callback;

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void set_edited_flow_script(const Ref<FlowScript> &p_flow_script);
	Ref<FlowScript> get_edited_flow_script() const;

	Vector2 get_next_flow_node_editor_position() const;
	Vector2 display_coordinates_to_graph_position(const Vector2i p_coordinates) const;

	FlowScriptEditorPanel();
};


#endif
