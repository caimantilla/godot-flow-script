#include "flow_script_editor_panel.hpp"
#include "flow_editor_constants.hpp"
#include "../singletons/flow_config_manager.hpp"
#include "editor/themes/editor_scale.h"
#include "scene/scene_string_names.h"


static const String MARGIN_NAMES[] = { "margin_left", "margin_top", "margin_right", "margin_bottom" };


void FlowScriptEditorPanel::_bind_methods()
{
	ADD_SIGNAL(MethodInfo("flow_node_selected", PropertyInfo(TYPE_FLOW_NODE_ID, "flow_node_id")));
}


void FlowScriptEditorPanel::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_READY:
			update_menu_visibility();
			break;
		case NOTIFICATION_THEME_CHANGED:
			update_style();
	}
}


void FlowScriptEditorPanel::set_edited_flow_script(const Ref<FlowScript> &p_flow_script)
{
	if (edited_flow_script == p_flow_script)
	{
		return;
	}

	edited_flow_script = p_flow_script;
	flow_script_editor_graph->set_edited_flow_script(p_flow_script);
	update_menu_visibility();
}


Ref<FlowScript> FlowScriptEditorPanel::get_edited_flow_script() const
{
	return edited_flow_script;
}


Vector2 FlowScriptEditorPanel::get_next_flow_node_editor_position() const
{
	return next_flow_node_editor_position;
}


// Does nothing right now, but if buttons or anything of the sort are added then the theming should be configured in this method.
void FlowScriptEditorPanel::update_style()
{
}


int FlowScriptEditorPanel::get_editor_margin() const
{
	int margin = 0;

	switch (FlowConfigManager::get_singleton()->get_flow_editor_gui_location())
	{
		case FlowEditorConstants::FLOW_EDITOR_GUI_LOCATION_MAIN_SCREEN:
			margin = EDSCALE * 4.0f;
		case FlowEditorConstants::FLOW_EDITOR_GUI_LOCATION_BOTTOM_PANEL:
		default:
			margin = 0;
	}

	return margin;
}


void FlowScriptEditorPanel::show_flow_node_creation_dialog()
{
	if (dlg_flow_node_creation_menu->is_visible() || !edited_flow_script.is_valid())
		return;

	update_next_flow_node_editor_position();

	Vector2i dlg_size_i = dlg_flow_node_creation_menu->get_size();
	Vector2 dlg_size_f = Vector2(dlg_size_i);

	switch (FlowConfigManager::get_singleton()->get_flow_node_creation_dialog_appear_position())
	{
		case FlowEditorConstants::FLOW_NODE_CREATION_DIALOG_APPEAR_POSITION_UNDER_MOUSE:
			{
				Vector2 dlg_origin = DisplayServer::get_singleton()->mouse_get_position() - (dlg_size_f * 0.5f);
				Rect2i dlg_rect = Rect2i(dlg_origin.round(), dlg_size_i);
				dlg_flow_node_creation_menu->popup(dlg_rect);
			}
			break;
		
		case FlowEditorConstants::FLOW_NODE_CREATION_DIALOG_APPEAR_POSITION_SCREEN_CENTER:
		default:
			{
				dlg_flow_node_creation_menu->popup_centered();
			}
			break;
	}
}


// void FlowScriptEditorPanel::show_flow_node_rename_dialog(const FlowNodeID p_for_flow_node_id)
// {
// 	dlg_rename_existing_flow_node->set_target_flow_node_id(p_for_flow_node_id);
// 	dlg_rename_existing_flow_node->popup_centered();
// }


// void FlowScriptEditorPanel::show_new_flow_node_naming_dialog(const StringName &p_flow_type_id)
// {
// 	dlg_name_new_flow_node->set_flow_type_id(p_flow_type_id);
// 	dlg_name_new_flow_node->popup_centered();
// }


