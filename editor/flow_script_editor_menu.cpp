#include "flow_script_editor_menu.hpp"
#include "flow_editor_constants.hpp"
#include "../singletons/flow_config_manager.hpp"
#include "editor/editor_string_names.h"
#include "editor/themes/editor_scale.h"


static StringName MARGIN_CONTAINER_NAMES[] = { SNAME("margin_left"), SNAME("margin_top"), SNAME("margin_right"), SNAME("margin_bottom") };


void FlowScriptEditorMenu::_bind_methods()
{
	ADD_SIGNAL(MethodInfo("node_creation_requested"));
}


void FlowScriptEditorMenu::_notification(int p_what)
{
	if (p_what == NOTIFICATION_THEME_CHANGED)
	{
		update_style();
	}
}


void FlowScriptEditorMenu::update_style()
{
	if (Engine::get_singleton()->is_editor_hint())
	{
		btn_create_node->set_icon(get_theme_icon(SNAME("Add"), EditorStringNames::get_singleton()->EditorIcons));
	}
}


int FlowScriptEditorMenu::get_inner_container_margin() const
{
	switch (FlowConfigManager::get_singleton()->get_flow_editor_gui_location())
	{
		case FlowEditorConstants::FLOW_EDITOR_GUI_LOCATION_MAIN_SCREEN:
			return 4 * EDSCALE;
		case FlowEditorConstants::FLOW_EDITOR_GUI_LOCATION_BOTTOM_PANEL:
		default:
			return 0;
	}
}


void FlowScriptEditorMenu::request_node_creation()
{
	emit_signal(SNAME("node_creation_requested"));
}


FlowScriptEditorMenu::FlowScriptEditorMenu()
{
	set_mouse_filter(MOUSE_FILTER_IGNORE);
	set_focus_mode(FOCUS_NONE);

	int inner_container_margin = get_inner_container_margin();

	inner_container = memnew(MarginContainer);
	inner_container->set_mouse_filter(MOUSE_FILTER_IGNORE);
	inner_container->set_focus_mode(FOCUS_NONE);
	inner_container->set_h_size_flags(SIZE_EXPAND_FILL);
	inner_container->set_v_size_flags(SIZE_EXPAND_FILL);

	int cont_name_count = sizeof(MARGIN_CONTAINER_NAMES) / sizeof(MARGIN_CONTAINER_NAMES[0]);
	for (int idx = 0; idx < cont_name_count; idx++)
	{
		inner_container->add_theme_constant_override(MARGIN_CONTAINER_NAMES[idx], inner_container_margin);
	}

	add_child(inner_container);

	bottom_spread = memnew(HBoxContainer);
	bottom_spread->set_mouse_filter(MOUSE_FILTER_IGNORE);
	bottom_spread->set_focus_mode(FOCUS_NONE);
	bottom_spread->set_h_size_flags(SIZE_EXPAND_FILL);
	bottom_spread->set_v_size_flags(SIZE_EXPAND_FILL);
	inner_container->add_child(bottom_spread);

	btn_create_node = memnew(Button);
	btn_create_node->set_text("Create Node");
	btn_create_node->connect(SNAME("pressed"), callable_mp(this, &FlowScriptEditorMenu::request_node_creation));
	// Not currently useful
	btn_create_node->hide();
	bottom_spread->add_child(btn_create_node);
}
