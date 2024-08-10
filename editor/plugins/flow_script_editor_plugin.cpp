#include "flow_script_editor_plugin.hpp"
#include "editor/themes/editor_scale.h"
#include "editor/editor_node.h"
#include "editor/gui/editor_bottom_panel.h"
#include "editor/editor_settings.h"
#include "editor/editor_command_palette.h"


bool FlowScriptEditorPlugin::handles(Object *p_object) const
{
	return Object::cast_to<FlowScript>(p_object) != nullptr;
}


void FlowScriptEditorPlugin::edit(Object *p_object)
{
	FlowScript *flow_script_ptr = Object::cast_to<FlowScript>(p_object);
	if (flow_script_ptr == nullptr)
	{
		return;
	}
}


void FlowScriptEditorPlugin::update_graph_theme()
{
	Ref<Font> main_font = EditorNode::get_singleton()->get_editor_theme()->get_font("main_msdf", EditorStringName(EditorFonts));
	Ref<Font> bold_font = EditorNode::get_singleton()->get_editor_theme()->get_font("main_bold_msdf", EditorStringName(EditorFonts));

	graph_theme->set_default_font(main_font);
	graph_theme->set_font("font", "Label", main_font);
	graph_theme->set_font("font", "GraphNodeTitleLabel", bold_font);
	graph_theme->set_font("normal_font", "RichTextLabel", main_font);
	graph_theme->set_font("bold_font", "RichTextLabel", bold_font);
}


FlowScriptEditorPlugin::FlowScriptEditorPlugin()
{
	window_wrapper = memnew(WindowWrapper);
	window_wrapper->set_window_title(vformat(TTR("%s - Godot Engine"), TTR("FlowScript Editor")));
	window_wrapper->set_margins_enabled(true);
	window_wrapper->connect("window_visibility_changed", callable_mp(this, &FlowScriptEditorPlugin::on_window_visibility_changed));

	bottom_panel_button = EditorNode::get_bottom_panel()->add_item(TTR("FlowScript Editor"), window_wrapper, ED_SHORTCUT_AND_COMMAND("bottom_panels/toggle_flow_script_editor_bottom_panel", TTR("Toggle FlowScript Editor Bottom Panel")));

	main_split = memnew(HSplitContainer);
	Ref<Shortcut> make_floating_shortcut = ED_SHORTCUT_AND_COMMAND("flow_script_editor/make_floating", TTR("Make Floating"));
	window_wrapper->set_wrapped_control(main_split, make_floating_shortcut);

	VBoxContainer *left_vbox = memnew(VBoxContainer);
	left_vbox->set_custom_minimum_size(Size2(200, 300) * EDSCALE);
	main_split->add_child(left_vbox);

	HBoxContainer *menu_hbox = memnew(HBoxContainer);
	left_vbox->add_child(menu_hbox);

	file_menu = memnew(MenuButton);
	file_menu->set_text(TTR("File"));
	file_menu->set_shortcut_context(main_split);
	file_menu->get_popup()->add_item(TTR("New FlowScript..."), FILE_NEW);
	file_menu->get_popup()->add_separator();
	file_menu->get_popup()->add_item(TTR("Load FlowScript File..."), FILE_OPEN);
	file_menu->get_popup()->add_shortcut(ED_SHORTCUT("flow_script_editor/save", TTR("Save File"), KeyModifierMask::ALT | KeyModifierMask::CMD_OR_CTRL | Key::S), FILE_SAVE);
	file_menu->get_popup()->add_shortcut(ED_SHORTCUT("flow_script_editor/save_as", TTR("Save File As...")), FILE_SAVE_AS);
	file_menu->get_popup()->add_separator();
	file_menu->get_popup()->add_item(TTR("Close File"), FILE_CLOSE);
	file_menu->connect(SceneStringName(id_pressed), callable_mp(this, &FlowScriptEditorPlugin::on_file_menu_item_pressed));
	menu_hbox->add_child(file_menu);

	file_menu_update_clickable();

	Control *menu_middle_pad = memnew(Control);
	menu_middle_pad->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	menu_middle_pad->add_child(menu_middle_pad);

	make_floating_button = memnew(ScreenSelect);
	make_floating_button->set_flat(true);
	make_floating_button->connect("request_open_in_screen", callable_mp(this, &FlowScriptEditorPlugin::on_make_floating_button_open_to_screen_request));
	if (!make_floating_button->is_disabled())
	{
		make_floating_button->set_tooltip_text(TTR("Make the FlowScript editor floating."));
	}
	menu_hbox->add_child(make_floating_button);

	script_list = memnew(ItemList);
	script_list->set_auto_translate_mode(AUTO_TRANSLATE_MODE_DISABLED);
	script_list->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	script_list->connect("item_selected", callable_mp(this, &FlowScriptEditorPlugin::on_script_list_item_selected));
	script_list->connect("item_clicked", callable_mp(this, &FlowScriptEditorPlugin::on_script_list_item_clicked));
	// SET_DRAG_FORWARDING_GCD(script_list, FlowScriptEditorPlugin);
	left_vbox->add_child(script_list);
}