void FlowScriptEditorPanel::update_next_flow_node_editor_position()
{
	Vector2 new_pos = flow_script_editor_graph->get_local_mouse_position();
	new_pos += flow_script_editor_graph->get_scroll_offset();

	if (!Math::is_zero_approx(flow_script_editor_graph->get_zoom()))
	{
		new_pos /= flow_script_editor_graph->get_zoom();
	}

	// new_pos /= EDSCALE;
	new_pos = new_pos.round();

	next_flow_node_editor_position = new_pos;
}


void FlowScriptEditorPanel::update_menu_visibility()
{
	if (!edited_flow_script.is_valid())
	{
		main_vbox->hide();
		no_flow_script_container->show();
	}
	else
	{
		no_flow_script_container->hide();
		main_vbox->show();
	}
}


void FlowScriptEditorPanel::initialize_position_of_flow_node_editor_to_mouse_cursor(FlowNodeEditor *p_editor)
{
	FlowNode *flow_node = p_editor->get_flow_node();
	
	Vector2 new_pos = next_flow_node_editor_position;
	Vector2 node_half_size = 0.5f * p_editor->get_size();

	new_pos -= node_half_size;
	new_pos = new_pos.floor();

	p_editor->set_position_offset(new_pos);
	p_editor->silent_copy_graph_position_to_flow_node();
}


void FlowScriptEditorPanel::create_new_flow_node(const String &p_flow_type_id)
{
	ERR_FAIL_COND(!edited_flow_script.is_valid());
	ERR_FAIL_COND(!FlowTypeDB::get_singleton()->has_type(p_flow_type_id));

	FlowNode *new_node = edited_flow_script->create_new_flow_node(p_flow_type_id);
	FlowNodeID new_node_id = new_node->get_flow_node_id();
	
	if (new_node == nullptr)
	{
		dlg_flow_node_creation_failed->popup_centered();
		return;
	}

	FlowNodeEditor *editor = flow_script_editor_graph->get_editor_for_flow_node_using_id(new_node_id);
	if (editor != nullptr)
	{
		// On the very first draw, position the node to where the mouse cursor is.
		Callable draw_callback = callable_mp(this, &FlowScriptEditorPanel::initialize_position_of_flow_node_editor_to_mouse_cursor).bind(editor);
		editor->connect(SceneStringNames::get_singleton()->draw, draw_callback, CONNECT_ONE_SHOT | CONNECT_DEFERRED);
	}
}


void FlowScriptEditorPanel::delete_multiple_flow_nodes(const FlowNodeIDArray &p_flow_node_id_list)
{
	ERR_FAIL_COND(!edited_flow_script.is_valid());
	edited_flow_script->remove_multiple_flow_nodes(p_flow_node_id_list);
}


bool FlowScriptEditorPanel::change_flow_node_id(const FlowNodeID p_from, const FlowNodeID p_to)
{
	ERR_FAIL_COND_V(!edited_flow_script.is_valid(), false);
	return edited_flow_script->change_flow_node_id(p_from, p_to);
}


void FlowScriptEditorPanel::on_graph_flow_node_selected(const FlowNodeID p_flow_node_id)
{
	emit_signal(SNAME("flow_node_selected"), p_flow_node_id);
}


void FlowScriptEditorPanel::on_graph_flow_node_creation_dialog_requested(const Vector2 &p_at_position)
{
	show_flow_node_creation_dialog();
}


void FlowScriptEditorPanel::on_graph_flow_nodes_deletion_requested(const FlowNodeIDArray &p_flow_node_id_list)
{
	delete_multiple_flow_nodes(p_flow_node_id_list);
}


// void FlowScriptEditorPanel::on_graph_flow_node_rename_dialog_requested(const FlowNodeID p_for_flow_node_id)
// {
// 	show_flow_node_rename_dialog(p_for_flow_node_id);
// }


void FlowScriptEditorPanel::on_dlg_flow_node_creation_menu_flow_type_chosen(const String &p_flow_type_id)
{
	ERR_FAIL_COND(!edited_flow_script.is_valid());
	
	dlg_flow_node_creation_menu->hide();
	if (FlowTypeDB::get_singleton()->has_type(p_flow_type_id))
	{
		Ref<FlowType> type = FlowTypeDB::get_singleton()->get_type(p_flow_type_id);
		create_new_flow_node(p_flow_type_id);
		// if (type->is_nameable())
		// {
		// 	show_new_flow_node_naming_dialog(p_flow_type_id);
		// }
		// else
		// {
			// StringName new_flow_node_id = edited_flow_script->generate_unique_flow_node_id();
			// create_new_flow_node(new_flow_node_id, p_flow_type_id);
		// }
	}
}


// void FlowScriptEditorPanel::on_dlg_name_new_flow_node_confirmed()
// {
// 	ERR_FAIL_NULL(edited_flow_script);

// 	StringName new_node_id = dlg_name_new_flow_node->get_entered_flow_node_id();
// 	if (!edited_flow_script->is_new_flow_node_id_valid(new_node_id))
// 	{
// 		dlg_name_new_flow_node->alert_invalid_id();
// 	}
// 	else
// 	{
// 		dlg_name_new_flow_node->hide();
// 		create_new_flow_node(new_node_id, dlg_name_new_flow_node->get_flow_type_id());
// 	}
// }


// void FlowScriptEditorPanel::on_dlg_rename_existing_flow_node_confirmed()
// {
// 	ERR_FAIL_NULL(edited_flow_script);

// 	StringName target_node_id = dlg_rename_existing_flow_node->get_target_flow_node_id();
// 	StringName new_node_id = dlg_rename_existing_flow_node->get_entered_flow_node_id();

// 	if (edited_flow_script->is_new_flow_node_id_valid(new_node_id) && change_flow_node_id(target_node_id, new_node_id))
// 	{
// 		dlg_rename_existing_flow_node->hide();
// 	}
// 	else
// 	{
// 		dlg_rename_existing_flow_node->alert_invalid_id();
// 	}
// }


FlowScriptEditorPanel::FlowScriptEditorPanel()
{
	on_graph_flow_node_selected_callback = callable_mp(this, &FlowScriptEditorPanel::on_graph_flow_node_selected);
	on_graph_flow_node_creation_dialog_requested_callback = callable_mp(this, &FlowScriptEditorPanel::on_graph_flow_node_creation_dialog_requested);
	on_graph_flow_nodes_deletion_requested_callback = callable_mp(this, &FlowScriptEditorPanel::on_graph_flow_nodes_deletion_requested);
	// on_graph_flow_node_rename_dialog_requested_callback = callable_mp(this, &FlowScriptEditorPanel::on_graph_flow_node_rename_dialog_requested);

	on_dlg_flow_node_creation_menu_flow_type_chosen_callback = callable_mp(this, &FlowScriptEditorPanel::on_dlg_flow_node_creation_menu_flow_type_chosen);
	// on_dlg_name_new_flow_node_confirmed_callback = callable_mp(this, &FlowScriptEditorPanel::on_dlg_name_new_flow_node_confirmed);
	// on_dlg_rename_existing_flow_node_confirmed_callback = callable_mp(this, &FlowScriptEditorPanel::on_dlg_rename_existing_flow_node_confirmed);

	int editor_margin = get_editor_margin();
	int margin_count = sizeof(MARGIN_NAMES) / sizeof(StringName);
	for (int i = 0; i < margin_count; i++)
	{
		add_theme_constant_override(MARGIN_NAMES[i], editor_margin);
	}

	dlg_flow_node_creation_menu = memnew(FlowNodeCreationDialog);
	dlg_flow_node_creation_menu->hide();
	dlg_flow_node_creation_menu->connect(SNAME("flow_type_chosen"), on_dlg_flow_node_creation_menu_flow_type_chosen_callback);
	add_child(dlg_flow_node_creation_menu);

	dlg_name_new_flow_node = memnew(FlowNodeNameDialog);
	dlg_name_new_flow_node->hide();
	dlg_name_new_flow_node->set_title("Enter new FlowNode name...");
	// dlg_name_new_flow_node->connect(SNAME("confirmed"), on_dlg_name_new_flow_node_confirmed_callback);
	add_child(dlg_name_new_flow_node);

	dlg_rename_existing_flow_node = memnew(FlowNodeNameDialog);
	dlg_rename_existing_flow_node->hide();
	dlg_rename_existing_flow_node->set_title("Rename existing FlowNode...");
	// dlg_rename_existing_flow_node->connect(SNAME("confirmed"), on_dlg_rename_existing_flow_node_confirmed_callback);
	add_child(dlg_rename_existing_flow_node);

	dlg_flow_node_creation_failed = memnew(AcceptDialog);
	dlg_flow_node_creation_failed->hide();
	dlg_flow_node_creation_failed->set_text("Failed to create FlowNode.");
	add_child(dlg_flow_node_creation_failed);

	main_vbox = memnew(VBoxContainer);
	main_vbox->set_anchors_and_offsets_preset(PRESET_FULL_RECT);
	main_vbox->set_h_size_flags(SIZE_EXPAND_FILL);
	main_vbox->set_v_size_flags(SIZE_EXPAND_FILL);
	add_child(main_vbox);

	top_menu = memnew(HBoxContainer);
	top_menu->set_h_size_flags(SIZE_EXPAND_FILL);
	top_menu->set_v_size_flags(SIZE_SHRINK_BEGIN);
	top_menu->hide();
	main_vbox->add_child(top_menu);

	no_flow_script_container = memnew(MarginContainer);
	add_child(no_flow_script_container);

	Label *no_flow_script_label = memnew(Label);
	no_flow_script_label->set_text("Select a FlowScript node to create and edit graphical scripts.");
	no_flow_script_label->set_h_size_flags(SIZE_EXPAND_FILL);
	no_flow_script_label->set_v_size_flags(SIZE_EXPAND_FILL);
	no_flow_script_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	no_flow_script_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	no_flow_script_container->add_child(no_flow_script_label);

	editor_container = memnew(MarginContainer);
	editor_container->set_anchors_and_offsets_preset(PRESET_FULL_RECT);
	editor_container->set_h_size_flags(SIZE_EXPAND_FILL);
	editor_container->set_v_size_flags(SIZE_EXPAND_FILL);
	main_vbox->add_child(editor_container);

	// NOTE: Don't use the half split container right now. I'll change things a bit to use it for favorite nodes or something... Or maybe that should spread vertically in the main stack?
	// editor_half_split_container = memnew(HSplitContainer);
	// editor_half_split_container->set_h_size_flags(SIZE_EXPAND_FILL);
	// editor_half_split_container->set_v_size_flags(SIZE_EXPAND_FILL);
	// main_vbox->add_child(editor_half_split_container);

	// Also i just realized that FlowScriptEditorMenu is supposed to overlay the graph.
	// Maybe use this instead?? idk
	flow_script_editor_graph = memnew(FlowScriptEditorGraph);
	flow_script_editor_graph->set_h_size_flags(SIZE_EXPAND_FILL);
	flow_script_editor_graph->set_v_size_flags(SIZE_EXPAND_FILL);
	// TODO: Hook up more signals for node creation, including automatic connection when dragged from empty. That should be possible, right...?
	flow_script_editor_graph->connect(SNAME("flow_node_selected"), on_graph_flow_node_selected_callback);
	flow_script_editor_graph->connect(SNAME("flow_node_creation_dialog_requested"), on_graph_flow_node_creation_dialog_requested_callback);
	flow_script_editor_graph->connect(SNAME("flow_nodes_deletion_requested"), on_graph_flow_nodes_deletion_requested_callback);
	// flow_script_editor_graph->connect(SNAME("flow_node_rename_dialog_requested"), on_graph_flow_node_rename_dialog_requested_callback);
	editor_container->add_child(flow_script_editor_graph);
}
