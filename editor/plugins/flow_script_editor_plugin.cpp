#include "flow_script_editor_plugin.hpp"
#include "../flow_script_node_type_db.hpp"
#include "../nodes/flow_script_node_editor.hpp"
#include "../nodes/flow_script_node_editor_placeholder.hpp"
#include "core/config/project_settings.h"
#include "core/os/time.h"
#include "scene/main/timer.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/menu_button.h"
#include "scene/gui/item_list.h"
#include "scene/gui/tab_container.h"
#include "scene/gui/separator.h"
#include "scene/gui/split_container.h"
#include "scene/gui/view_panner.h"
#include "scene/gui/graph_edit.h"
#include "scene/gui/graph_edit_arranger.h"
#include "scene/gui/graph_frame.h"
#include "scene/gui/graph_node.h"
#include "editor/editor_settings.h"
#include "editor/editor_command_palette.h"
#include "editor/window_wrapper.h"
#include "editor/editor_string_names.h"
#include "editor/editor_node.h"
#include "editor/editor_interface.h"
#include "editor/editor_undo_redo_manager.h"
#include "editor/editor_help.h"
#include "editor/editor_paths.h"
#include "editor/filesystem_dock.h"
#include "editor/gui/editor_bottom_panel.h"
#include "editor/gui/editor_file_dialog.h"
#include "editor/themes/editor_scale.h"


bool EditorInspectorPluginFlowScriptNode::can_handle(Object *p_object)
{
	return Object::cast_to<FlowScriptNode>(p_object) != nullptr;
}


bool EditorInspectorPluginFlowScriptNode::parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide)
{
	FlowScriptNode *node = Object::cast_to<FlowScriptNode>(p_object);

	if (node != nullptr)
	{
		if (p_path == "script" || p_path == "resource_path")
		{
			return true;
		}
	}

	return false;
}


bool FlowScriptEditorNodeCreateDialog::is_any_type_selected() const
{
	return type_tree->is_anything_selected();
}


FlowScriptNodeTypeInfo FlowScriptEditorNodeCreateDialog::get_selected_type_info() const
{
	const TreeItem *selected_item = type_tree->get_selected();
	if (selected_item != nullptr)
	{
		const Dictionary type_dict = selected_item->get_metadata(0);
		if (!type_dict.is_empty())
		{
			return FlowScriptNodeTypeInfo::create_from_dictionary(type_dict);
		}
	}
	return FlowScriptNodeTypeInfo::create_null();
}


String FlowScriptEditorNodeCreateDialog::get_search_filter() const
{
	return type_search_filter_line->get_text().strip_edges();
}


void FlowScriptEditorNodeCreateDialog::update_mark_type_favorite_button()
{
	if (is_any_type_selected())
	{
		mark_type_favorite_button->set_disabled(false);
		const FlowScriptNodeTypeInfo type = get_selected_type_info();
		bool is_selected_favorite = favorite_type_info_list.has(type);
		mark_type_favorite_button->set_pressed_no_signal(is_selected_favorite);
	}
	else
	{
		mark_type_favorite_button->set_disabled(true);
		mark_type_favorite_button->set_pressed_no_signal(false);
	}
}


void FlowScriptEditorNodeCreateDialog::update_type_description()
{
	const FlowScriptNodeTypeInfo selected_type = get_selected_type_info();
	if (selected_type.enabled)
	{
		String disp_desc = selected_type.type_description.is_empty() ? TTR("No description available.") : selected_type.type_description;
		help_bit->set_custom_text(TTR("Type:"), selected_type.type_name, disp_desc);
	}
	else
	{
		help_bit->set_custom_text(TTR("Type:"), TTR("No type selected."), String());
	}
}


void FlowScriptEditorNodeCreateDialog::rebuild_type_tree_gui()
{
	const String filter = get_search_filter().strip_edges();
	TreeItem *root_item = type_tree->create_item();
	LocalVector<int> displayed_type_index_list;
	displayed_type_index_list.reserve(local_type_info_list.size());

	if (filter.is_empty())
	{
		for (int i = 0; i < local_type_info_list.size(); i++)
		{
			displayed_type_index_list.push_back(i);
		}
	}
	else
	{
		for (int i = 0; i < local_type_info_list.size(); i++)
		{
			const FlowScriptNodeTypeInfo &type_info = local_type_info_list[i];
			if (type_info.type_name.containsn(filter))
			{
				displayed_type_index_list.push_back(i);
			}
		}
	}

	HashSet<String> category_set;

	for (const int type_index : displayed_type_index_list)
	{
		const FlowScriptNodeTypeInfo &type_info = local_type_info_list[type_index];
		if (!type_info.type_category.is_empty())
		{
			category_set.insert(type_info.type_category);
		}
	}

	HashMap<String, TreeItem *> category_item_map;
	category_item_map.reserve(category_set.size());

	for (const String &category : category_set)
	{
		TreeItem *super_item = type_tree->get_root();
		PackedStringArray split = category.split("/", false, 0);

		for (int i = 1; i < split.size(); i++)
		{
			const String super_category = String::chr('/').join(split.slice(0, i - 1));

			if (!super_category.is_empty() && !category_item_map.has(super_category))
			{
				const String super_item_name = split[i - 1];

				super_item = super_item->create_child();
				super_item->set_selectable(0, false);
				super_item->set_text(0, super_item_name);

				category_item_map.insert(super_category, super_item);
			}
		}

		const String last_item_name = split[split.size() - 1];

		TreeItem *last_item = super_item->create_child();
		last_item->set_selectable(0, false);
		last_item->set_text(0, last_item_name);

		category_item_map.insert(category, last_item);
	}

	TreeItem *item_to_auto_select = nullptr;
	double peak_item_name_similarity = -1.0;

	for (const int type_index : displayed_type_index_list)
	{
		const FlowScriptNodeTypeInfo &type_info = local_type_info_list[type_index];
		TreeItem *parent_item = type_tree->get_root();

		if (!type_info.type_category.is_empty() && category_item_map.has(type_info.type_category))
		{
			parent_item = category_item_map[type_info.type_category];
		}

		TreeItem *type_item = parent_item->create_child();
		type_item->set_selectable(0, true);
		type_item->set_text(0, type_info.type_name);
		type_item->set_tooltip_text(0, create_type_tooltip_text(type_info));
		type_item->set_metadata(0, type_info.to_dictionary());

		if (!filter.is_empty())
		{
			double curr_similarity = filter.similarity(type_info.type_name);
			if (curr_similarity > peak_item_name_similarity)
			{
				peak_item_name_similarity = curr_similarity;
				item_to_auto_select = type_item;
			}
		}
	}

	// Move category items to display under types, rather than above.
	for (KeyValue<String, TreeItem *> &E : category_item_map)
	{
		TreeItem *item = E.value;
		TreeItem *parent = item->get_parent();
		const int child_count = parent->get_child_count();
		if (child_count > 1)
		{
			item->move_after(parent->get_child(child_count - 1));
		}
	}

	expand_type_tree_gui();

	if (item_to_auto_select != nullptr)
	{
		type_tree->set_selected(item_to_auto_select, 0);
	}
}


void FlowScriptEditorNodeCreateDialog::clear_type_tree_gui()
{
	type_tree->clear();
}


void FlowScriptEditorNodeCreateDialog::expand_type_tree_gui()
{
	type_tree->get_root()->set_collapsed_recursive(false);
}


void FlowScriptEditorNodeCreateDialog::collapse_type_tree_gui()
{
	TreeItem *root = type_tree->get_root();

	for (int i = 0; i < root->get_child_count(); i++)
	{
		TreeItem *child = root->get_child(i);

		if (!child->is_folding_disabled())
		{
			child->set_collapsed_recursive(true);
		}
	}
}


void FlowScriptEditorNodeCreateDialog::clear_type_list()
{
	local_type_info_list.clear();
	type_tree->clear();
}


void FlowScriptEditorNodeCreateDialog::refresh_type_list()
{
	clear_type_list();

	const PackedStringArray nodes_blacklist = GLOBAL_GET("flow_script/types/blacklist/nodes");
	const PackedStringArray categories_blacklist = GLOBAL_GET("flow_script/types/blacklist/categories");
	const PackedStringArray nodes_whitelist = GLOBAL_GET("flow_script/types/whitelist/nodes");
	const PackedStringArray categories_whitelist = GLOBAL_GET("flow_script/types/whitelist/categories");

	HashSet<String> nodes_blackset;
	nodes_blackset.reserve(nodes_blacklist.size());
	HashSet<String> nodes_whiteset;
	nodes_whiteset.reserve(nodes_whitelist.size());

	for (const String &node_id : nodes_blacklist)
	{
		nodes_blackset.insert(node_id.strip_edges());
	}
	for (const String &node_id : nodes_whitelist)
	{
		nodes_whiteset.insert(node_id.strip_edges());
	}

	const Vector<FlowScriptNodeTypeInfo> retrieved_type_list = FlowScriptNodeTypeDB::get_singleton()->get_node_type_list();

	for (const FlowScriptNodeTypeInfo &type_info : retrieved_type_list)
	{
		if (!type_info.enabled)
		{
			continue;
		}

		bool blacklisted = !(nodes_whiteset.is_empty() && categories_whitelist.is_empty());
		bool whitelisted = false;

		if (nodes_blackset.has(type_info.type_id))
		{
			blacklisted = true;
		}
		else
		{
			for (const String &category : categories_blacklist)
			{
				if (type_info.type_category.begins_with(category))
				{
					blacklisted = true;
					break;
				}
			}
		}
		if (nodes_whiteset.has(type_info.type_id))
		{
			whitelisted = true;
		}
		else
		{
			for (const String &category : categories_whitelist)
			{
				if (type_info.type_category.begins_with(category))
				{
					whitelisted = true;
					break;
				}
			}
		}

		if (whitelisted || !blacklisted)
		{
			local_type_info_list.push_back(type_info);
		}
	}

	local_type_info_list.sort_custom<NodeTypeAlphaComparator>();

	load_favorite_types();
	load_recent_types();
}


String FlowScriptEditorNodeCreateDialog::create_type_tooltip_text(const FlowScriptNodeTypeInfo &p_type_info)
{
	return p_type_info.type_id + "\n\n" + p_type_info.type_description;
}


void FlowScriptEditorNodeCreateDialog::quick_access_type_list_gui_add_type(ItemList *p_item_list, const FlowScriptNodeTypeInfo &p_type_info)
{
	if (!p_type_info.enabled)
	{
		return;
	}

	const int item = p_item_list->add_item(p_type_info.type_name);
	p_item_list->set_item_metadata(item, p_type_info.to_dictionary());
	p_item_list->set_item_tooltip(item, create_type_tooltip_text(p_type_info));
	p_item_list->set_item_tooltip_enabled(item, true);
}


void FlowScriptEditorNodeCreateDialog::update_quick_access_type_list_gui(ItemList *p_item_list, const LocalVector<FlowScriptNodeTypeInfo> &p_type_info_list)
{
	p_item_list->clear();

	for (const FlowScriptNodeTypeInfo &type_info : p_type_info_list)
	{
		quick_access_type_list_gui_add_type(p_item_list, type_info);
	}
}


String FlowScriptEditorNodeCreateDialog::get_quick_access_type_list_filepath(const String &p_filename)
{
	return EditorPaths::get_singleton()->get_project_settings_dir().path_join(p_filename);
}


void FlowScriptEditorNodeCreateDialog::save_quick_access_type_list(const LocalVector<FlowScriptNodeTypeInfo> &p_type_info_list, const String &p_filename)
{
	const String filepath = get_quick_access_type_list_filepath(p_filename);
	Ref<FileAccess> file = FileAccess::open(filepath, FileAccess::WRITE);

	if (file.is_valid())
	{
		for (const FlowScriptNodeTypeInfo type_info : p_type_info_list)
		{
			if (type_info.type_id.is_valid_identifier() && EditorNode::get_editor_data().is_type_recognized(type_info.type_id))
			{
				file->store_line(type_info.type_id);
			}
		}
		file->close();
	}
}


void FlowScriptEditorNodeCreateDialog::load_quick_access_type_list(LocalVector<FlowScriptNodeTypeInfo> *p_type_info_list, const String &p_filename)
{
	p_type_info_list->clear();

	const String filepath = get_quick_access_type_list_filepath(p_filename);
	Ref<FileAccess> file = FileAccess::open(filepath, FileAccess::READ);

	if (file.is_valid())
	{
		while (!file->eof_reached())
		{
			const String type_id = file->get_line().strip_edges();
			if (type_id.is_valid_identifier() && EditorNode::get_editor_data().is_type_recognized(type_id))
			{
				const FlowScriptNodeTypeInfo type_info = FlowScriptNodeTypeDB::get_singleton()->get_type_by_class_name(type_id);
				if (type_info.enabled)
				{
					p_type_info_list->push_back(type_info);
				}
			}
		}
	}
}


void FlowScriptEditorNodeCreateDialog::save_favorite_types()
{
	save_quick_access_type_list(favorite_type_info_list, "favorites.FlowScriptNode");
}


void FlowScriptEditorNodeCreateDialog::load_favorite_types()
{
	load_quick_access_type_list(&favorite_type_info_list, "favorites.FlowScriptNode");
	update_quick_access_type_list_gui(item_list_favorites, favorite_type_info_list);
}


void FlowScriptEditorNodeCreateDialog::save_recent_types()
{
	save_quick_access_type_list(recent_type_info_list, "create_recent.FlowScriptNode");
}


void FlowScriptEditorNodeCreateDialog::load_recent_types()
{
	load_quick_access_type_list(&recent_type_info_list, "create_recent.FlowScriptNode");
	update_quick_access_type_list_gui(item_list_recents, recent_type_info_list);
}


void FlowScriptEditorNodeCreateDialog::choose_type(const FlowScriptNodeTypeInfo &p_type_info)
{
	ERR_FAIL_COND(!p_type_info.enabled);

	recent_type_info_list.erase(p_type_info);
	recent_type_info_list.insert(0, p_type_info);

	emit_signal(SNAME("type_chosen"), p_type_info.to_dictionary());
}


void FlowScriptEditorNodeCreateDialog::on_this_confirmed()
{
	choose_type(get_selected_type_info());
}


void FlowScriptEditorNodeCreateDialog::on_item_list_favorites_item_selected(const int p_item)
{
	const Dictionary type_info_dict = item_list_favorites->get_item_metadata(p_item);
	choose_type(FlowScriptNodeTypeInfo::create_from_dictionary(type_info_dict));
}


void FlowScriptEditorNodeCreateDialog::on_item_list_recents_item_selected(const int p_item)
{
	const Dictionary type_info_dict = item_list_recents->get_item_metadata(p_item);
	choose_type(FlowScriptNodeTypeInfo::create_from_dictionary(type_info_dict));
}


void FlowScriptEditorNodeCreateDialog::on_type_search_filter_line_gui_input(const Ref<InputEvent> &p_event)
{
	const Ref<InputEventKey> key_event = p_event;
	if (key_event.is_valid())
	{
		if (
				key_event->is_action(SNAME("ui_up"))
				|| key_event->is_action(SNAME("ui_down"))
				|| key_event->is_action(SNAME("ui_accept"))
		)
		{
			type_tree->gui_input(key_event);
			type_search_filter_line->accept_event();
		}
	}
}


void FlowScriptEditorNodeCreateDialog::on_type_search_filter_line_text_changed(const String &p_new_text)
{
	rebuild_type_tree_gui();
}


void FlowScriptEditorNodeCreateDialog::on_fold_action_menu_popup_id_pressed(const int p_id)
{
	if (p_id == FOLD_EXPAND_ALL)
	{
		expand_type_tree_gui();
	}
	else if (p_id == FOLD_COLLAPSE_ALL)
	{
		collapse_type_tree_gui();
	}
}


void FlowScriptEditorNodeCreateDialog::on_mark_type_favorite_button_toggled(const bool p_toggled_on)
{
	ERR_FAIL_COND(!is_any_type_selected());

	const FlowScriptNodeTypeInfo type_info = get_selected_type_info();
	
	if (p_toggled_on)
	{
		ERR_FAIL_COND(favorite_type_info_list.has(type_info));
		favorite_type_info_list.push_back(type_info);
		favorite_type_info_list.sort();
	}
	else
	{
		bool erase_ok = favorite_type_info_list.erase(type_info);
		ERR_FAIL_COND(!erase_ok);
	}
	
	update_quick_access_type_list_gui(item_list_favorites, favorite_type_info_list);
	update_mark_type_favorite_button();
}


void FlowScriptEditorNodeCreateDialog::on_type_tree_item_activated()
{
	const TreeItem *item = type_tree->get_selected();
	const Dictionary type_dict = item->get_metadata(0);
	if (!type_dict.is_empty())
	{
		const FlowScriptNodeTypeInfo type_info = FlowScriptNodeTypeInfo::create_from_dictionary(type_dict);
		choose_type(type_info);
	}
}


void FlowScriptEditorNodeCreateDialog::on_type_tree_item_selected()
{
	update_type_description();
}


void FlowScriptEditorNodeCreateDialog::on_type_tree_nothing_selected()
{
	update_type_description();
}


void FlowScriptEditorNodeCreateDialog::_notification(int p_what)
{
	if (p_what == NOTIFICATION_POSTINITIALIZE)
	{
		connect(SceneStringName(confirmed), callable_mp(this, &FlowScriptEditorNodeCreateDialog::on_this_confirmed));
	}
	else if (p_what == NOTIFICATION_VISIBILITY_CHANGED)
	{
		if (is_visible())
		{
			refresh_type_list();
			// I couldn't use callable_mp for some reason...
			type_search_filter_line->grab_focus();
		}
		else
		{
			save_favorite_types();
			save_recent_types();
		}
	}
	else if (p_what == NOTIFICATION_THEME_CHANGED)
	{
		const Ref<Texture2D> icon_favorite = get_editor_theme_icon(SNAME("Favorites"));
		const Ref<Texture2D> icon_tool = get_editor_theme_icon(SNAME("Tools"));

		mark_type_favorite_button->set_button_icon(icon_favorite);
		fold_action_menu_button->set_button_icon(icon_tool);
	}
}


void FlowScriptEditorNodeCreateDialog::_bind_methods()
{
	GLOBAL_DEF("flow_script/types/blacklist/nodes", PackedStringArray());
	GLOBAL_DEF("flow_script/types/blacklist/categories", PackedStringArray());
	GLOBAL_DEF("flow_script/types/whitelist/nodes", PackedStringArray());
	GLOBAL_DEF("flow_script/types/whitelist/categories", PackedStringArray());

	ADD_SIGNAL(MethodInfo("type_chosen", PropertyInfo(Variant::DICTIONARY, "type_info")));
}


FlowScriptEditorNodeCreateDialog::FlowScriptEditorNodeCreateDialog()
{
	set_flag(Window::FLAG_RESIZE_DISABLED, false);
	set_wrap_controls(true);
	set_min_size(Size2i((Size2(400, 250) * EDSCALE).round()));
	set_hide_on_ok(false);

	HSplitContainer *main_split = memnew(HSplitContainer);
	add_child(main_split);

	VSplitContainer *left_vsplit = memnew(VSplitContainer);
	main_split->add_child(left_vsplit);

	VBoxContainer *favorites_vbox = memnew(VBoxContainer);
	favorites_vbox->set_custom_minimum_size(Size2(150, 100) * EDSCALE);
	favorites_vbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	left_vsplit->add_child(favorites_vbox);

	item_list_favorites = memnew(ItemList);
	item_list_favorites->set_allow_reselect(true);
	item_list_favorites->connect("item_selected", callable_mp(this, &FlowScriptEditorNodeCreateDialog::on_item_list_favorites_item_selected));
	favorites_vbox->add_margin_child(TTR("Favorites:"), item_list_favorites, true);

	VBoxContainer *recents_vbox = memnew(VBoxContainer);
	recents_vbox->set_custom_minimum_size(Size2(150, 100) * EDSCALE);
	recents_vbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	left_vsplit->add_child(recents_vbox);

	item_list_recents = memnew(ItemList);
	item_list_recents->set_allow_reselect(true);
	item_list_recents->connect("item_selected", callable_mp(this, &FlowScriptEditorNodeCreateDialog::on_item_list_recents_item_selected));
	recents_vbox->add_margin_child(TTR("Recent:"), item_list_recents, true);

	VBoxContainer *right_vbox = memnew(VBoxContainer);
	right_vbox->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	right_vbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	right_vbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	right_vbox->set_focus_mode(Control::FOCUS_NONE);
	main_split->add_child(right_vbox);

	HBoxContainer *top_hbox = memnew(HBoxContainer);
	top_hbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	top_hbox->set_v_size_flags(Control::SIZE_SHRINK_BEGIN);
	right_vbox->add_child(top_hbox);

	type_search_filter_line = memnew(LineEdit);
	type_search_filter_line->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	type_search_filter_line->set_editable(true);
	type_search_filter_line->set_placeholder(TTR("Search"));
	type_search_filter_line->set_tooltip_text(TTR("Search for nodes by category or by name."));
	type_search_filter_line->connect(SceneStringName(gui_input), callable_mp(this, &FlowScriptEditorNodeCreateDialog::on_type_search_filter_line_gui_input));
	type_search_filter_line->connect(SNAME("text_changed"), callable_mp(this, &FlowScriptEditorNodeCreateDialog::on_type_search_filter_line_text_changed));
	top_hbox->add_child(type_search_filter_line);

	fold_action_menu_button = memnew(MenuButton);
	fold_action_menu_button->set_tooltip_text(TTR("Fold Actions"));
	top_hbox->add_child(fold_action_menu_button);

	PopupMenu *fold_action_menu_popup = fold_action_menu_button->get_popup();
	fold_action_menu_popup->add_item(TTR("Expand All"), FOLD_EXPAND_ALL);
	fold_action_menu_popup->add_item(TTR("Collapse All"), FOLD_COLLAPSE_ALL);
	fold_action_menu_popup->connect(SceneStringName(id_pressed), callable_mp(this, &FlowScriptEditorNodeCreateDialog::on_fold_action_menu_popup_id_pressed));

	mark_type_favorite_button = memnew(Button);
	mark_type_favorite_button->set_toggle_mode(true);
	mark_type_favorite_button->connect(SNAME("toggled"), callable_mp(this, &FlowScriptEditorNodeCreateDialog::on_mark_type_favorite_button_toggled));
	top_hbox->add_child(mark_type_favorite_button);

	type_tree = memnew(Tree);
	type_tree->set_focus_mode(Control::FOCUS_NONE);
	type_tree->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	type_tree->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	type_tree->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	type_tree->set_hide_root(true);
	type_tree->set_allow_reselect(true);
	type_tree->connect(SNAME("item_selected"), callable_mp(this, &FlowScriptEditorNodeCreateDialog::on_type_tree_item_selected));
	type_tree->connect(SNAME("item_activated"), callable_mp(this, &FlowScriptEditorNodeCreateDialog::on_type_tree_item_activated));
	right_vbox->add_child(type_tree);

	help_bit = memnew(EditorHelpBit);
	description_visibility_parent = right_vbox->add_margin_child(TTR("Description:"), help_bit, false);
	update_type_description();
}


void FlowScriptEditorNodeRenameDialog::set_new_node_name(const String &p_name)
{
	line_edit->set_text(p_name);
}


String FlowScriptEditorNodeRenameDialog::get_new_node_name() const
{
	return line_edit->get_text().strip_edges();
}


void FlowScriptEditorNodeRenameDialog::_notification(int p_what)
{
	if (p_what == NOTIFICATION_VISIBILITY_CHANGED)
	{
		if (is_visible())
		{
			line_edit->grab_focus();
		}
	}
}


FlowScriptEditorNodeRenameDialog::FlowScriptEditorNodeRenameDialog()
{
	set_title(TTR("Rename Node..."));

	line_edit = memnew(LineEdit);
	register_text_enter(line_edit);
}


void FlowScriptEditorSubIncludeNode::set_filename(const String &p_name)
{
	if (p_name == filename)
	{
		return;
	}

	filename = p_name;
	update_file_label();
}


String FlowScriptEditorSubIncludeNode::get_filename() const
{
	return filename;
}


void FlowScriptEditorSubIncludeNode::update_file_label()
{
	if (filename.is_empty())
	{
		file_label->set_text(TTR("No FlowScript"));
	}
	else
	{
		file_label->set_text(filename);
	}
}


FlowScriptEditorSubIncludeNode::FlowScriptEditorSubIncludeNode()
{
	set_mouse_filter(Control::MOUSE_FILTER_IGNORE);
	set_resizable(false);
	set_draggable(false);
	set_selectable(false);

	file_label = memnew(Label);
	update_file_label();
	add_child(file_label);

	set_slot_enabled_left(0, true);
}


Button *FlowScriptEditorIncludeFrame::get_edit_button() const
{
	return edit_button;
}


Button *FlowScriptEditorIncludeFrame::get_delete_button() const
{
	return delete_button;
}


void FlowScriptEditorIncludeFrame::set_include_id(const FlowScriptIncludeID p_id)
{
	include_id = p_id;
}


FlowScriptIncludeID FlowScriptEditorIncludeFrame::get_include_id() const
{
	return include_id;
}


void FlowScriptEditorIncludeFrame::on_edit_button_pressed()
{
	emit_signal(SNAME("edit_request"));
}


void FlowScriptEditorIncludeFrame::on_delete_button_pressed()
{
	emit_signal(SNAME("delete_request"));
}


void FlowScriptEditorIncludeFrame::_notification(int p_what)
{
	if (p_what == NOTIFICATION_THEME_CHANGED)
	{
		const Ref<Texture2D> icon_edit = get_editor_theme_icon(SNAME("Edit"));
		const Ref<Texture2D> icon_remove = get_editor_theme_icon(SNAME("Remove"));

		edit_button->set_button_icon(icon_edit);
		delete_button->set_button_icon(icon_remove);
	}
}


void FlowScriptEditorIncludeFrame::_bind_methods()
{
	ADD_SIGNAL(MethodInfo("edit_request"));
}


FlowScriptEditorIncludeFrame::FlowScriptEditorIncludeFrame()
{
	set_resizable(false);
	set_draggable(true);
	set_selectable(true);

	edit_button = memnew(Button);
	edit_button->set_tooltip_text(TTR("Edit included FlowScript."));
	edit_button->connect(SceneStringName(pressed), callable_mp(this, &FlowScriptEditorIncludeFrame::on_edit_button_pressed));
	get_titlebar_hbox()->add_child(edit_button);

	delete_button = memnew(Button);
	delete_button->set_tooltip_text(TTR("Delete Include."));
	delete_button->connect(SceneStringName(pressed), callable_mp(this, &FlowScriptEditorIncludeFrame::on_delete_button_pressed));
	get_titlebar_hbox()->add_child(delete_button);
}


bool FlowScriptEditorGraph::is_node_hover_valid(const StringName &p_from, const int p_from_port, const StringName &p_to, const int p_to_port)
{
	return callback_is_node_hover_valid.call(p_from, p_from_port, p_to, p_to_port);
}


void FlowScriptEditorGraph::init_dependencies(Callable p_callback_is_node_hover_valid)
{
	callback_is_node_hover_valid = p_callback_is_node_hover_valid;
}


void FlowScriptEditorGraph::sync_editor_settings()
{
	set_minimap_opacity(EDITOR_GET("editors/visual_editors/minimap_opacity"));
	set_grid_pattern((GraphEdit::GridPattern)(int) EDITOR_GET("editors/visual_editors/grid_pattern"));
	set_connection_lines_curvature(EDITOR_GET("editors/visual_editors/line_curvature"));

	get_panner()->setup((ViewPanner::ControlScheme)(int) EDITOR_GET("editors/panning/sub_editors_panning_scheme"), ED_GET_SHORTCUT("canvas_item_editor/pan_view"), EDITOR_GET("editors/panning/simple_panning"));
	set_warped_panning(EDITOR_GET("editors/panning/warped_mouse_panning"));
}


void FlowScriptEditorGraph::_notification(int p_what)
{
	if (p_what == NOTIFICATION_THEME_CHANGED)
	{
		sync_editor_settings();
	}
	else if (p_what == EditorSettings::NOTIFICATION_EDITOR_SETTINGS_CHANGED)
	{
			if (
					EditorSettings::get_singleton()->check_changed_settings_in_group("editors/visual_editors")
					|| EditorSettings::get_singleton()->check_changed_settings_in_group("editors/panning")
					|| EditorSettings::get_singleton()->check_changed_settings_in_group("canvas_item_editor")
			)
			{
				sync_editor_settings();
			}
	}
}


FlowScriptEditorGraph::FlowScriptEditorGraph()
{
	set_grid_pattern(GraphEdit::GRID_PATTERN_DOTS);
}


void FlowScriptEditorClipboard::clear()
{
	list_nodes.clear();
	list_connections.clear();
}


bool FlowScriptEditorClipboard::is_empty() const
{
	return list_nodes.is_empty();
}


int FlowScriptEditorClipboard::get_node_count() const
{
	return list_nodes.size();
}


FlowScriptEditorClipboard::Node FlowScriptEditorClipboard::get_node(const int p_index) const
{
	ERR_FAIL_INDEX_V(p_index, list_nodes.size(), Node());
	return list_nodes[p_index];
}


void FlowScriptEditorClipboard::add_node(const FlowScriptEditorClipboard::Node &p_node)
{
	list_nodes.push_back(p_node);
}


int FlowScriptEditorClipboard::get_connection_count() const
{
	return list_connections.size();
}


FlowScriptEditorClipboard::Connection FlowScriptEditorClipboard::get_connection(const int p_index) const
{
	ERR_FAIL_INDEX_V(p_index, list_connections.size(), Connection());
	return list_connections[p_index];
}


void FlowScriptEditorClipboard::add_connection(const FlowScriptEditorClipboard::Connection &p_connection)
{
	list_connections.push_back(p_connection);
}


void FlowScriptEditorDialogs::set_window_layout(Ref<ConfigFile> p_layout)
{
	const Size2 desired_node_create_dialog_size = EDSCALE * Size2(p_layout->get_value("FlowScriptEditor", "node_create_dialog_size", node_create_dialog->get_size()));
	node_create_dialog->set_size(Size2i(desired_node_create_dialog_size.round()));
}


void FlowScriptEditorDialogs::get_window_layout(Ref<ConfigFile> p_layout)
{
	const Size2i wnd_size_node_create_dialog = Size2(Size2(node_create_dialog->get_size()) / EDSCALE).round();
	p_layout->set_value("FlowScriptEditor", "node_create_dialog_size", wnd_size_node_create_dialog);
}


#if 0
void FlowScriptEditorDialogs::set_main_window(const Window *p_window)
{
	main_window = p_window;
}


const Window *FlowScriptEditorDialogs::get_main_window() const
{
	if (main_window == nullptr)
	{
		return EditorNode::get_singleton()->get_window();
	}
	else
	{
		return main_window;
	}
}
#endif // 0


void FlowScriptEditorDialogs::popup_create_node(Callable p_create_node_callback, const Ref<FlowScript> &p_flow_script, const FlowScriptNodeID p_node_id, const Point2 &p_at_position)
{
	cache_create_node_callback = p_create_node_callback;
	cache_create_node_target_flow_script = p_flow_script;
	cache_create_node_id = p_node_id;
	cache_create_node_position = p_at_position;

	Rect2i pop_rect;
	pop_rect.size = node_create_dialog->get_size();
	pop_rect.position = DisplayServer::get_singleton()->mouse_get_position() - (pop_rect.size / 2);
	EditorInterface::get_singleton()->popup_dialog(node_create_dialog, pop_rect);
}


void FlowScriptEditorDialogs::popup_rename_node(Callable p_rename_node_callback, const Ref<FlowScript> &p_flow_script, const FlowScriptNodeID p_node_id)
{
	cache_rename_node_callback = p_rename_node_callback;
	cache_rename_node_target_flow_script = p_flow_script;
	cache_rename_node_id = p_node_id;

	const String current_name = cache_rename_node_target_flow_script->get_node_name(p_node_id);
	node_rename_dialog->set_new_node_name(current_name);
	EditorInterface::get_singleton()->popup_dialog_centered(node_rename_dialog);
}


void FlowScriptEditorDialogs::popup_add_include(Callable p_add_include_callback, const Ref<FlowScript> &p_root_flow_script, const FlowScriptIncludeID p_include_id, const Point2 &p_at_position)
{
	cache_add_include_callback = p_add_include_callback;
	cache_add_include_target_flow_script = p_root_flow_script;
	cache_add_include_id = p_include_id;
	cache_add_include_position = p_at_position;

	TypedArray<StringName> filter;
	filter.push_back("FlowScript");
	EditorInterface::get_singleton()->popup_quick_open(callable_mp(this, &FlowScriptEditorDialogs::on_add_include_quick_open_path_selected), filter);
}


void FlowScriptEditorDialogs::confirm_close_unsaved_flow_script(Callable p_save_callback, Callable p_discard_callback, const Ref<FlowScript> &p_flow_script, const double p_last_save_time, const double p_last_edit_time)
{
	const String time_str_save = Time::get_singleton()->get_datetime_string_from_unix_time(p_last_save_time);
	const String time_str_edit = Time::get_singleton()->get_datetime_string_from_unix_time(p_last_edit_time);

	confirm_unsaved_flow_script_dialog->set_text(vformat(TTR("Close and save changes?\n\"%s\"\nLast saved: %s\nLast edited: %s"), p_flow_script->get_path(), time_str_save, time_str_edit));
	EditorInterface::get_singleton()->popup_dialog_centered(confirm_unsaved_flow_script_dialog);
}


void FlowScriptEditorDialogs::confirm_delete_elements(Callable p_delete_confirmed_callback, const Ref<FlowScript> &p_flow_script, const PackedFlowScriptIncludeIDArray &p_include_id_list, const PackedFlowScriptNodeIDArray &p_node_id_list)
{
	cache_delete_elements_callback = p_delete_confirmed_callback;
	cache_delete_elements_target_flow_script = p_flow_script;
	cache_delete_include_id_list = p_include_id_list;
	cache_delete_node_id_list = p_node_id_list;

	String new_window_title;
	String new_text;

	if (cache_delete_include_id_list.is_empty() && !cache_delete_node_id_list.is_empty())
	{
		new_window_title = TTRN("Delete Node?", "Delete Nodes?", cache_delete_node_id_list.size());
		new_text = vformat(TTRN("Are you sure you'd like to delete %d Node?", "Are you sure you'd like to delete %d Nodes?", cache_delete_node_id_list.size()), cache_delete_node_id_list.size());
	}
	else if (!cache_delete_include_id_list.is_empty() && cache_delete_node_id_list.is_empty())
	{
		new_window_title = TTRN("Delete Include?", "Delete Includes?", cache_delete_include_id_list.size());
		new_text = vformat(TTRN("Are you sure you'd like to delete %d Include?", "Are you sure you'd like to delete %d Includes?", cache_delete_include_id_list.size()), cache_delete_include_id_list.size());
	}
	else if (!cache_delete_include_id_list.is_empty() && !cache_delete_node_id_list.is_empty())
	{
		new_window_title = vformat(TTR("Delete %s and %s?"), TTRN("Include", "Includes", cache_delete_include_id_list.size()), TTRN("Node", "Nodes", cache_delete_node_id_list.size()));
		new_text = vformat(TTR("Are you sure you'd like to delete %s and %s?"), vformat(TTRN("%d Include", "%d Includes", cache_delete_include_id_list.size()), cache_delete_include_id_list.size()), vformat(TTRN("%d Node", "%d Nodes", cache_delete_node_id_list.size()), cache_delete_node_id_list.size()));
	}
	else
	{
		return;
	}

	confirm_delete_elements_dialog->set_title(new_window_title);
	confirm_delete_elements_dialog->set_text(new_text);
	EditorInterface::get_singleton()->popup_dialog_centered(confirm_delete_elements_dialog);
}


void FlowScriptEditorDialogs::on_node_create_dialog_type_chosen(const Dictionary &p_type_info)
{
	const FlowScriptNodeTypeInfo type_info = FlowScriptNodeTypeInfo::create_from_dictionary(p_type_info);

	if (!type_info.enabled || !cache_create_node_target_flow_script->is_node_slot_available(cache_create_node_id))
	{
		alert_cannot_add_node_dialog->set_text(vformat(TTR("Cannot add node of type \"%s\" to slot #%d."), type_info.type_id, cache_create_node_id));
		EditorInterface::get_singleton()->popup_dialog_centered(alert_cannot_add_node_dialog);
		return;
	}

	node_create_dialog->hide();
	cache_create_node_callback.call_deferred(cache_create_node_id, p_type_info, cache_create_node_position);
}


void FlowScriptEditorDialogs::on_node_rename_dialog_confirmed()
{
	const StringName new_name = node_rename_dialog->get_new_node_name();
	const StringName current_name = cache_rename_node_target_flow_script->get_node_name(cache_rename_node_id);

	if (new_name != current_name && !new_name.is_empty())
	{
		const FlowScript::NodeNameAddError err = cache_rename_node_target_flow_script->get_can_add_node_name_error(new_name);
		String err_str;

		if (err != FlowScript::ERR_NAME_OK)
		{
			switch (err)
			{
				case FlowScript::ERR_NAME_INVALID_IDENTIFIER: {
					err_str = vformat(TTR("New name \"%s\" is an invalid identifier."), new_name);
				} break;
				case FlowScript::ERR_NAME_TAKEN: {
					const FlowScriptNodeID taken_node_id = cache_rename_node_target_flow_script->get_node_id_by_name(new_name);
					err_str = vformat(TTR("New name \"%s\" is already associated with Node #%d."), new_name, taken_node_id);
				} break;
				case FlowScript::ERR_NAME_EMPTY: {
					err_str = TTR("New name is empty.");
				} break;
				default: {
					err_str = vformat(TTR("Node cannot be renamed to \"%s\"."), new_name);
				} break;
			}

			alert_cannot_rename_node_dialog->set_text(err_str);
			EditorInterface::get_singleton()->popup_dialog_centered(alert_cannot_rename_node_dialog);
			return;
		}
	}

	node_rename_dialog->hide();
	cache_rename_node_callback.call(cache_rename_node_id, new_name);
}


void FlowScriptEditorDialogs::on_confirm_unsaved_flow_script_dialog_confirmed()
{
	confirm_unsaved_flow_script_save_callback.call();
}


void FlowScriptEditorDialogs::on_confirm_unsaved_flow_script_dialog_custom_action(const StringName &p_action)
{
	if (p_action == SNAME("discard"))
	{
		confirm_unsaved_flow_script_discard_callback.call();
	}
}


void FlowScriptEditorDialogs::on_confirm_delete_elements_dialog_confirmed()
{
	confirm_delete_elements_dialog->hide();
	cache_delete_elements_callback.call(cache_delete_include_id_list, cache_delete_node_id_list);
}


void FlowScriptEditorDialogs::on_add_include_quick_open_path_selected(const String &p_path)
{
	const Ref<FlowScript> scr = ResourceLoader::load(p_path, "FlowScript");

	if (scr.is_null())
	{
		return;
	}

	const FlowScript::IncludeAddError err = cache_add_include_target_flow_script->get_can_include_flow_script_error(scr);

	if (err != FlowScript::ERR_INCLUDE_OK)
	{
		String err_str;
		switch (err)
		{
			case FlowScript::ERR_INCLUDE_NULL: {
				err_str = vformat(TTR("The FlowScript at \"%s\" failed to load."), p_path);
			} break;
			case FlowScript::ERR_INCLUDE_CIRCULAR_REFERENCE: {
				err_str = vformat(TTR("Cannot include the FlowScript at \"%s\" due to a circular dependency."), p_path);
			} break;
			case FlowScript::ERR_INCLUDE_SELF: {
				err_str = TTR("Attempted to include the FlowScript into itself.");
			} break;
			case FlowScript::ERR_INCLUDE_ALREADY_PRESENT: {
				FlowScriptIncludeID exist_incl_id = FlowScriptConstants::INCLUDE_ID_INVALID;
				for (const FlowScriptIncludeID curr_incl_id : cache_add_include_target_flow_script->get_include_id_list())
				{
					if (scr == cache_add_include_target_flow_script->get_include_flow_script(curr_incl_id))
					{
						exist_incl_id = curr_incl_id;
						break;
					}
				}
				err_str = vformat(TTR("The FlowScript at \"%s\" is already included in slot #%d."), p_path, exist_incl_id);
			} break;
			default: {
				err_str = vformat(TTR("Failed to include the FlowScript at \"%s\"."), p_path);
			} break;
		}
		alert_cannot_add_include_dialog->set_text(err_str);
		EditorInterface::get_singleton()->popup_dialog_centered(alert_cannot_add_include_dialog);
		return;
	}

	cache_add_include_callback.call(cache_add_include_id, scr, cache_add_include_position);
}


void FlowScriptEditorDialogs::on_alert_cannot_add_include_dialog_confirmed()
{
	alert_cannot_add_include_dialog->hide();
	popup_add_include(cache_add_include_callback, cache_add_include_target_flow_script, cache_add_include_id, cache_add_include_position);
}


FlowScriptEditorDialogs::FlowScriptEditorDialogs()
{
	node_create_dialog = memnew(FlowScriptEditorNodeCreateDialog);
	node_create_dialog->set_unparent_when_invisible(true);
	node_create_dialog->set_hide_on_ok(false);
	node_create_dialog->connect(SNAME("type_chosen"), callable_mp(this, &FlowScriptEditorDialogs::on_node_create_dialog_type_chosen));

	node_rename_dialog = memnew(FlowScriptEditorNodeRenameDialog);
	node_rename_dialog->set_unparent_when_invisible(true);
	node_rename_dialog->set_hide_on_ok(false);
	node_rename_dialog->connect(SceneStringName(confirmed), callable_mp(this, &FlowScriptEditorDialogs::on_node_rename_dialog_confirmed));

	confirm_unsaved_flow_script_dialog = memnew(ConfirmationDialog);
	confirm_unsaved_flow_script_dialog->set_unparent_when_invisible(true);
	confirm_unsaved_flow_script_dialog->set_ok_button_text(TTR("Save"));
	confirm_unsaved_flow_script_dialog->add_button(TTR("Discard"), DisplayServer::get_singleton()->get_swap_cancel_ok(), "discard");
	confirm_unsaved_flow_script_dialog->connect(SceneStringName(confirmed), callable_mp(this, &FlowScriptEditorDialogs::on_confirm_unsaved_flow_script_dialog_confirmed));
	confirm_unsaved_flow_script_dialog->connect(SNAME("custom_action"), callable_mp(this, &FlowScriptEditorDialogs::on_confirm_unsaved_flow_script_dialog_custom_action));

	confirm_delete_elements_dialog = memnew(ConfirmationDialog);
	confirm_delete_elements_dialog->set_unparent_when_invisible(true);
	confirm_delete_elements_dialog->set_title(TTR("Delete elements?"));

	alert_cannot_add_include_dialog = memnew(AcceptDialog);
	alert_cannot_add_include_dialog->set_unparent_when_invisible(true);
	alert_cannot_add_include_dialog->set_title(TTR("Cannot add Include!"));
	alert_cannot_add_include_dialog->connect(SceneStringName(confirmed), callable_mp(this, &FlowScriptEditorDialogs::on_alert_cannot_add_include_dialog_confirmed));

	alert_cannot_add_node_dialog = memnew(AcceptDialog);
	alert_cannot_add_node_dialog->set_unparent_when_invisible(true);
	alert_cannot_add_node_dialog->set_title(TTR("Cannot add Node!"));

	alert_cannot_rename_node_dialog = memnew(AcceptDialog);
	alert_cannot_rename_node_dialog->set_unparent_when_invisible(true);
	alert_cannot_rename_node_dialog->set_title(TTR("Cannot rename Node!"));
}


FlowScriptEditorDialogs::~FlowScriptEditorDialogs()
{
	node_create_dialog->queue_free();
	node_rename_dialog->queue_free();
	confirm_delete_elements_dialog->queue_free();
	alert_cannot_add_include_dialog->queue_free();
	alert_cannot_add_node_dialog->queue_free();
	alert_cannot_rename_node_dialog->queue_free();
}


void FlowScriptEditorCommonObject::edit_flow_script(Ref<FlowScript> p_flow_script)
{
	emit_signal(SNAME("edit_request"), p_flow_script);
}


void FlowScriptEditorCommonObject::init_dependencies(Ref<Theme> p_msdf_theme, FlowScriptEditorClipboard *p_clipboard, FlowScriptEditorDialogs *p_dialogs)
{
	msdf_theme = p_msdf_theme;
	clipboard = p_clipboard;
	dialogs = p_dialogs;
}


void FlowScriptEditorCommonObject::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("edit_flow_script"), &FlowScriptEditorCommonObject::edit_flow_script);

	ADD_SIGNAL(MethodInfo("edit_request", PropertyInfo(Variant::OBJECT, "flow_script", PROPERTY_HINT_RESOURCE_TYPE, "FlowScript")));
}


LocalVector<FlowScriptIncludeID> FlowScriptEditor::EditedInclude::get_editor_sub_include_id_list() const
{
	LocalVector<FlowScriptIncludeID> ret;
	ret.reserve(map_sub_include_nodes.size());
	for (const KeyValue<FlowScriptIncludeID, FlowScriptEditorSubIncludeNode *> &E : map_sub_include_nodes)
	{
		ret.push_back(E.key);
	}
	return ret;
}


LocalVector<FlowScriptNodeID> FlowScriptEditor::EditedInclude::get_editor_node_id_list() const
{
	LocalVector<FlowScriptNodeID> ret;
	ret.reserve(map_node_editors.size());
	for (const KeyValue<FlowScriptNodeID, FlowScriptNodeEditor *> &E : map_node_editors)
	{
		ret.push_back(E.key);
	}
	return ret;
}


Ref<FlowScript> FlowScriptEditor::get_flow_script() const
{
	return flow_script;
}


FlowScriptEditorGraph *FlowScriptEditor::get_graph() const
{
	return graph;
}


void FlowScriptEditor::init_dependencies(FlowScriptEditorCommonObject *p_common, Ref<FlowScript> p_flow_script)
{
	common = p_common;
	flow_script = p_flow_script;
	flow_script->connect_changed(callable_mp(this, &FlowScriptEditor::on_flow_script_changed));
}


void FlowScriptEditor::connect_extra_flow_script_signals()
{
	flow_script->connect(SNAME("include_changed"), callable_mp(this, &FlowScriptEditor::on_flow_script_include_changed));
	flow_script->connect(SNAME("include_rect_changed"), callable_mp(this, &FlowScriptEditor::on_flow_script_include_rect_changed));
	flow_script->connect(SNAME("include_added"), callable_mp(this, &FlowScriptEditor::on_flow_script_include_added));
	flow_script->connect(SNAME("removing_include"), callable_mp(this, &FlowScriptEditor::on_flow_script_removing_include));
	flow_script->connect(SNAME("include_removed"), callable_mp(this, &FlowScriptEditor::on_flow_script_include_removed));
	flow_script->connect(SNAME("node_changed"), callable_mp(this, &FlowScriptEditor::on_flow_script_node_changed));
	flow_script->connect(SNAME("node_renamed"), callable_mp(this, &FlowScriptEditor::on_flow_script_node_renamed));
	flow_script->connect(SNAME("node_rect_changed"), callable_mp(this, &FlowScriptEditor::on_flow_script_node_rect_changed));
	flow_script->connect(SNAME("node_connection_changed"), callable_mp(this, &FlowScriptEditor::on_flow_script_node_connection_changed));
	flow_script->connect(SNAME("node_added"), callable_mp(this, &FlowScriptEditor::on_flow_script_node_added));
	flow_script->connect(SNAME("removing_node"), callable_mp(this, &FlowScriptEditor::on_flow_script_removing_node));
	flow_script->connect(SNAME("node_removed"), callable_mp(this, &FlowScriptEditor::on_flow_script_node_removed));
}


void FlowScriptEditor::disconnect_extra_flow_script_signals()
{
	flow_script->disconnect(SNAME("include_changed"), callable_mp(this, &FlowScriptEditor::on_flow_script_include_changed));
	flow_script->disconnect(SNAME("include_rect_changed"), callable_mp(this, &FlowScriptEditor::on_flow_script_include_rect_changed));
	flow_script->disconnect(SNAME("include_added"), callable_mp(this, &FlowScriptEditor::on_flow_script_include_added));
	flow_script->disconnect(SNAME("removing_include"), callable_mp(this, &FlowScriptEditor::on_flow_script_removing_include));
	flow_script->disconnect(SNAME("include_removed"), callable_mp(this, &FlowScriptEditor::on_flow_script_include_removed));
	flow_script->disconnect(SNAME("node_changed"), callable_mp(this, &FlowScriptEditor::on_flow_script_node_changed));
	flow_script->disconnect(SNAME("node_renamed"), callable_mp(this, &FlowScriptEditor::on_flow_script_node_renamed));
	flow_script->disconnect(SNAME("node_rect_changed"), callable_mp(this, &FlowScriptEditor::on_flow_script_node_rect_changed));
	flow_script->disconnect(SNAME("node_connection_changed"), callable_mp(this, &FlowScriptEditor::on_flow_script_node_connection_changed));
	flow_script->disconnect(SNAME("node_added"), callable_mp(this, &FlowScriptEditor::on_flow_script_node_added));
	flow_script->disconnect(SNAME("removing_node"), callable_mp(this, &FlowScriptEditor::on_flow_script_removing_node));
	flow_script->disconnect(SNAME("node_removed"), callable_mp(this, &FlowScriptEditor::on_flow_script_node_removed));
}


#if 0
void FlowScriptEditor::draw_node_connections_create(const FlowScriptNodeReference &p_from)
{
	ERR_FAIL_COND(!flow_script->has_target(p_from));

	if (p_from.include_id == FlowScriptConstants::INCLUDE_ID_INVALID)
	{
		ERR_FAIL_COND(!map_node_editors.has(p_from.node_id));

		FlowScriptNodeEditor *from_editor = map_node_editors[p_from.node_id];

		const FlowScriptNodeConnectionListNo list_count = flow_script->get_node_connection_list_count(p_from.node_id);
		for (FlowScriptNodeConnectionListNo list_no = 0; list_no < list_count; list_no++)
		{
			const FlowScriptNodeConnectionListLength slot_count = flow_script->get_node_connection_list_length(p_from.node_id, list_no);
			for (FlowScriptNodeConnectionListSlotNo slot_no = 0; slot_no < slot_count; slot_no++)
			{
				const FlowScriptNodeOutputConnection output = FlowScriptNodeOutputConnection::create_connection(list_no, slot_no);
				const FlowScriptNodeReference target = flow_script->get_node_connection(p_from.node_id, output);

				if (target.node_id == FlowScriptConstants::NODE_ID_INVALID)
				{
					continue;
				}

				FlowScriptNodeEditor *target_editor;

				if (target.include_id == FlowScriptConstants::INCLUDE_ID_INVALID)
				{
					ERR_CONTINUE(!map_node_editors.has(target.node_id));
					target_editor = map_node_editors[target.node_id];
				}
				else
				{
					ERR_CONTINUE(!map_include_editors.has(p_from.include_id));
					ERR_CONTINUE(!map_include_editors[target.include_id].map_node_editors.has(target.node_id));
					target_editor = map_include_editors[target.include_id].map_node_editors[target.node_id];
				}

				const int output_slot = from_editor->output_connection_to_graph_slot(output);
				const int input_slot = target_editor->get_input_slot();
				ERR_CONTINUE(!from_editor->is_slot_enabled_right(output_slot));
				ERR_CONTINUE(!target_editor->is_slot_enabled_left(input_slot));
				const int output_port = from_editor->get_slot_port_index_right(output_slot);
				const int input_port = target_editor->get_slot_port_index_left(input_slot);

				graph->connect_node(from_editor->get_name(), output_port, target_editor->get_name(), input_port);
			}
		}
	}
	else
	{
		const Ref<FlowScript> include_flow_script = flow_script->get_include_flow_script(p_from.include_id);

		ERR_FAIL_COND(!map_include_editors.has(p_from.include_id));
		EditedInclude &include_editor = map_include_editors[p_from.include_id];
		ERR_FAIL_COND(!include_editor.map_node_editors.has(p_from.node_id));
		FlowScriptNodeEditor *from_editor = include_editor.map_node_editors[p_from.node_id];

		const FlowScriptNodeConnectionListNo list_count = include_flow_script->get_node_connection_list_count(p_from.node_id);
		for (FlowScriptNodeConnectionListNo list_no = 0; list_no < list_count; list_no++)
		{
			const FlowScriptNodeConnectionListLength slot_count = flow_script->get_node_connection_list_length(p_from.node_id, list_no);
			for (FlowScriptNodeConnectionListSlotNo slot_no = 0; slot_no < slot_count; slot_no++)
			{
				const FlowScriptNodeOutputConnection output = FlowScriptNodeOutputConnection::create_connection(list_no, slot_no);
				const FlowScriptNodeReference target = include_flow_script->get_node_connection(p_from.node_id, output);

				if (target.node_id == FlowScriptConstants::NODE_ID_INVALID)
				{
					continue;
				}

				const int output_slot = from_editor->output_connection_to_graph_slot(output);
				ERR_CONTINUE(!from_editor->is_slot_enabled_right(output_slot));
				const int output_port = from_editor->get_slot_port_index_right(output_slot);

				if (target.include_id == FlowScriptConstants::INCLUDE_ID_INVALID)
				{
					ERR_CONTINUE(!include_editor.map_node_editors.has(target.node_id));
					FlowScriptNodeEditor *target_editor = include_editor.map_node_editors[target.node_id];
					const int input_slot = target_editor->get_input_slot();
					ERR_CONTINUE(!target_editor->is_slot_enabled_left(input_slot));
					const int input_port = target_editor->get_slot_port_index_left(input_slot);
					graph->connect_node(from_editor->get_name(), output_port, target_editor->get_name(), input_port);
				}
				else
				{
					ERR_CONTINUE(!include_editor.map_sub_include_nodes.has(target.include_id));
					FlowScriptEditorSubIncludeNode *target_graph_node = include_editor.map_sub_include_nodes[target.include_id];
					graph->connect_node(from_editor->get_name(), output_port, target_graph_node->get_name(), FlowScriptEditorSubIncludeNode::ANON_INPUT_PORT);
				}
			}
		}
	}
}
#endif // 0


void FlowScriptEditor::draw_connections()
{
	for (const KeyValue<FlowScriptIncludeID, EditedInclude> &incl_kv : map_include_editors)
	{
		const FlowScriptIncludeID incl_id = incl_kv.key;
		ERR_CONTINUE(!flow_script->has_include(incl_id));
		const Ref<FlowScript> incl_flow_script = flow_script->get_include_flow_script(incl_id);
		ERR_CONTINUE(incl_flow_script.is_null());

		const EditedInclude &incl = incl_kv.value;

		for (const KeyValue<FlowScriptNodeID, FlowScriptNodeEditor *> &node_kv : incl.map_node_editors)
		{
			const FlowScriptNodeID node_id = node_kv.key;
			const FlowScriptNodeEditor *node_editor = node_kv.value;

			const FlowScriptNodeConnectionListNo list_count = incl_flow_script->get_node_connection_list_count(node_id);
			for (FlowScriptNodeConnectionListNo list_no = 0; list_no < list_count; list_no++)
			{
				const FlowScriptNodeConnectionListLength slot_count = incl_flow_script->get_node_connection_list_length(node_id, list_no);
				for (FlowScriptNodeConnectionListSlotNo slot_no = 0; slot_no < slot_count; slot_no++)
				{
					const FlowScriptNodeOutputConnection output = FlowScriptNodeOutputConnection::create_connection(list_no, slot_no);
					const FlowScriptNodeReference target = incl_flow_script->get_node_connection(node_id, output);

					if (target.node_id == FlowScriptConstants::NODE_ID_INVALID)
					{
						continue;
					}

					const int output_slot = node_editor->output_connection_to_graph_slot(output);
					ERR_CONTINUE(!node_editor->is_slot_enabled_right(output_slot));
					const int output_port = node_editor->get_slot_port_index_right(output_slot);

					if (target.include_id == FlowScriptConstants::INCLUDE_ID_INVALID)
					{
						ERR_CONTINUE(!incl.map_node_editors.has(target.node_id));
						FlowScriptNodeEditor *target_editor = incl.map_node_editors[target.node_id];

						const int input_slot = target_editor->get_input_slot();
						ERR_CONTINUE(!target_editor->is_slot_enabled_left(input_slot));
						const int input_port = target_editor->get_slot_port_index_left(input_slot);
						graph->connect_node(node_editor->get_name(), output_port, target_editor->get_name(), input_port);
					}
					else
					{
						ERR_CONTINUE(!incl.map_sub_include_nodes.has(target.include_id));
						graph->connect_node(node_editor->get_name(), output_port, incl.map_sub_include_nodes[target.include_id]->get_name(), FlowScriptEditorSubIncludeNode::ANON_INPUT_PORT);
					}
				}
			}
		}
	}

	for (const KeyValue<FlowScriptNodeID, FlowScriptNodeEditor *> &node_kv : map_node_editors)
	{
		const FlowScriptNodeID node_id = node_kv.key;
		const FlowScriptNodeEditor *node_editor = node_kv.value;

		const FlowScriptNodeConnectionListNo list_count = flow_script->get_node_connection_list_count(node_id);
		for (FlowScriptNodeConnectionListNo list_no = 0; list_no < list_count; list_no++)
		{
			const FlowScriptNodeConnectionListLength slot_count = flow_script->get_node_connection_list_length(node_id, list_no);
			for (FlowScriptNodeConnectionListSlotNo slot_no = 0; slot_no < slot_count; slot_no++)
			{
				const FlowScriptNodeOutputConnection output = FlowScriptNodeOutputConnection::create_connection(list_no, slot_no);
				const FlowScriptNodeReference target = flow_script->get_node_connection(node_id, output);

				if (target.node_id == FlowScriptConstants::NODE_ID_INVALID)
				{
					continue;
				}

				FlowScriptNodeEditor *target_editor;

				if (target.include_id == FlowScriptConstants::INCLUDE_ID_INVALID)
				{
					ERR_CONTINUE(!map_node_editors.has(target.node_id));
					target_editor = map_node_editors[target.node_id];
				}
				else
				{
					ERR_CONTINUE(!map_include_editors.has(target.include_id));
					ERR_CONTINUE(!map_include_editors[target.include_id].map_node_editors.has(target.node_id));
					target_editor = map_include_editors[target.include_id].map_node_editors[target.node_id];
				}

				const int output_slot = node_editor->output_connection_to_graph_slot(output);
				const int input_slot = target_editor->get_input_slot();
				ERR_CONTINUE(!node_editor->is_slot_enabled_right(output_slot));
				ERR_CONTINUE(!target_editor->is_slot_enabled_left(input_slot));
				const int output_port = node_editor->get_slot_port_index_right(output_slot);
				const int input_port = target_editor->get_slot_port_index_left(input_slot);

				graph->connect_node(node_editor->get_name(), output_port, target_editor->get_name(), input_port);
			}
		}
	}
}


void FlowScriptEditor::update_editor_node_rect(const FlowScriptNodeReference &p_target)
{
	FlowScriptNodeEditor *node_editor = get_node_editor_by_reference(p_target);
	ERR_FAIL_NULL(node_editor);

	Ref<FlowScriptNode> node_data;
	Rect2i data_rect;

	if (p_target.include_id == FlowScriptConstants::INCLUDE_ID_INVALID)
	{
		ERR_FAIL_COND(!flow_script->has_node(p_target.node_id));
		node_data = flow_script->get_node_data(p_target.node_id);
		data_rect = flow_script->get_node_rect(p_target.node_id);
	}
	else
	{
		ERR_FAIL_COND(!flow_script->has_include(p_target.include_id));
		const Ref<FlowScript> incl_data = flow_script->get_include_flow_script(p_target.include_id);
		ERR_FAIL_COND(!incl_data->has_node(p_target.node_id));
		node_data = incl_data->get_node_data(p_target.node_id);
		data_rect = incl_data->get_node_rect(p_target.node_id);
		data_rect.position += flow_script->get_include_position(p_target.include_id);
	}

	if (node_editor->get_type_info().editable_size)
	{
		node_editor->set_size(EDSCALE * (Size2)data_rect.size);
	}
	else
	{
		const Size2 curr_size = node_editor->get_size();
		const Size2 min_size = node_editor->get_combined_minimum_size();

		if (!curr_size.is_equal_approx(min_size))
		{
			node_editor->set_size(node_editor->get_combined_minimum_size());
		}
	}

	const Point2 target_pos = convert_point_data_to_graph(data_rect.position) - (node_editor->get_size() * 0.5);
	node_editor->set_position_offset(target_pos);
}


void FlowScriptEditor::update_editor_node(const FlowScriptNodeReference &p_target)
{
	FlowScriptNodeEditor *node_editor = get_node_editor_by_reference(p_target);
	ERR_FAIL_NULL(node_editor);

	const FlowScriptNodeTypeInfo type_info = node_editor->get_type_info();
	const String node_name = flow_script->get_node_name(p_target.node_id);

	String new_title;
	PackedStringArray new_tooltip_text_lines;

	if (node_name.is_empty())
	{
		new_title = type_info.type_name;
	}
	else
	{
		new_title = vformat("%s - %s", type_info.type_name, node_name);
		new_tooltip_text_lines.push_back(vformat(TTR("Name: %s"), node_name));
	}

	if (p_target.include_id == FlowScriptConstants::INCLUDE_ID_INVALID)
	{
		new_tooltip_text_lines.push_back(vformat(TTR("Root FlowScript - Node #%d"), p_target.node_id));
	}
	else
	{
		new_tooltip_text_lines.push_back(vformat(TTR("Include FlowScript #%d - Node #%d"), p_target.include_id, p_target.node_id));
	}

	new_tooltip_text_lines.push_back(vformat(TTR("Type: %s"), type_info.type_id));
	new_tooltip_text_lines.push_back(vformat(TTR("Description:\n%s"), type_info.type_description));

	node_editor->set_title(new_title);
	node_editor->set_tooltip_text(String::chr('\n').join(new_tooltip_text_lines));
	node_editor->sync();

	update_editor_node_rect(p_target);
}


void FlowScriptEditor::reload_include(const FlowScriptIncludeID p_include_id)
{
	if (map_include_editors.has(p_include_id))
	{
		delete_edited_include(p_include_id);
	}

	if (!flow_script->has_include(p_include_id))
	{
		return;
	}

	const Ref<FlowScript> incl_flow_script = flow_script->get_include_flow_script(p_include_id);

	EditedInclude incl = EditedInclude();

	incl.frame = memnew(FlowScriptEditorIncludeFrame);
	// incl.frame->set_name("include_" + itos(p_include_id));
	incl.frame->set_title(vformat(TTR("Include - %s"), p_include_id, incl_flow_script->get_path()));
	incl.frame->set_tooltip_text(vformat(TTR("Include #%d\nPath: %s\nContains %d Nodes\nContains %d Sub-Includes"), p_include_id, incl_flow_script->get_path(), incl_flow_script->get_node_count(), incl_flow_script->get_include_count()));
	incl.frame->set_include_id(p_include_id);

	incl.frame->connect(SNAME("delete_request"), callable_mp(this, &FlowScriptEditor::on_include_delete_request).bind(p_include_id));
	incl.frame->connect(SNAME("dragged"), callable_mp(this, &FlowScriptEditor::on_include_dragged).bind(p_include_id));
	incl.frame->connect(SNAME("deselected"), callable_mp(this, &FlowScriptEditor::on_include_deselected).bind(p_include_id));
	incl.frame->connect(SNAME("selected"), callable_mp(this, &FlowScriptEditor::on_include_selected).bind(p_include_id));
	incl.frame->connect(SNAME("position_offset_changed"), callable_mp(this, &FlowScriptEditor::on_include_position_offset_changed).bind(p_include_id));
	incl.frame->connect(SNAME("raise_request"), callable_mp(this, &FlowScriptEditor::on_include_raise_request).bind(p_include_id));
	incl.frame->connect(SNAME("resize_end"), callable_mp(this, &FlowScriptEditor::on_include_resize_end).bind(p_include_id));
	incl.frame->connect(SNAME("resize_request"), callable_mp(this, &FlowScriptEditor::on_include_resize_request).bind(p_include_id));
	incl.frame->connect(SNAME("edit_request"), callable_mp(this, &FlowScriptEditor::on_include_edit_request).bind(p_include_id));

	graph->add_child(incl.frame);
	incl.frame->set_position_offset(convert_point_data_to_graph(flow_script->get_include_position(p_include_id)) + (incl.frame->get_size() * 0.5));

	for (const FlowScriptIncludeID sub_include_id : incl_flow_script->get_include_id_list())
	{
		FlowScriptEditorSubIncludeNode *sub_incl_node = memnew(FlowScriptEditorSubIncludeNode);
		incl.map_sub_include_nodes.insert(sub_include_id, sub_incl_node);
		sub_incl_node->set_theme(common->get_msdf_theme());
		// sub_incl_node->set_name("include_" + itos(p_include_id) + "_sub_" + itos(sub_include_id));
		sub_incl_node->set_title(vformat(TTR("Sub-Include #%d"), sub_include_id));
		sub_incl_node->set_tooltip_text(vformat(TTR("Sub-Include #%d\nPath: %s"), sub_include_id, incl_flow_script->get_include_flow_script(sub_include_id)->get_path()));
		sub_incl_node->set_filename(incl_flow_script->get_include_flow_script(sub_include_id)->get_path().get_file());
		graph->add_child(sub_incl_node);
		graph->attach_graph_element_to_frame(sub_incl_node->get_name(), incl.frame->get_name());
	}

	map_include_editors.insert(p_include_id, incl);

	for (const FlowScriptNodeID node_id : incl_flow_script->get_node_id_list())
	{
		create_node_editor_instance(FlowScriptNodeReference::create_include_script_reference(p_include_id, node_id));
	}
}


void FlowScriptEditor::delete_edited_include(const FlowScriptIncludeID p_include_id)
{
	ERR_FAIL_COND(!map_include_editors.has(p_include_id));

	EditedInclude &incl = map_include_editors[p_include_id];

	for (const FlowScriptNodeID curr_node_id : incl.get_editor_node_id_list())
	{
		delete_node_editor_instance(FlowScriptNodeReference::create_include_script_reference(p_include_id, curr_node_id));
	}

	for (const FlowScriptIncludeID curr_sub_include_id : incl.get_editor_sub_include_id_list())
	{
		FlowScriptEditorSubIncludeNode *sub_include_node_editor = incl.map_sub_include_nodes[curr_sub_include_id];
		graph->detach_graph_element_from_frame(sub_include_node_editor->get_name());
		graph->remove_child(sub_include_node_editor);
		sub_include_node_editor->queue_free();
	}

	graph->remove_child(incl.frame);
	incl.frame->queue_free();

	map_include_editors.erase(p_include_id);
}


void FlowScriptEditor::request_create_node_at_position(const Point2 &p_position)
{
	FlowScriptNodeID node_id = flow_script->get_first_available_node_slot();
	ERR_FAIL_COND(node_id == FlowScriptConstants::NODE_ID_INVALID);

	common->get_dialogs()->popup_create_node(callable_mp(this, &FlowScriptEditor::hook_create_node), flow_script, node_id, p_position);
}


void FlowScriptEditor::request_instantiate_include_at_position(const Point2 &p_position)
{
	FlowScriptIncludeID include_id = flow_script->get_first_available_include_slot();
	ERR_FAIL_COND(include_id == FlowScriptConstants::INCLUDE_ID_INVALID);

	common->get_dialogs()->popup_add_include(callable_mp(this, &FlowScriptEditor::hook_add_include), flow_script, include_id, p_position);
}


FlowScriptNodeEditor *FlowScriptEditor::create_node_editor_instance(const FlowScriptNodeReference &p_target)
{
	ERR_FAIL_COND_V(!flow_script->has_target(p_target), nullptr);

	const Ref<FlowScript> owner_flow_script = p_target.include_id == FlowScriptConstants::INCLUDE_ID_INVALID ? flow_script : flow_script->get_include_flow_script(p_target.include_id);
	const Ref<FlowScriptNode> node_data = owner_flow_script->get_node_data(p_target.node_id);
	const FlowScriptNodeTypeInfo type_info = FlowScriptNodeTypeDB::get_singleton()->get_type_of_node(node_data);

	FlowScriptNodeEditor *node_editor = nullptr;

	if (type_info.enabled)
	{
		node_editor = FlowScriptNodeTypeDB::get_singleton()->create_editor_for_node(node_data);
	}
	if (node_editor == nullptr)
	{
		vformat(TTR("Failed to instantiate editor for node (%d, %d); creating placeholder node editor instead."), p_target.include_id, p_target.node_id);
		node_editor = memnew(FlowScriptNodeEditorPlaceholder);
	}

	// Do NOT pass owner_flow_script here.
	node_editor->init_dependencies(type_info, flow_script, p_target, common->get_msdf_theme());
	graph->add_child(node_editor);

	if (p_target.include_id == FlowScriptConstants::INCLUDE_ID_INVALID)
	{
		DEV_ASSERT(!map_node_editors.has(p_target.node_id));

		map_node_editors.insert(p_target.node_id, node_editor);
		// node_editor->set_name("node_" + itos(p_target.node_id));
		node_editor->startup();
		update_editor_node(p_target);
	}
	else
	{
		DEV_ASSERT(map_include_editors.has(p_target.include_id));

		EditedInclude &incl = map_include_editors[p_target.include_id];
		DEV_ASSERT(!incl.map_node_editors.has(p_target.node_id));

		incl.map_node_editors.insert(p_target.node_id, node_editor);
		// node_editor->set_name("include_" + itos(p_target.include_id) + "_node_" + itos(p_target.node_id));
		make_element_unclickable(node_editor);
		node_editor->startup();
		update_editor_node(p_target);
		graph->attach_graph_element_to_frame(node_editor->get_name(), incl.frame->get_name());
	}

	return node_editor;
}


void FlowScriptEditor::delete_node_editor_instance(const FlowScriptNodeReference &p_target)
{
	FlowScriptNodeEditor *node_editor;

	if (p_target.include_id == FlowScriptConstants::INCLUDE_ID_INVALID)
	{
		DEV_ASSERT(map_node_editors.has(p_target.node_id));

		node_editor = map_node_editors[p_target.node_id];
	}
	else
	{
		DEV_ASSERT(map_include_editors.has(p_target.include_id));

		EditedInclude &incl = map_include_editors[p_target.include_id];
		DEV_ASSERT(incl.map_node_editors.has(p_target.node_id));

		node_editor = incl.map_node_editors[p_target.node_id];
		graph->detach_graph_element_from_frame(node_editor->get_name());
	}

	node_editor->cleanup();
	graph->remove_child(node_editor);
	node_editor->queue_free();
}


bool FlowScriptEditor::should_element_deletion_require_confirmation(const PackedFlowScriptIncludeIDArray &p_include_id_list, const PackedFlowScriptNodeIDArray &p_node_id_list) const
{
	if (!p_include_id_list.is_empty())
	{
		return true;
	}
	if (p_node_id_list.size() > 1)
	{
		return true;
	}
	for (const FlowScriptNodeID node_id : p_node_id_list)
	{
		if (flow_script->get_node_name(node_id) != StringName())
		{
			return true;
		}
	}
	return false;
}


Object *FlowScriptEditor::get_undo_redo_context() const
{
	// Maybe this should return the FlowScript, maybe it shouldn't.
	// I'm of the opinion that it shouldn't, as changing the FlowScript from a scene's subresource to a resource on-disk would break the undo/redo history, and vice versa.
	return nullptr;
}


EditorUndoRedoManager *FlowScriptEditor::get_undo_redo() const
{
	return EditorUndoRedoManager::get_singleton();
}


void FlowScriptEditor::create_undo_redo_action(const String &p_name, const UndoRedo::MergeMode p_mode, const bool p_backwards_undo_ops)
{
	EditorUndoRedoManager *udrd = get_undo_redo();
	DEV_ASSERT(udrd != nullptr);

	udrd->create_action(p_name, p_mode, get_undo_redo_context(), p_backwards_undo_ops);
	udrd->add_do_method(common, SNAME("edit_flow_script"), flow_script);
	udrd->add_undo_method(common, SNAME("edit_flow_script"), flow_script);
}


void FlowScriptEditor::commit_undo_redo_action()
{
	op_begin();
	EditorUndoRedoManager *udrd = get_undo_redo();
	udrd->commit_action(true);
	op_end();
}


void FlowScriptEditor::op_begin()
{
	op_active_counter++;
}


void FlowScriptEditor::op_end()
{
	DEV_ASSERT(op_active_counter > 0);
	op_active_counter--;
}


bool FlowScriptEditor::is_op_active() const
{
	return op_active_counter > 0;
}


LocalVector<FlowScriptIncludeID> FlowScriptEditor::get_editor_include_id_list() const
{
	LocalVector<FlowScriptIncludeID> ret;
	ret.reserve(map_include_editors.size());
	for (const KeyValue<FlowScriptIncludeID, EditedInclude> &E : map_include_editors)
	{
		ret.push_back(E.key);
	}
	return ret;
}


LocalVector<FlowScriptNodeID> FlowScriptEditor::get_editor_node_id_list() const
{
	LocalVector<FlowScriptNodeID> ret;
	ret.reserve(map_node_editors.size());
	for (const KeyValue<FlowScriptNodeID, FlowScriptNodeEditor *> &E : map_node_editors)
	{
		ret.push_back(E.key);
	}
	return ret;
}


void FlowScriptEditor::queue_update_editor()
{
	if (!editor_update_queued)
	{
		editor_update_queued = true;
		if (is_visible_in_tree())
		{
			callable_mp(this, &FlowScriptEditor::immediate_update_editor).call_deferred();
		}
	}
}


void FlowScriptEditor::immediate_update_editor()
{
	editor_update_queued = false;

	bool should_redraw_connections = (
			!rf_queue_change_include_set.is_empty()
			|| !rf_queue_remove_include_set.is_empty()
			|| !rf_queue_remove_node_set.is_empty()
			|| !rf_queue_connection_create_set.is_empty()
			|| !rf_queue_connection_break_set.is_empty()
	);

	if (should_redraw_connections)
	{
		graph->clear_connections();
	}

	for (const FlowScriptNodeID node_id : rf_queue_remove_node_set)
	{
		if (map_node_editors.has(node_id))
		{
			delete_node_editor_instance(FlowScriptNodeReference::create_same_script_reference(node_id));
			map_node_editors.erase(node_id);
		}
	}

	for (const FlowScriptIncludeID include_id : rf_queue_remove_include_set)
	{
		if (map_include_editors.has(include_id))
		{
			delete_edited_include(include_id);
		}
	}

	for (const FlowScriptIncludeID include_id : rf_queue_change_include_set)
	{
		if (flow_script->has_include(include_id))
		{
			reload_include(include_id);
		}
	}

	for (const FlowScriptNodeID node_id : rf_queue_change_node_set)
	{
		if (flow_script->has_node(node_id))
		{
			create_node_editor_instance(FlowScriptNodeReference::create_same_script_reference(node_id));
		}
	}

	if (should_redraw_connections)
	{
		draw_connections();
	}

	rf_queue_change_include_set.clear();
	rf_queue_remove_include_set.clear();
	rf_queue_change_node_set.clear();
	rf_queue_remove_node_set.clear();
	rf_queue_connection_create_set.clear();
	rf_queue_connection_break_set.clear();
}


void FlowScriptEditor::total_refresh_editor()
{
	editor_total_refresh_queued = false;

	graph->clear_connections();

	for (const FlowScriptNodeID curr_node_id : get_editor_node_id_list())
	{
		delete_node_editor_instance(FlowScriptNodeReference::create_same_script_reference(curr_node_id));
	}

	for (const FlowScriptIncludeID curr_include_id : get_editor_include_id_list())
	{
		delete_edited_include(curr_include_id);
	}

	for (const FlowScriptIncludeID curr_include_id : flow_script->get_include_id_list())
	{
		reload_include(curr_include_id);
	}

	for (const FlowScriptNodeID curr_node_id : flow_script->get_node_id_list())
	{
		create_node_editor_instance(FlowScriptNodeReference::create_same_script_reference(curr_node_id));
	}
}


FlowScriptNodeEditor *FlowScriptEditor::get_node_editor_by_reference(const FlowScriptNodeReference &p_node) const
{
	if (p_node.include_id == FlowScriptConstants::INCLUDE_ID_INVALID)
	{
		ERR_FAIL_COND_V(!map_node_editors.has(p_node.node_id), nullptr);
		return map_node_editors[p_node.node_id];
	}
	else
	{
		ERR_FAIL_COND_V(!map_include_editors.has(p_node.include_id), nullptr);
		const EditedInclude &incl = map_include_editors[p_node.include_id];
		ERR_FAIL_COND_V(!incl.map_node_editors.has(p_node.node_id), nullptr);
		return incl.map_node_editors[p_node.node_id];
	}
}


void FlowScriptEditor::get_selected_element_id_list(List<FlowScriptIncludeID> *p_include_id_list, List<FlowScriptNodeID> *p_node_id_list) const
{
	if (p_include_id_list != nullptr)
	{
		for (const KeyValue<FlowScriptIncludeID, EditedInclude> &E : map_include_editors)
		{
			if (E.value.frame->is_selected())
			{
				p_include_id_list->push_back(E.key);
			}
		}
	}

	if (p_node_id_list != nullptr)
	{
		for (const KeyValue<FlowScriptNodeID, FlowScriptNodeEditor *> &E : map_node_editors)
		{
			if (E.value->is_selected())
			{
				p_node_id_list->push_back(E.key);
			}
		}
	}
}


void FlowScriptEditor::get_selected_include_id_list(List<FlowScriptIncludeID> *p_list) const
{
	get_selected_element_id_list(p_list, nullptr);
}


void FlowScriptEditor::get_selected_node_id_list(List<FlowScriptNodeID> *p_list) const
{
	get_selected_element_id_list(nullptr, p_list);
}


void FlowScriptEditor::push_node_set_to_clipboard(const HashSet<FlowScriptNodeID> &p_node_id_set, const bool p_clear, FlowScriptEditorClipboard *p_target_clipboard)
{
	if (p_clear)
	{
		p_target_clipboard->clear();
	}

	HashMap<FlowScriptNodeID, int> node_id_to_clip_index;
	int curr_node_clip_index = p_target_clipboard->get_node_count();

	for (const FlowScriptNodeID curr_node_id : p_node_id_set)
	{
		ERR_CONTINUE(!flow_script->has_node(curr_node_id));

		node_id_to_clip_index[curr_node_id] = curr_node_clip_index;
		curr_node_clip_index++;

		const FlowScriptEditorClipboard::Node clip_node = {
			.data = flow_script->get_node_data(curr_node_id),
			.data_rect = flow_script->get_node_rect(curr_node_id),
			.screen_center_offset = convert_point_data_to_screen(flow_script->get_node_position(curr_node_id)) - get_screen_center(),
		};
		p_target_clipboard->add_node(clip_node);
	}

	for (const FlowScriptConnectionInfo &curr_conn_info : flow_script->get_node_connection_list())
	{
		if (
				curr_conn_info.target.include_id != FlowScriptConstants::INCLUDE_ID_INVALID
				|| !node_id_to_clip_index.has(curr_conn_info.target.node_id)
				|| !node_id_to_clip_index.has(curr_conn_info.from_node_id)
		)
		{
			continue;
		}

		const FlowScriptEditorClipboard::Connection clip_connection = {
			.from_node = node_id_to_clip_index[curr_conn_info.from_node_id],
			.from_output = curr_conn_info.from_output,
			.to_node = node_id_to_clip_index[curr_conn_info.target.node_id],
		};
		p_target_clipboard->add_connection(clip_connection);
	}
}


Point2 FlowScriptEditor::get_element_graph_center(const GraphElement *p_element) const
{
	DEV_ASSERT(p_element != nullptr);
	return p_element->get_position_offset() + (p_element->get_size() * 0.5);
}


void FlowScriptEditor::make_element_unclickable(GraphElement *p_element)
{
	DEV_ASSERT(p_element != nullptr);

	p_element->set_draggable(false);
	p_element->set_selectable(false);
	p_element->set_resizable(false);

	List<Control *> control_list;
	control_list.push_back(p_element);

	for (Control *node : control_list)
	{
		node->set_mouse_filter(MOUSE_FILTER_IGNORE);
		node->set_focus_mode(FOCUS_NONE);

		for (int i = 0; i < node->get_child_count(); i++)
		{
			Control *child = Object::cast_to<Control>(node->get_child(i));
			if (child != nullptr)
			{
				control_list.push_back(child);
			}
		}
	}
}


bool FlowScriptEditor::hook_graph_is_node_hover_valid(const StringName &p_from, const int p_from_port, const StringName &p_to, const int p_to_port) const
{
	Node *from_obj = graph->get_node(String(p_from));
	Node *to_obj = graph->get_node(String(p_to));

	FlowScriptNodeEditor *from_node_editor = Object::cast_to<FlowScriptNodeEditor>(from_obj);
	FlowScriptNodeEditor *to_node_editor = Object::cast_to<FlowScriptNodeEditor>(to_obj);

	if (
			from_node_editor == nullptr
			|| to_node_editor == nullptr
			|| from_node_editor == to_node_editor
			|| from_node_editor->is_include()
	)
	{
		return false;
	}
	return true;
}


void FlowScriptEditor::hook_exec_rename_node(const FlowScriptNodeID p_node_id, const StringName &p_new_name)
{
	op_rename_node(p_node_id, p_new_name);
}


void FlowScriptEditor::hook_delete_elements_confirmed(const PackedFlowScriptIncludeIDArray &p_include_id_list, const PackedFlowScriptNodeIDArray &p_node_id_list)
{
	HashSet<FlowScriptIncludeID> include_id_set;
	HashSet<FlowScriptNodeID> node_id_set;

	for (const FlowScriptIncludeID include_id : p_include_id_list)
	{
		include_id_set.insert(include_id);
	}
	for (const FlowScriptNodeID node_id : p_node_id_list)
	{
		node_id_set.insert(node_id);
	}

	op_delete_element_sets(include_id_set, node_id_set);
}


bool FlowScriptEditor::hook_create_node(const FlowScriptNodeID p_node_id, const Dictionary &p_type_info_dict, const Point2 &p_position)
{
	const Point2i data_pos = convert_point_graph_to_data(p_position);
	const FlowScriptNodeTypeInfo type_info = FlowScriptNodeTypeInfo::create_from_dictionary(p_type_info_dict);
	const Ref<FlowScriptNode> node_data = FlowScriptNodeTypeDB::get_singleton()->instantiate_node_for_type(type_info);
	ERR_FAIL_COND_V(node_data.is_null(), false);

	return op_create_node(p_node_id, node_data, data_pos);
}


void FlowScriptEditor::hook_create_node_and_connect(const FlowScriptNodeID p_node_id, const Dictionary &p_type_info_dict, const Point2 &p_position, const FlowScriptNodeID p_from_node_id, const FlowScriptNodeConnectionListNo p_from_list, const FlowScriptNodeConnectionListSlotNo p_from_slot)
{
	if (hook_create_node(p_node_id, p_type_info_dict, p_position))
	{
		const FlowScriptNodeOutputConnection output = FlowScriptNodeOutputConnection::create_connection(p_from_list, p_from_slot);
		const FlowScriptNodeReference target = FlowScriptNodeReference::create_same_script_reference(p_node_id);

		const FlowScriptConnectionInfo conn = FlowScriptConnectionInfo::create(p_from_node_id, output, target);
		op_connect_node(conn);
	}
}


void FlowScriptEditor::hook_add_include(const FlowScriptIncludeID p_include_id, Ref<FlowScript> p_flow_script, const Point2 &p_position)
{
	const Point2i data_pos = convert_point_graph_to_data(p_position);
	op_instantiate_include(p_include_id, p_flow_script, p_position);
}


void FlowScriptEditor::queue_op_include_move(const IncludeMoveOp &p_op)
{
	op_queue_include_moves.push_back(p_op);
	queue_process_deferred_ops();
}


void FlowScriptEditor::queue_op_node_move(const NodeMoveOp &p_op)
{
	op_queue_node_moves.push_back(p_op);
	queue_process_deferred_ops();
}


void FlowScriptEditor::queue_process_deferred_ops()
{
	if (!deferred_op_processing_queued)
	{
		deferred_op_processing_queued = true;
		callable_mp(this, &FlowScriptEditor::immediate_process_deferred_ops).call_deferred();
	}
}


void FlowScriptEditor::immediate_process_deferred_ops()
{
	deferred_op_processing_queued = false;

	op_move_elements(op_queue_include_moves, op_queue_node_moves);

	op_queue_include_moves.clear();
	op_queue_node_moves.clear();
}


void FlowScriptEditor::queue_rf_include_changed(const FlowScriptIncludeID p_include_id)
{
	rf_queue_change_include_set.insert(p_include_id);
	queue_update_editor();
}


void FlowScriptEditor::queue_rf_include_removed(const FlowScriptIncludeID p_include_id)
{
	rf_queue_remove_include_set.insert(p_include_id);
	queue_update_editor();
}


void FlowScriptEditor::queue_rf_node_changed(const FlowScriptNodeID p_node_id)
{
	rf_queue_change_node_set.insert(p_node_id);
	queue_update_editor();
}


void FlowScriptEditor::queue_rf_node_removed(const FlowScriptNodeID p_node_id)
{
	rf_queue_remove_node_set.insert(p_node_id);
	queue_update_editor();
}


#if 0
void FlowScriptEditor::queue_rf_connection_create(const FlowScriptConnectionInfo &p_connection_info)
{
	rf_queue_connection_create_set.insert(p_connection_info);
	queue_update_editor();
}


void FlowScriptEditor::queue_rf_connection_break(const FlowScriptConnectionInfo &p_connection_info)
{
	rf_queue_connection_break_set.insert(p_connection_info);
	queue_update_editor();
}
#endif // 0


void FlowScriptEditor::on_flow_script_changed()
{
	if (is_visible_in_tree())
	{
		queue_update_editor();
	}
	else
	{
		total_refresh_editor();
	}
}


void FlowScriptEditor::on_flow_script_include_changed(const FlowScriptIncludeID p_include_id)
{
	queue_rf_include_changed(p_include_id);
}


void FlowScriptEditor::on_flow_script_include_rect_changed(const FlowScriptIncludeID p_include_id)
{
	// I think that include rects should only be synchronized when loading.
}


void FlowScriptEditor::on_flow_script_include_added(const FlowScriptIncludeID p_include_id)
{
	queue_rf_include_changed(p_include_id);
}


void FlowScriptEditor::on_flow_script_removing_include(const FlowScriptIncludeID p_include_id)
{
}


void FlowScriptEditor::on_flow_script_include_removed(const FlowScriptIncludeID p_include_id)
{
	queue_rf_include_removed(p_include_id);
}


void FlowScriptEditor::on_flow_script_node_changed(const FlowScriptNodeID p_node_id)
{
	queue_rf_node_changed(p_node_id);
}


void FlowScriptEditor::on_flow_script_node_renamed(const FlowScriptNodeID p_node_id)
{
	queue_rf_node_changed(p_node_id);
}


void FlowScriptEditor::on_flow_script_node_rect_changed(const FlowScriptNodeID p_node_id)
{
	queue_rf_node_changed(p_node_id);
}


void FlowScriptEditor::on_flow_script_node_connection_changed(const FlowScriptNodeID p_node_id, const FlowScriptNodeConnectionListNo p_list, const FlowScriptNodeConnectionListSlotNo p_slot, const FlowScriptIncludeID p_old_target_include_id, const FlowScriptNodeID p_old_target_node_id, const FlowScriptIncludeID p_new_target_include_id, const FlowScriptNodeID p_new_target_node_id)
{
	const FlowScriptNodeOutputConnection output = FlowScriptNodeOutputConnection::create_connection(p_list, p_slot);
	const FlowScriptNodeReference old_target = FlowScriptNodeReference::create_include_script_reference(p_old_target_include_id, p_old_target_node_id);
	const FlowScriptNodeReference new_target = FlowScriptNodeReference::create_include_script_reference(p_new_target_include_id, p_new_target_node_id);

	const FlowScriptConnectionInfo old_conn = FlowScriptConnectionInfo::create(p_node_id, output, old_target);
	const FlowScriptConnectionInfo new_conn = FlowScriptConnectionInfo::create(p_node_id, output, new_target);

	if (old_conn == new_conn)
	{
		return;
	}

	if (old_conn.target.is_valid())
	{
		queue_rf_connection_break(old_conn);
	}
	if (new_conn.target.is_valid())
	{
		queue_rf_connection_create(new_conn);
	}

	queue_rf_node_changed(p_node_id);
}


void FlowScriptEditor::on_flow_script_node_added(const FlowScriptNodeID p_node_id)
{
	queue_rf_node_changed(p_node_id);
}


void FlowScriptEditor::on_flow_script_removing_node(const FlowScriptNodeID p_node_id)
{
}


void FlowScriptEditor::on_flow_script_node_removed(const FlowScriptNodeID p_node_id)
{
	queue_rf_node_removed(p_node_id);
}


void FlowScriptEditor::on_btn_instantiate_include_pressed()
{
	const Point2 pos = get_size() * 0.5;
	request_instantiate_include_at_position(pos);
}


void FlowScriptEditor::on_btn_create_node_pressed()
{
	const Point2 pos = get_size() * 0.5;
	request_create_node_at_position(pos);
}


void FlowScriptEditor::on_graph_gui_input(const Ref<InputEvent> &p_event)
{
	const Ref<InputEventMouseButton> event_mb = p_event;
	if (event_mb.is_null() || event_mb->get_button_index() != MouseButton::LEFT)
	{
		return;
	}

	const Ref<GraphEdit::Connection> closest_connection = graph->get_closest_connection_at_point(event_mb->get_position());
	if (closest_connection.is_null())
	{
		return;
	}

	FlowScriptNodeEditor *from_node_editor = Object::cast_to<FlowScriptNodeEditor>(graph->get_node(String(closest_connection->from_node)));
	FlowScriptNodeEditor *to_node_editor = Object::cast_to<FlowScriptNodeEditor>(graph->get_node(String(closest_connection->to_node)));
	if (
			to_node_editor == nullptr
			|| from_node_editor == nullptr
			|| from_node_editor->get_edited_node_reference().include_id != FlowScriptConstants::INCLUDE_ID_INVALID
	)
	{
		return;
	}

	const int from_slot = from_node_editor->get_output_port_slot(closest_connection->from_port);
	const int to_slot = to_node_editor->get_input_port_slot(closest_connection->to_port);
	const FlowScriptNodeOutputConnection output = from_node_editor->output_graph_slot_to_connection(from_slot);
	const FlowScriptConnectionInfo conn_info = FlowScriptConnectionInfo::create(from_node_editor->get_edited_node_reference().node_id, output, to_node_editor->get_edited_node_reference());

	op_disconnect_node(conn_info);
}


void FlowScriptEditor::on_graph_begin_node_move()
{
}


void FlowScriptEditor::on_graph_connection_drag_ended()
{
}


void FlowScriptEditor::on_graph_connection_drag_started(const StringName &p_from_node, const int p_from_port, const bool p_is_output)
{
}


void FlowScriptEditor::on_graph_connection_from_empty(const StringName &p_to_node, const int p_to_port, const Point2 &p_release_position)
{
}


void FlowScriptEditor::on_graph_connection_request(const StringName &p_from_node, const int p_from_port, const StringName &p_to_node, const int p_to_port)
{
	FlowScriptNodeEditor *from_node_editor = Object::cast_to<FlowScriptNodeEditor>(graph->get_node(String(p_from_node)));
	FlowScriptNodeEditor *to_node_editor = Object::cast_to<FlowScriptNodeEditor>(graph->get_node(String(p_to_node)));

	if (from_node_editor == nullptr || to_node_editor == nullptr)
	{
		return;
	}

	const FlowScriptNodeReference from_node_ref = from_node_editor->get_edited_node_reference();
	const FlowScriptNodeReference to_node_ref = to_node_editor->get_edited_node_reference();

	if (from_node_ref.include_id != FlowScriptConstants::INCLUDE_ID_INVALID)
	{
		return;
	}

	const int from_slot = from_node_editor->get_output_port_slot(p_from_port);
	const FlowScriptNodeOutputConnection output = from_node_editor->output_graph_slot_to_connection(from_slot);
	const FlowScriptConnectionInfo conn_info = FlowScriptConnectionInfo::create(from_node_ref.node_id, output, to_node_ref);

	op_connect_node(conn_info);
}


void FlowScriptEditor::on_graph_connection_to_empty(const StringName &p_from_node, const int p_from_port, const Point2 &p_release_position)
{
	FlowScriptNodeEditor *from_node_editor = Object::cast_to<FlowScriptNodeEditor>(graph->get_node(String(p_from_node)));
	ERR_FAIL_NULL(from_node_editor);

	const int from_slot = from_node_editor->get_output_port_slot(p_from_port);
	const FlowScriptNodeOutputConnection output = from_node_editor->output_graph_slot_to_connection(from_slot);

	const FlowScriptNodeID from_node_id = from_node_editor->get_edited_node_id();
	const FlowScriptNodeID to_node_id = flow_script->get_first_available_node_slot();
	ERR_FAIL_COND(to_node_id == FlowScriptConstants::NODE_ID_INVALID);

	Callable create_node_callback = callable_mp(this, &FlowScriptEditor::hook_create_node_and_connect).bind(from_node_id, output.list, output.slot);
	common->get_dialogs()->popup_create_node(create_node_callback, flow_script, to_node_id, p_release_position);
}


void FlowScriptEditor::on_graph_copy_nodes_request()
{
	List<FlowScriptNodeID> selected_node_id_list;
	get_selected_node_id_list(&selected_node_id_list);

	if (selected_node_id_list.is_empty())
	{
		return;
	}

	HashSet<FlowScriptNodeID> node_id_set;
	node_id_set.reserve(selected_node_id_list.size());

	for (const FlowScriptNodeID node_id : selected_node_id_list)
	{
		node_id_set.insert(node_id);
	}

	push_node_set_to_clipboard(node_id_set, true, common->get_clipboard());
}


void FlowScriptEditor::on_graph_cut_nodes_request()
{
	List<FlowScriptNodeID> selected_node_id_list;
	get_selected_node_id_list(&selected_node_id_list);

	if (selected_node_id_list.is_empty())
	{
		return;
	}

	HashSet<FlowScriptNodeID> node_id_set;
	node_id_set.reserve(selected_node_id_list.size());

	for (const FlowScriptNodeID node_id : selected_node_id_list)
	{
		node_id_set.insert(node_id);
	}

	op_cut_node_set(node_id_set);
}


void FlowScriptEditor::on_graph_delete_nodes_request(const TypedArray<StringName> &p_nodes)
{
	HashSet<FlowScriptIncludeID> include_id_set;
	HashSet<FlowScriptNodeID> node_id_set;

	for (int i = 0; i < p_nodes.size(); i++)
	{
		const StringName node_name = p_nodes[i];
		Node *obj = graph->get_node(String(node_name));
		
		FlowScriptNodeEditor *node_editor = Object::cast_to<FlowScriptNodeEditor>(obj);
		FlowScriptEditorIncludeFrame *include_frame = Object::cast_to<FlowScriptEditorIncludeFrame>(obj);
		
		if (node_editor != nullptr)
		{
			ERR_CONTINUE(node_editor->get_edited_include_id() != FlowScriptConstants::INCLUDE_ID_INVALID);
			node_id_set.insert(node_editor->get_edited_node_id());
		}
		else if (include_frame != nullptr)
		{
			include_id_set.insert(include_frame->get_include_id());
		}
	}

	PackedFlowScriptIncludeIDArray include_id_list;
	PackedFlowScriptNodeIDArray node_id_list;

	int curr_idx;

	include_id_list.resize(include_id_set.size());
	node_id_list.resize(node_id_set.size());

	curr_idx = 0;
	for (const FlowScriptIncludeID include_id : include_id_set)
	{
		include_id_list.write[curr_idx++] = include_id;
	}
	curr_idx = 0;
	for (const FlowScriptNodeID node_id : node_id_set)
	{
		node_id_list.write[curr_idx++] = node_id;
	}

	if (should_element_deletion_require_confirmation(include_id_list, node_id_list))
	{
		common->get_dialogs()->confirm_delete_elements(callable_mp(this, &FlowScriptEditor::hook_delete_elements_confirmed), flow_script, include_id_list, node_id_list);
	}
	else
	{
		op_delete_element_sets(include_id_set, node_id_set);
	}
}


void FlowScriptEditor::on_graph_disconnection_request(const StringName &p_from_node, const int p_from_port, const StringName &p_to_node, const int p_to_port)
{
}


void FlowScriptEditor::on_graph_duplicate_nodes_request()
{
}


void FlowScriptEditor::on_graph_end_node_move()
{
}


void FlowScriptEditor::on_graph_frame_rect_changed(GraphFrame *p_frame, const Size2 &p_new_rect)
{
}


void FlowScriptEditor::on_graph_graph_elements_linked_to_frame_request(const Array &p_elements, const StringName &p_frame)
{
}


void FlowScriptEditor::on_graph_nodes_arranged()
{
}


void FlowScriptEditor::on_graph_node_deselected(Node *p_node)
{
}


void FlowScriptEditor::on_graph_node_selected(Node *p_node)
{
}


void FlowScriptEditor::on_graph_paste_nodes_request()
{
	op_paste_clipboard();
}


void FlowScriptEditor::on_graph_popup_request(const Point2 &p_at_position)
{
	request_create_node_at_position(p_at_position);
}


void FlowScriptEditor::on_graph_scroll_offset_changed(const Point2 &p_offset)
{
}


void FlowScriptEditor::on_node_resized(const FlowScriptNodeID p_node_id)
{
	FlowScriptNodeReference target = FlowScriptNodeReference::create_same_script_reference(p_node_id);
	FlowScriptNodeEditor *node_editor = get_node_editor_by_reference(target);
	ERR_FAIL_NULL(node_editor);

	// Ignore resizing that occurs by user input.
	// Instead, only resizing done for changes in parameters should trigger an update, ie. re-centering.
	if (node_editor->is_resizing())
	{
		return;
	}

	update_editor_node_rect(target);
}


void FlowScriptEditor::on_node_delete_request(const FlowScriptNodeID p_node_id)
{
	PackedFlowScriptNodeIDArray arr;
	arr.push_back(p_node_id);
	if (should_element_deletion_require_confirmation(PackedFlowScriptIncludeIDArray(), arr))
	{
		common->get_dialogs()->confirm_delete_elements(callable_mp(this, &FlowScriptEditor::hook_delete_elements_confirmed), flow_script, PackedFlowScriptIncludeIDArray(), arr);
	}
	else
	{
		op_delete_node(p_node_id);
	}
}


void FlowScriptEditor::on_node_dragged(const Point2 &p_from, const Point2 &p_to, const FlowScriptNodeID p_node_id)
{
	const NodeMoveOp op = {
		.node_id = p_node_id,
		.from = p_from,
		.to = p_to,
	};
	queue_op_node_move(op);
}


void FlowScriptEditor::on_node_deselected(const FlowScriptNodeID p_node_id)
{
}


void FlowScriptEditor::on_node_selected(const FlowScriptNodeID p_node_id)
{
}


void FlowScriptEditor::on_node_position_offset_changed(const FlowScriptNodeID p_node_id)
{
}


void FlowScriptEditor::on_node_raise_request(const FlowScriptNodeID p_node_id)
{
	EditorInterface::get_singleton()->edit_resource(flow_script->get_node_data(p_node_id));
}


void FlowScriptEditor::on_node_resize_end(const Size2 &p_new_size, const FlowScriptNodeID p_node_id)
{
	ERR_FAIL_COND(!flow_script->has_node(p_node_id));

	FlowScriptNodeEditor *node_editor = get_node_editor_by_reference(FlowScriptNodeReference::create_same_script_reference(p_node_id));
	ERR_FAIL_NULL(node_editor);

	const FlowScriptNodeTypeInfo type_info = node_editor->get_type_info();
	ERR_FAIL_COND(!type_info.editable_size);

	const Rect2i new_data_rect = Rect2i(
		convert_point_graph_to_data(get_element_graph_center(node_editor)),
		convert_point_graph_to_data((p_new_size / EDSCALE).round())
	);

	op_resize_node(p_node_id, new_data_rect);
}


void FlowScriptEditor::on_node_resize_request(const Size2 &p_new_size, const FlowScriptNodeID p_node_id)
{
	FlowScriptNodeEditor *node_editor = get_node_editor_by_reference(FlowScriptNodeReference::create_same_script_reference(p_node_id));
	ERR_FAIL_NULL(node_editor);

	const FlowScriptNodeTypeInfo type_info = FlowScriptNodeTypeDB::get_singleton()->get_type_of_node(flow_script->get_node_data(p_node_id));
	ERR_FAIL_COND_MSG(!type_info.editable_size, TTR("Node type does not permit resizing."));

	node_editor->set_size(p_new_size, true);
}


void FlowScriptEditor::on_node_rename_request(const FlowScriptNodeID p_node_id)
{
	ERR_FAIL_COND(!flow_script->has_node(p_node_id));

	FlowScriptNodeEditor *node_editor = get_node_editor_by_reference(FlowScriptNodeReference::create_same_script_reference(p_node_id));
	ERR_FAIL_NULL(node_editor);

	const FlowScriptNodeTypeInfo type_info = FlowScriptNodeTypeDB::get_singleton()->get_type_of_node(flow_script->get_node_data(p_node_id));
	ERR_FAIL_COND(!type_info.editable_name);

	common->get_dialogs()->popup_rename_node(callable_mp(this, &FlowScriptEditor::hook_exec_rename_node), flow_script, p_node_id);
}


void FlowScriptEditor::on_include_resized(const FlowScriptIncludeID p_include_id)
{
}


void FlowScriptEditor::on_include_delete_request(const FlowScriptIncludeID p_include_id)
{
	PackedFlowScriptIncludeIDArray arr;
	arr.push_back(p_include_id);
	common->get_dialogs()->confirm_delete_elements(callable_mp(this, &FlowScriptEditor::hook_delete_elements_confirmed), flow_script, arr, PackedFlowScriptNodeIDArray());
}


void FlowScriptEditor::on_include_dragged(const Point2 &p_from, const Point2 &p_to, const FlowScriptIncludeID p_include_id)
{
	const IncludeMoveOp op = {
		.include_id = p_include_id,
		.from = p_from,
		.to = p_to,
	};
	queue_op_include_move(op);
}


void FlowScriptEditor::on_include_deselected(const FlowScriptIncludeID p_include_id)
{
}


void FlowScriptEditor::on_include_selected(const FlowScriptIncludeID p_include_id)
{
}


void FlowScriptEditor::on_include_position_offset_changed(const FlowScriptIncludeID p_include_id)
{
}


void FlowScriptEditor::on_include_raise_request(const FlowScriptIncludeID p_include_id)
{
}


void FlowScriptEditor::on_include_resize_end(const Size2 &p_new_size, const FlowScriptIncludeID p_include_id)
{
}


void FlowScriptEditor::on_include_resize_request(const Size2 &p_new_size, const FlowScriptIncludeID p_include_id)
{
}


void FlowScriptEditor::on_include_edit_request(const FlowScriptIncludeID p_include_id)
{
	ERR_FAIL_COND(!flow_script->has_include(p_include_id));
	Ref<FlowScript> incl_scr = flow_script->get_include_flow_script(p_include_id);
	DEV_ASSERT(incl_scr.is_valid());
	common->edit_flow_script(incl_scr);
}


void FlowScriptEditor::set_state(const Dictionary &p_state)
{
	if (p_state.has("zoom"))
	{
		graph->set_zoom((float)p_state["zoom"]);
	}
	if (p_state.has("scroll"))
	{
		graph->set_scroll_offset(EDSCALE * (Point2)p_state["scroll"]);
	}
}


Dictionary FlowScriptEditor::get_state() const
{
	Dictionary d;

	d["zoom"] = graph->get_zoom();
	d["scroll"] = Point2i((graph->get_scroll_offset() / EDSCALE).round());

	return d;
}


Point2 FlowScriptEditor::get_screen_center() const
{
	return graph->get_size() * 0.5;
}


#if 0
Rect2 FlowScriptEditor::get_node_graph_rect(const FlowScriptNodeReference &p_node) const
{
	const FlowScriptNodeEditor *node_editor = get_node_editor_by_reference(p_node);
	ERR_FAIL_NULL_V(node_editor, Rect2());
	return Rect2(node_editor->get_position_offset(), node_editor->get_size());

#if 0
	Rect2i rect_i;

	if (p_node.include_id == FlowScriptConstants::INCLUDE_ID_INVALID)
	{
		rect_i = flow_script->get_node_rect(p_node.node_id);
	}
	else if (flow_script->has_include(p_node.include_id))
	{
		rect_i = flow_script->get_include_flow_script(p_node.include_id)->get_node_rect(p_node.node_id);
	}
	else
	{
		ERR_FAIL_V(Rect2i());
	}

	// PASS IDK
	return rect_i; // grrr this is wrong but idk why i wrote this
#endif // 0
}
#endif // 0


Rect2i FlowScriptEditor::get_node_data_rect(const FlowScriptNodeReference &p_node) const
{
	ERR_FAIL_COND_V(!flow_script->has_target(p_node), Rect2i());

	if (p_node.include_id == FlowScriptConstants::INCLUDE_ID_INVALID)
	{
		return flow_script->get_node_rect(p_node.node_id);
	}
	else
	{
		Rect2i ret = flow_script->get_include_flow_script(p_node.include_id)->get_node_rect(p_node.node_id);
		ret.position += flow_script->get_include_position(p_node.include_id);
		return ret;
	}
}


Point2 FlowScriptEditor::convert_point_data_to_graph(const Point2i &p_data_point) const
{
	Point2 ret = p_data_point;
	ret *= EDSCALE;
	return ret;
}


Point2 FlowScriptEditor::convert_point_data_to_screen(const Point2i &p_data_point) const
{
	return convert_point_graph_to_screen(convert_point_data_to_graph(p_data_point));
}


Point2i FlowScriptEditor::convert_point_graph_to_data(const Point2 &p_graph_point) const
{
	Point2 scaled_point = p_graph_point / EDSCALE;
	scaled_point = scaled_point.round();
	Point2i ret = scaled_point;
	return ret;
}


Point2 FlowScriptEditor::convert_point_graph_to_screen(const Point2 &p_graph_point) const
{
	// TODO: Test this. I just wrote the inverse of convert_point_screen_to_graph hoping that it'd work, but honestly, I'm not smart enough to know.
	Point2 ret;
	ret -= graph->get_scroll_offset();
	ret *= graph->get_zoom();
	return ret;
}


Point2 FlowScriptEditor::convert_point_screen_to_graph(const Point2 &p_screen_point) const
{
	Point2 ret = p_screen_point;
	ret += graph->get_scroll_offset();
	ret /= graph->get_zoom();
	return ret;
}


Point2i FlowScriptEditor::convert_point_screen_to_data(const Point2 &p_screen_point) const
{
	return convert_point_graph_to_data(convert_point_screen_to_graph(p_screen_point));
}


bool FlowScriptEditor::op_instantiate_include(const FlowScriptIncludeID p_include_id, const Ref<FlowScript> &p_include_flow_script, const Point2i &p_position)
{
	ERR_FAIL_COND_V(flow_script->has_include(p_include_id), false);
	ERR_FAIL_COND_V(!flow_script->can_include_flow_script(p_include_flow_script), false);
	
	EditorUndoRedoManager *udrd = get_undo_redo();

	create_undo_redo_action(TTR("Instantiate Include"), UndoRedo::MERGE_DISABLE, false);
	udrd->add_do_method(flow_script.ptr(), SNAME("set_include_flow_script"), p_include_id, p_include_flow_script);
	udrd->add_do_method(flow_script.ptr(), SNAME("set_include_position"), p_include_id, p_position);
	udrd->add_undo_method(flow_script.ptr(), SNAME("remove_include"), p_include_id);
	commit_undo_redo_action();

	return true;
}


bool FlowScriptEditor::op_create_node(const FlowScriptNodeID p_node_id, const Ref<FlowScriptNode> &p_data, const Point2i &p_position)
{
	ERR_FAIL_COND_V(flow_script->has_node(p_node_id), false);
	ERR_FAIL_COND_V(p_data.is_null(), false);
	ERR_FAIL_COND_V(!flow_script->can_add_node_data(p_data), false);

	EditorUndoRedoManager *udrd = get_undo_redo();

	create_undo_redo_action(TTR("Create Node"), UndoRedo::MERGE_DISABLE, false);
	udrd->add_do_method(flow_script.ptr(), SNAME("set_node_data"), p_node_id, p_data);
	udrd->add_do_method(flow_script.ptr(), SNAME("set_node_position"), p_node_id, p_position);
	udrd->add_undo_method(flow_script.ptr(), SNAME("remove_node"), p_node_id);
	commit_undo_redo_action();

	return true;
}


bool FlowScriptEditor::op_delete_include(const FlowScriptIncludeID p_include_id)
{
	HashSet<FlowScriptIncludeID> include_id_set;
	include_id_set.insert(p_include_id);
	return op_delete_element_sets(include_id_set, HashSet<FlowScriptNodeID>());
}


bool FlowScriptEditor::op_delete_node(const FlowScriptNodeID p_node_id)
{
	HashSet<FlowScriptNodeID> node_id_set;
	node_id_set.insert(p_node_id);
	return op_delete_element_sets(HashSet<FlowScriptIncludeID>(), node_id_set);
}


void FlowScriptEditor::op_step_remove_element_sets_from_flow_script(const HashSet<FlowScriptIncludeID> &p_include_id_set, const HashSet<FlowScriptNodeID> &p_node_id_set)
{
	EditorUndoRedoManager *udrd = get_undo_redo();

	for (const FlowScriptIncludeID include_id : p_include_id_set)
	{
		ERR_CONTINUE(!flow_script->has_include(include_id));

		const Ref<FlowScript> include_flow_script = flow_script->get_include_flow_script(include_id);
		udrd->add_do_method(flow_script.ptr(), SNAME("remove_include"), include_id);
		udrd->add_undo_method(flow_script.ptr(), SNAME("set_include_flow_script"), include_id, include_flow_script);
	}

	for (const FlowScriptNodeID node_id : p_node_id_set)
	{
		ERR_CONTINUE(!flow_script->has_node(node_id));

		const Ref<FlowScriptNode> node_data = flow_script->get_node_data(node_id);
		udrd->add_do_method(flow_script.ptr(), SNAME("remove_node"), node_id);
		udrd->add_undo_method(flow_script.ptr(), SNAME("set_node_data"), node_id, node_data);
	}

	for (const FlowScriptIncludeID include_id : p_include_id_set)
	{
		ERR_CONTINUE(!flow_script->has_include(include_id));

		const Point2i pos = flow_script->get_include_position(include_id);
		udrd->add_undo_method(flow_script.ptr(), SNAME("set_include_position"), include_id, pos);
	}

	for (const FlowScriptNodeID node_id : p_node_id_set)
	{
		ERR_CONTINUE(!flow_script->has_node(node_id));

		const StringName name = flow_script->get_node_name(node_id);
		const Rect2i rect = flow_script->get_node_rect(node_id);

		if (name != StringName())
		{
			udrd->add_undo_method(flow_script.ptr(), SNAME("set_node_name"), node_id, name);
		}
		udrd->add_undo_method(flow_script.ptr(), SNAME("set_node_rect"), node_id, rect);
	}

	for (const FlowScriptConnectionInfo &conn_info : flow_script->get_node_connection_list())
	{
		if (
				p_include_id_set.has(conn_info.target.include_id)
				|| (
						conn_info.target.include_id == FlowScriptConstants::INCLUDE_ID_INVALID
						&& p_node_id_set.has(conn_info.target.node_id)
				)
		)
		{
			udrd->add_undo_method(flow_script.ptr(), SNAME("add_node_connection"), conn_info.to_dictionary());
		}
	}
}


bool FlowScriptEditor::op_delete_element_sets(const HashSet<FlowScriptIncludeID> &p_include_id_set, const HashSet<FlowScriptNodeID> &p_node_id_set)
{
	String action_name;

	if (!p_include_id_set.is_empty() && p_node_id_set.is_empty())
	{
		action_name = TTRN("Delete Include", "Delete Includes", p_include_id_set.size());
	}
	else if (p_include_id_set.is_empty() && !p_node_id_set.is_empty())
	{
		action_name = TTRN("Delete Node", "Delete Nodes", p_node_id_set.size());
	}
	else if (!p_include_id_set.is_empty() && !p_node_id_set.is_empty())
	{
		action_name = vformat(TTR("Delete %s and %s"), TTRN("Include", "Includes", p_include_id_set.size()), TTRN("Node", "Nodes", p_node_id_set.size()));
	}
	else
	{
		return false;
	}

	create_undo_redo_action(action_name, UndoRedo::MERGE_DISABLE, false);
	op_step_remove_element_sets_from_flow_script(p_include_id_set, p_node_id_set);
	commit_undo_redo_action();

	return true;
}


bool FlowScriptEditor::op_connect_node(const FlowScriptConnectionInfo &p_connection_info)
{
	ERR_FAIL_COND_V(flow_script->has_node_connection(p_connection_info), false);

	EditorUndoRedoManager *udrd = get_undo_redo();

	create_undo_redo_action(TTR("Connect Nodes"), UndoRedo::MERGE_DISABLE, false);
	udrd->add_do_method(flow_script.ptr(), SNAME("add_node_connection"), p_connection_info.to_dictionary());
	udrd->add_undo_method(flow_script.ptr(), SNAME("remove_node_connection"), p_connection_info.from_node_id, p_connection_info.from_output.list, p_connection_info.from_output.slot);
	commit_undo_redo_action();

	return true;
}


bool FlowScriptEditor::op_disconnect_node(const FlowScriptConnectionInfo &p_connection_info)
{
	ERR_FAIL_COND_V(!flow_script->has_node_connection(p_connection_info), false);

	EditorUndoRedoManager *udrd = get_undo_redo();

	create_undo_redo_action(TTR("Disconnect Nodes"), UndoRedo::MERGE_DISABLE, false);
	udrd->add_do_method(flow_script.ptr(), SNAME("remove_node_connection"), p_connection_info.from_node_id, p_connection_info.from_output.list, p_connection_info.from_output.slot);
	udrd->add_undo_method(flow_script.ptr(), SNAME("add_node_connection"), p_connection_info.to_dictionary());
	commit_undo_redo_action();

	return true;
}


bool FlowScriptEditor::op_rename_node(const FlowScriptNodeID p_node_id, const StringName &p_new_name)
{
	ERR_FAIL_COND_V(!flow_script->has_node(p_node_id), false);

	const StringName curr_name = flow_script->get_node_name(p_node_id);
	if (p_new_name == curr_name)
	{
		return true;
	}

	ERR_FAIL_COND_V(!flow_script->can_add_node_name(p_new_name), false);

	const String action_name = p_new_name == StringName() ? TTR("Clear Node Name") : TTR("Rename Node");
	EditorUndoRedoManager *udrd = get_undo_redo();
	create_undo_redo_action(action_name, UndoRedo::MERGE_DISABLE, false);
	udrd->add_do_method(flow_script.ptr(), SNAME("set_node_name"), p_node_id, p_new_name);
	udrd->add_undo_method(flow_script.ptr(), SNAME("set_node_name"), p_node_id, curr_name);
	commit_undo_redo_action();
	return true;
}


bool FlowScriptEditor::op_move_elements(const List<IncludeMoveOp> &p_include_moves, const List<NodeMoveOp> &p_node_moves)
{
	String action_name;

	if (!p_include_moves.is_empty() && p_node_moves.is_empty())
	{
		action_name = TTRN("Move Include", "Move Includes", p_include_moves.size());
	}
	else if (p_include_moves.is_empty() && !p_node_moves.is_empty())
	{
		action_name = TTRN("Move Node", "Move Nodes", p_node_moves.size());
	}
	else if (!p_include_moves.is_empty() && !p_node_moves.is_empty())
	{
		action_name = vformat(TTR("Move %s and %s"), TTRN("Include", "Includes", p_include_moves.size()), TTRN("Node", "Nodes", p_node_moves.size()));
	}
	else
	{
		return false;
	}

	create_undo_redo_action(action_name, UndoRedo::MERGE_DISABLE, false);
	EditorUndoRedoManager *udrd = get_undo_redo();;

	for (const IncludeMoveOp &move : p_include_moves)
	{
		ERR_CONTINUE(!flow_script->has_include(move.include_id));

		const Point2i curr_data_pos = flow_script->get_include_position(move.include_id);
		const Vector2 graph_pos_diff = move.to - move.from; 
		const Point2i new_data_pos = curr_data_pos + Vector2i((graph_pos_diff / EDSCALE).round());

		udrd->add_do_method(flow_script.ptr(), SNAME("set_include_position"), move.include_id, new_data_pos);
		udrd->add_undo_method(flow_script.ptr(), SNAME("set_include_position"), move.include_id, curr_data_pos);
	}

	for (const NodeMoveOp &move : p_node_moves)
	{
		ERR_CONTINUE(!flow_script->has_node(move.node_id));

		const Point2i curr_data_pos = flow_script->get_node_position(move.node_id);
		const Vector2 graph_pos_diff = move.to - move.from;
		const Point2i new_data_pos = curr_data_pos + Vector2i((graph_pos_diff / EDSCALE).round());

		udrd->add_do_method(flow_script.ptr(), SNAME("set_node_position"), move.node_id, new_data_pos);
		udrd->add_undo_method(flow_script.ptr(), SNAME("set_node_position"), move.node_id, curr_data_pos);
	}

	commit_undo_redo_action();

	return true;
}


bool FlowScriptEditor::op_cut_node_set(const HashSet<FlowScriptNodeID> &p_node_id_set)
{
	if (p_node_id_set.is_empty())
	{
		return false;
	}

	push_node_set_to_clipboard(p_node_id_set, true, common->get_clipboard());

	create_undo_redo_action(TTRN("Cut Node", "Cut Nodes", p_node_id_set.size()), UndoRedo::MERGE_DISABLE, false);
	op_step_remove_element_sets_from_flow_script(HashSet<FlowScriptIncludeID>(), p_node_id_set);
	commit_undo_redo_action();

	return true;
}


void FlowScriptEditor::op_step_paste_from_clipboard(const FlowScriptEditorClipboard *p_clipboard, const PastePlacementMode p_placement_node, const Vector2 &p_placement_offset)
{
	EditorUndoRedoManager *udrd = get_undo_redo();

	Vector2 placement_offset = p_placement_offset;
	placement_offset /= graph->get_zoom();

	PackedFlowScriptNodeIDArray new_node_id_list;
	HashMap<int, FlowScriptNodeID> map_clipboard_index_to_node_id;
	map_clipboard_index_to_node_id.reserve(p_clipboard->get_node_count());

	FlowScriptNodeID search_free_slot_from = FlowScriptConstants::NODE_ID_MIN;
	for (int clip_node_index = 0; clip_node_index < p_clipboard->get_node_count(); clip_node_index++)
	{
		const FlowScriptNodeID new_node_id = flow_script->get_first_available_node_slot(search_free_slot_from);
		ERR_BREAK(new_node_id == FlowScriptConstants::NODE_ID_INVALID);
		search_free_slot_from = new_node_id + 1;
		new_node_id_list.push_back(new_node_id);
		map_clipboard_index_to_node_id.insert(clip_node_index, new_node_id);

		const FlowScriptEditorClipboard::Node clip_node = p_clipboard->get_node(clip_node_index);
		const Ref<FlowScriptNode> node_data = clip_node.data;
		Point2 node_pos_f;

		switch (p_placement_node)
		{
			case PASTE_SCREEN_OFFSET: {
				node_pos_f = graph->get_size() * 0.5;
				node_pos_f += clip_node.screen_center_offset;
				node_pos_f += placement_offset;
			} break;
			case PASTE_RECT:
			default: {
				node_pos_f = convert_point_data_to_graph(clip_node.data_rect.position);
				node_pos_f += placement_offset;
			} break;
		}

		const Rect2i dest_rect = Rect2i(convert_point_graph_to_data(node_pos_f), clip_node.data_rect.size);

		udrd->add_do_method(flow_script.ptr(), SNAME("set_node_data"), new_node_id, node_data);
		udrd->add_do_method(flow_script.ptr(), SNAME("set_node_rect"), new_node_id, dest_rect);
	}

	for (int clip_connection_index = 0; clip_connection_index < p_clipboard->get_connection_count(); clip_connection_index++)
	{
		const FlowScriptEditorClipboard::Connection clip_connection = p_clipboard->get_connection(clip_connection_index);

		if (
				map_clipboard_index_to_node_id.has(clip_connection.from_node)
				&& map_clipboard_index_to_node_id.has(clip_connection.to_node)
		)
		{
			const FlowScriptConnectionInfo connection_info = FlowScriptConnectionInfo::create(
					map_clipboard_index_to_node_id[clip_connection.from_node],
					clip_connection.from_output,
					FlowScriptNodeReference::create_same_script_reference(map_clipboard_index_to_node_id[clip_connection.to_node])
			);
			udrd->add_do_method(flow_script.ptr(), SNAME("add_node_connection"), connection_info.to_dictionary());
		}
	}

	udrd->add_undo_method(flow_script.ptr(), SNAME("remove_node_list"), new_node_id_list);
}


bool FlowScriptEditor::op_paste_clipboard()
{
	if (common->get_clipboard()->is_empty())
	{
		return false;
	}

	create_undo_redo_action(TTRN("Paste Node", "Paste Nodes", common->get_clipboard()->get_node_count()), UndoRedo::MERGE_DISABLE, false);
	op_step_paste_from_clipboard(common->get_clipboard(), PASTE_SCREEN_OFFSET, element_paste_offset);
	commit_undo_redo_action();

	return true;
}


bool FlowScriptEditor::op_duplicate_node_set(const HashSet<FlowScriptNodeID> &p_node_id_set)
{
	FlowScriptEditorClipboard dupe_clipboard = FlowScriptEditorClipboard();
	push_node_set_to_clipboard(p_node_id_set, false, &dupe_clipboard);

	if (dupe_clipboard.is_empty())
	{
		return false;
	}

	create_undo_redo_action(TTRN("Duplicate Node", "Duplicate Nodes", p_node_id_set.size()), UndoRedo::MERGE_DISABLE, false);
	op_step_paste_from_clipboard(&dupe_clipboard, PASTE_RECT, element_duplicate_offset);
	commit_undo_redo_action();

	return true;
}


bool FlowScriptEditor::op_resize_node(const FlowScriptNodeID p_node_id, const Rect2i &p_new_rect)
{
	ERR_FAIL_COND_V(!flow_script->has_node(p_node_id), false);

	create_undo_redo_action(TTR("Resize Node"), UndoRedo::MERGE_DISABLE, false);
	EditorUndoRedoManager *udrd = get_undo_redo();
	udrd->add_do_method(flow_script.ptr(), SNAME("set_node_rect"), p_node_id, p_new_rect);
	udrd->add_undo_method(flow_script.ptr(), SNAME("set_node_rect"), p_node_id, flow_script->get_node_rect(p_node_id));
	commit_undo_redo_action();

	return true;
}


Size2 FlowScriptEditor::get_minimum_size() const
{
	return graph->get_combined_minimum_size();
}


void FlowScriptEditor::sync_editor_settings()
{
	element_paste_offset = EDSCALE * Vector2(EDITOR_GET("editors/flow_script_editor/clipboard/element_paste_offset"));
	element_duplicate_offset = EDSCALE * Vector2(EDITOR_GET("editors/flow_script_editor/clipboard/element_duplicate_offset"));
}


void FlowScriptEditor::_notification(int p_what)
{
	if (p_what == NOTIFICATION_THEME_CHANGED)
	{
		sync_editor_settings();

		// "Instance.svg" has a chain/link icon.
		// Its broken counterpart is "Unlinked.svg".
		const Ref<Texture2D> icon_instance = get_editor_theme_icon(SNAME("Instance"));
		const Ref<Texture2D> icon_add = get_editor_theme_icon(SNAME("Add"));

		btn_instantiate_include->set_button_icon(icon_instance);
		btn_create_node->set_button_icon(icon_add);
	}
	else if (p_what == EditorSettings::NOTIFICATION_EDITOR_SETTINGS_CHANGED)
	{
		if (EditorSettings::get_singleton()->check_changed_settings_in_group("editors/flow_script_editor"))
		{
			sync_editor_settings();
		}
	}
	else if (p_what == NOTIFICATION_VISIBILITY_CHANGED)
	{
		if (is_visible_in_tree())
		{
			if (editor_total_refresh_queued)
			{
				total_refresh_editor();
			}
			else if (editor_update_queued)
			{
				immediate_update_editor();
			}
			editor_total_refresh_queued = false;
			editor_update_queued = false;
		}
	}
#if 0 // The undo/redo system should re-open the editor if it was closed, and all operations are anyhow done on the FlowScript directly.
	else if (p_what == NOTIFICATION_EXIT_TREE)
	{
		EditorUndoRedoManager *udrd = get_undo_redo();
		if (udrd != nullptr)
		{
			const int history_id = udrd->get_history_id_for_object(get_undo_redo_context());
			udrd->clear_history(history_id);
		}
	}
#endif // 0
}


void FlowScriptEditor::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_graph"), &FlowScriptEditor::get_graph);
	ClassDB::bind_method(D_METHOD("get_flow_script"), &FlowScriptEditor::get_flow_script);

	EDITOR_DEF("editors/flow_script_editor/clipboard/element_paste_offset", Vector2i(40, 40));
	EDITOR_DEF("editors/flow_script_editor/clipborad/element_duplicate_offset", Vector2i(40, 40));
}


FlowScriptEditor::FlowScriptEditor()
{
	graph = memnew(FlowScriptEditorGraph);
	graph->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	graph->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	graph->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	graph->init_dependencies(callable_mp(this, &FlowScriptEditor::hook_graph_is_node_hover_valid));
	graph->connect(SceneStringName(gui_input), callable_mp(this, &FlowScriptEditor::on_graph_gui_input));
	graph->connect(SNAME("begin_node_move"), callable_mp(this, &FlowScriptEditor::on_graph_begin_node_move));
	graph->connect(SNAME("connection_drag_ended"), callable_mp(this, &FlowScriptEditor::on_graph_connection_drag_ended));
	graph->connect(SNAME("connection_drag_started"), callable_mp(this, &FlowScriptEditor::on_graph_connection_drag_started));
	graph->connect(SNAME("connection_from_empty"), callable_mp(this, &FlowScriptEditor::on_graph_connection_from_empty));
	graph->connect(SNAME("connection_request"), callable_mp(this, &FlowScriptEditor::on_graph_connection_request));
	graph->connect(SNAME("connection_to_empty"), callable_mp(this, &FlowScriptEditor::on_graph_connection_to_empty));
	graph->connect(SNAME("copy_nodes_request"), callable_mp(this, &FlowScriptEditor::on_graph_copy_nodes_request));
	graph->connect(SNAME("cut_nodes_request"), callable_mp(this, &FlowScriptEditor::on_graph_cut_nodes_request));
	graph->connect(SNAME("delete_nodes_request"), callable_mp(this, &FlowScriptEditor::on_graph_delete_nodes_request));
	graph->connect(SNAME("disconnection_request"), callable_mp(this, &FlowScriptEditor::on_graph_disconnection_request));
	graph->connect(SNAME("duplicate_nodes_request"), callable_mp(this, &FlowScriptEditor::on_graph_duplicate_nodes_request));
	graph->connect(SNAME("end_node_move"), callable_mp(this, &FlowScriptEditor::on_graph_end_node_move));
	graph->connect(SNAME("frame_rect_changed"), callable_mp(this, &FlowScriptEditor::on_graph_frame_rect_changed));
	graph->connect(SNAME("graph_elements_linked_to_frame_request"), callable_mp(this, &FlowScriptEditor::on_graph_graph_elements_linked_to_frame_request));
	graph->connect(SNAME("nodes_arranged"), callable_mp(this, &FlowScriptEditor::on_graph_nodes_arranged));
	graph->connect(SNAME("node_deselected"), callable_mp(this, &FlowScriptEditor::on_graph_node_deselected));
	graph->connect(SNAME("node_selected"), callable_mp(this, &FlowScriptEditor::on_graph_node_selected));
	graph->connect(SNAME("paste_nodes_request"), callable_mp(this, &FlowScriptEditor::on_graph_paste_nodes_request));
	graph->connect(SNAME("popup_request"), callable_mp(this, &FlowScriptEditor::on_graph_popup_request));
	graph->connect(SNAME("scroll_offset_changed"), callable_mp(this, &FlowScriptEditor::on_graph_scroll_offset_changed));
	add_child(graph);

	HBoxContainer *graph_submenu = memnew(HBoxContainer);
	graph->get_menu_hbox()->add_child(graph_submenu);
	graph->get_menu_hbox()->move_child(graph_submenu, 0);

	btn_create_node = memnew(Button);
	btn_create_node->set_tooltip_text(TTR("Create Node..."));
	btn_create_node->set_flat(true);
	btn_create_node->connect(SceneStringName(pressed), callable_mp(this, &FlowScriptEditor::on_btn_create_node_pressed));
	graph_submenu->add_child(btn_create_node);

	btn_instantiate_include = memnew(Button);
	btn_instantiate_include->set_tooltip_text(TTR("Instantiate Include..."));
	btn_instantiate_include->set_flat(true);
	btn_instantiate_include->connect(SceneStringName(pressed), callable_mp(this, &FlowScriptEditor::on_btn_instantiate_include_pressed));
	graph_submenu->add_child(btn_instantiate_include);

	graph_submenu->add_child(memnew(VSeparator));
}


FlowScriptEditor::~FlowScriptEditor()
{
}


void FlowScriptEditorPlugin::submit_file_option(const int p_option)
{
	switch (p_option)
	{
		case FILE_OPTION_SAVE: {
			ERR_FAIL_COND(current_selected_flow_script.is_null());
			save_flow_script(current_selected_flow_script);
		} break;
		case FILE_OPTION_SAVE_ALL: {
			save_all_flow_scripts();
		} break;
		case FILE_OPTION_CLOSE: {
			ERR_FAIL_COND(current_selected_flow_script.is_null());
			close_flow_script(current_selected_flow_script, false);
		} break;
		case FILE_OPTION_CLOSE_ALL: {
			close_all_flow_scripts(false);
		} break;
		default: {} break;
	}
}


void FlowScriptEditorPlugin::update_file_menu()
{
	PopupMenu *file_menu_popup = file_menu_button->get_popup();

	for (int item = 0; item < FILE_OPTION_MAX; item++)
	{
		const int index = file_menu_popup->get_item_index(item);
		file_menu_popup->set_item_disabled(index, true);
	}

	if (current_selected_flow_script.is_valid())
	{
		if (!current_selected_flow_script->is_built_in())
		{
			file_menu_popup->set_item_disabled(file_menu_popup->get_item_index(FILE_OPTION_SAVE), false);
		}
		file_menu_popup->set_item_disabled(file_menu_popup->get_item_index(FILE_OPTION_CLOSE), false);
	}

	if (!map_edited_roots.is_empty())
	{
		file_menu_popup->set_item_disabled(file_menu_popup->get_item_index(FILE_OPTION_SAVE_ALL), false);
		file_menu_popup->set_item_disabled(file_menu_popup->get_item_index(FILE_OPTION_CLOSE_ALL), false);
	}
}


bool FlowScriptEditorPlugin::add_flow_script_to_edited_list(const Ref<FlowScript> &p_flow_script)
{
	ERR_FAIL_COND_V(p_flow_script.is_null(), false);

	if (map_edited_roots.has(p_flow_script))
	{
		return true;
	}

	FlowScriptEditor *editor = memnew(FlowScriptEditor);
	editor->init_dependencies(common, p_flow_script);
	editor->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	editor->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	editor_tabs->add_child(editor);

	EditedRoot edited_root = EditedRoot();
	edited_root.editor = editor;
	edited_root.timestamp_last_change = 0.0;
	edited_root.timestamp_last_save = 0.0;
	map_edited_roots.insert(p_flow_script, edited_root);

	queue_refresh_flow_script_item_list();

	return true;
}


void FlowScriptEditorPlugin::select_flow_script(const Ref<FlowScript> &p_flow_script)
{
	if (p_flow_script == current_selected_flow_script)
	{
		return;
	}

	if (current_selected_flow_script.is_valid())
	{
		current_selected_flow_script->disconnect_changed(callable_mp(this, &FlowScriptEditorPlugin::on_current_selected_flow_script_changed));
	}

	current_selected_flow_script = Ref<FlowScript>();

	if (p_flow_script.is_valid())
	{
		if (!map_edited_roots.has(p_flow_script))
		{
			ERR_FAIL_COND(!add_flow_script_to_edited_list(p_flow_script));
		}
		p_flow_script->connect_changed(callable_mp(this, &FlowScriptEditorPlugin::on_current_selected_flow_script_changed));
	}

	current_selected_flow_script = p_flow_script;
	queue_update_visible_tab();
	queue_refresh_flow_script_item_list();
}


void FlowScriptEditorPlugin::trigger_flow_script_edit_sync_timer()
{
	if (flow_script_edit_sync_timer->is_stopped())
	{
		flow_script_edit_sync_timer->start();
	}
}


void FlowScriptEditorPlugin::handle_flow_script_saved(const Ref<FlowScript> &p_flow_script)
{
	ERR_FAIL_COND(!map_edited_roots.has(p_flow_script));

	EditedRoot &edited_root = map_edited_roots[p_flow_script];
	edited_root.timestamp_last_save = OS::get_singleton()->get_unix_time();
	queue_refresh_flow_script_item_list();
}


bool FlowScriptEditorPlugin::save_flow_script(Ref<FlowScript> p_flow_script)
{
	ERR_FAIL_COND_V(p_flow_script.is_null(), false);
	ERR_FAIL_COND_V(!map_edited_roots.has(p_flow_script), false);

	if (p_flow_script->is_built_in())
	{
		return true;
	}

	EditedRoot &edited_root = map_edited_roots[p_flow_script];

	const String path = p_flow_script->get_path();
	const Error save_err = ResourceSaver::save(p_flow_script, path);
	ERR_FAIL_COND_V(save_err != OK, false);

	handle_flow_script_saved(p_flow_script);

	return true;
}


void FlowScriptEditorPlugin::save_all_flow_scripts()
{
	for (KeyValue<Ref<FlowScript>, EditedRoot> &E : map_edited_roots)
	{
		save_flow_script(E.key);
	}
}


bool FlowScriptEditorPlugin::close_flow_script(const Ref<FlowScript> &p_flow_script, const bool p_ignore_unsaved)
{
	ERR_FAIL_COND_V(p_flow_script.is_null(), false);
	ERR_FAIL_COND_V(!map_edited_roots.has(p_flow_script), false);

	EditedRoot &edited_root = map_edited_roots[p_flow_script];

	if (!p_ignore_unsaved && edited_root.timestamp_last_save < edited_root.timestamp_last_change)
	{
		common->get_dialogs()->confirm_close_unsaved_flow_script(
				callable_mp(this, &FlowScriptEditorPlugin::hook_pre_close_flow_script_save).bind(p_flow_script),
				callable_mp(this, &FlowScriptEditorPlugin::hook_pre_close_flow_script_discard).bind(p_flow_script),
				p_flow_script,
				edited_root.timestamp_last_save,
				edited_root.timestamp_last_change
		);
		return false;
	}

	if (p_flow_script == current_selected_flow_script)
	{
		select_flow_script(Ref<FlowScript>());
	}

	edited_root.editor->queue_free();
	map_edited_roots.erase(p_flow_script);
	queue_refresh_flow_script_item_list();

	return true;
}


void FlowScriptEditorPlugin::close_all_flow_scripts(const bool p_ignore_unsaved)
{
	LocalVector<const Ref<FlowScript>> del_queue;
	for (const KeyValue<Ref<FlowScript>, EditedRoot> &E : map_edited_roots)
	{
		del_queue.push_back(E.key);
	}
	for (const Ref<FlowScript> &scr : del_queue)
	{
		close_flow_script(scr, p_ignore_unsaved);
	}
}


void FlowScriptEditorPlugin::clear_edit_history()
{
	const int history_id = EditorUndoRedoManager::GLOBAL_HISTORY;
	EditorUndoRedoManager::get_singleton()->clear_history(history_id);
}


void FlowScriptEditorPlugin::queue_update_visible_tab()
{
	if (!visible_tab_dirty)
	{
		visible_tab_dirty = true;
		callable_mp(this, &FlowScriptEditorPlugin::immediate_update_visible_tab).call_deferred();
	}
}


void FlowScriptEditorPlugin::immediate_update_visible_tab()
{
	visible_tab_dirty = false;

	if (current_selected_flow_script.is_valid())
	{
		ERR_FAIL_COND(!map_edited_roots.has(current_selected_flow_script));
		const EditedRoot &edited = map_edited_roots[current_selected_flow_script];
		editor_tabs->set_current_tab(editor_tabs->get_tab_idx_from_control(edited.editor));
	}
	else
	{
		editor_tabs->set_current_tab(-1);
	}
}


void FlowScriptEditorPlugin::update_theme()
{
	const Ref<Font> font_main = main_control->get_theme_font("main_msdf", EditorStringName(EditorFonts));
	const Ref<Font> font_main_bold = main_control->get_theme_font("main_bold_msdf", EditorStringName(EditorFonts));

	msdf_theme->set_default_font(font_main);
	msdf_theme->set_font("font", "Label", font_main);
	msdf_theme->set_font("font", "GraphNodeTitleLabel", font_main_bold);
	msdf_theme->set_font("normal_font", "RichTextLabel", font_main);
	msdf_theme->set_font("bold_font", "RichTextLabel", font_main_bold);
}


void FlowScriptEditorPlugin::immediate_refresh_flow_script_item_list()
{
	flow_script_item_list_dirty = false;

	flow_script_item_list->clear();

	const Vector<Ref<FlowScript>> sorted_script_list = get_alpha_sorted_edited_flow_script_list();
	const Ref<Texture2D> ico = get_plugin_icon();

	int desired_select_item = -1;

	for (const Ref<FlowScript> &E : sorted_script_list)
	{
		ERR_CONTINUE(!map_edited_roots.has(E));
		const EditedRoot &edited_root = map_edited_roots[E];

		String item_title = E->get_path().get_file();
		if (edited_root.timestamp_last_save < edited_root.timestamp_last_change && !E->is_built_in())
		{
			item_title += "(*)";
		}
		const String item_tooltip_text = E->get_path();

		const int item_id = flow_script_item_list->add_item(item_title, ico, true);
		flow_script_item_list->set_item_tooltip_enabled(item_id, true);
		flow_script_item_list->set_item_tooltip(item_id, item_tooltip_text);
		flow_script_item_list->set_item_metadata(item_id, E);

		if (E == current_selected_flow_script)
		{
			desired_select_item = item_id;
		}
	}

	if (desired_select_item != -1)
	{
		flow_script_item_list->select(desired_select_item, true);
	}
}


void FlowScriptEditorPlugin::queue_refresh_flow_script_item_list()
{
	if (!flow_script_item_list_dirty)
	{
		flow_script_item_list_dirty = true;
		callable_mp(this, &FlowScriptEditorPlugin::immediate_refresh_flow_script_item_list).call_deferred();
	}
}


Vector<Ref<FlowScript>> FlowScriptEditorPlugin::get_alpha_sorted_edited_flow_script_list() const
{
	struct FilenameComparator final
	{
		bool operator()(const Ref<FlowScript> &a, const Ref<FlowScript> &b) const
		{
			const String name_a = a->get_path().get_file();
			const String name_b = b->get_path().get_file();
			return name_a < name_b;
		}
	};

	Vector<Ref<FlowScript>> ret;

	for (const KeyValue<Ref<FlowScript>, EditedRoot> &E : map_edited_roots)
	{
		ret.push_back(E.key);
	}

	ret.sort_custom<FilenameComparator>();

	return ret;
}


void FlowScriptEditorPlugin::hook_pre_close_flow_script_save(Ref<FlowScript> p_flow_script)
{
	ERR_FAIL_COND(!save_flow_script(p_flow_script));
	close_flow_script(p_flow_script, false);
}


void FlowScriptEditorPlugin::hook_pre_close_flow_script_discard(Ref<FlowScript> p_flow_script)
{
	ERR_FAIL_COND(!close_flow_script(p_flow_script, true));
}


void FlowScriptEditorPlugin::on_this_resource_saved(Ref<Resource> p_resource)
{
	const Ref<FlowScript> saved_scr = p_resource;
	if (saved_scr.is_null())
	{
		return;
	}
	for (const KeyValue<Ref<FlowScript>, EditedRoot> &E : map_edited_roots)
	{
		const Ref<FlowScript> iter_scr = E.key;
		if (saved_scr == iter_scr)
		{
			handle_flow_script_saved(saved_scr);
			break;
		}
	}
}


void FlowScriptEditorPlugin::on_this_scene_closed(const String &p_filepath)
{
	for (const Ref<FlowScript> &scr : get_alpha_sorted_edited_flow_script_list())
	{
		if (scr->is_built_in() && scr->get_path().begins_with(p_filepath))
		{
			close_flow_script(scr, true);
		}
	}
}


void FlowScriptEditorPlugin::on_this_scene_saved(const String &p_filepath)
{
	for (const Ref<FlowScript> &scr : get_alpha_sorted_edited_flow_script_list())
	{
		if (scr->is_built_in() && scr->get_path().begins_with(p_filepath))
		{
			handle_flow_script_saved(scr);
		}
	}
}


void FlowScriptEditorPlugin::on_filesystem_dock_resource_removed(Ref<Resource> p_resource)
{
	const Ref<FlowScript> deleted_scr = p_resource;

	if (deleted_scr.is_null())
	{
		return;
	}
	for (const KeyValue<Ref<FlowScript>, EditedRoot> &E : map_edited_roots)
	{
		const Ref<FlowScript> iter_scr = E.key;
		if (iter_scr == deleted_scr)
		{
			close_flow_script(deleted_scr, true);
			clear_edit_history();
			break;
		}
	}
}


void FlowScriptEditorPlugin::on_common_edit_request(Ref<FlowScript> p_flow_script)
{
	edit_flow_script(p_flow_script);
}


void FlowScriptEditorPlugin::on_make_floating_button_request_open_in_screen(const int p_screen_id)
{
	window_wrapper->enable_window_on_screen(p_screen_id, true);
}


void FlowScriptEditorPlugin::on_window_visibility_changed(const bool p_visible)
{
	make_floating_button->set_visible(!p_visible);
}


void FlowScriptEditorPlugin::on_flow_script_item_list_item_selected(const int p_item)
{
	const Ref<FlowScript> scr = flow_script_item_list->get_item_metadata(p_item);
	if (scr.is_valid())
	{
		select_flow_script(scr);
	}
}


void FlowScriptEditorPlugin::on_flow_script_item_list_item_clicked(const int p_item, const Point2 &p_local_mouse_position, const MouseButton p_button_index)
{
	// TODO: Implement a context menu, perhaps one that has an option to save and close.
}


void FlowScriptEditorPlugin::on_flow_script_edit_sync_timer_timeout()
{
	queue_refresh_flow_script_item_list();
}


void FlowScriptEditorPlugin::on_current_selected_flow_script_changed()
{
	ERR_FAIL_COND(current_selected_flow_script.is_null());
	ERR_FAIL_COND(!map_edited_roots.has(current_selected_flow_script));

	EditedRoot &edited_root = map_edited_roots[current_selected_flow_script];
	edited_root.timestamp_last_change = OS::get_singleton()->get_unix_time();

	queue_refresh_flow_script_item_list();
}


String FlowScriptEditorPlugin::get_plugin_name() const
{
	return "FlowScript";
}


const Ref<Texture2D> FlowScriptEditorPlugin::get_plugin_icon() const
{
	const Ref<Texture2D> icon = EditorNode::get_singleton()->get_class_icon("FlowScript");
	return icon;
}


String FlowScriptEditorPlugin::get_plugin_version() const
{
	return "0.3";
}


void FlowScriptEditorPlugin::make_visible(bool p_visible)
{
	if (p_visible)
	{
		make_bottom_panel_item_visible(bottom_panel_button);
	}
}


bool FlowScriptEditorPlugin::handles(Object *p_object) const
{
	return Object::cast_to<FlowScript>(p_object) != nullptr;
}


void FlowScriptEditorPlugin::edit(Object *p_object)
{
	Ref<FlowScript> scr = Ref<FlowScript>(p_object);

	if (scr.is_null())
	{
		return;
	}

	edit_flow_script(scr);
}


void FlowScriptEditorPlugin::set_window_layout(Ref<ConfigFile> p_layout)
{
	dialogs->set_window_layout(p_layout);

	const Array d_open_script_list = p_layout->get_value("FlowScriptEditor", "open_flow_scripts", Array());
	const String d_selected_script_path = p_layout->get_value("FlowScriptEditor", "selected_flow_script", String());
	const int d_split = p_layout->get_value("FlowScriptEditor", "flow_script_split_offset", 0);

	const double new_split = ((double)d_split) * EDSCALE;
	main_control->set_split_offset(new_split);

	for (int i = 0; i < d_open_script_list.size(); i++)
	{
		const Dictionary root_data = d_open_script_list.get(i);
		if (root_data.has("path"))
		{
			const String path = root_data.get("path", String());
			const Ref<FlowScript> scr = ResourceLoader::load(path, "FlowScript");
			if (scr.is_valid() && add_flow_script_to_edited_list(scr))
			{
				const Dictionary editor_state = root_data.get("editor_state", Dictionary());
				EditedRoot &root = map_edited_roots[scr];
				root.editor->set_state(editor_state);
			}
		}
	}

	const Ref<FlowScript> sel_scr = ResourceLoader::load(d_selected_script_path, "FlowScript");
	if (sel_scr.is_valid())
	{
		select_flow_script(sel_scr);
	}
}


void FlowScriptEditorPlugin::get_window_layout(Ref<ConfigFile> p_layout)
{
	dialogs->get_window_layout(p_layout);

	if (!map_edited_roots.is_empty())
	{
		Array root_data_list;
		for (const Ref<FlowScript> &scr : get_alpha_sorted_edited_flow_script_list())
		{
			if (scr->is_built_in())
			{
				continue;
			}

			const EditedRoot &scr_editor_data = map_edited_roots[scr];

			Dictionary scr_data;
			scr_data["path"] = scr->get_path();
			scr_data["editor_state"] = scr_editor_data.editor->get_state();
			root_data_list.push_back(scr_data);
		}
		p_layout->set_value("FlowScriptEditor", "open_flow_scripts", root_data_list);
	}

	if (current_selected_flow_script.is_valid())
	{
		p_layout->set_value("FlowScriptEditor", "selected_flow_script", current_selected_flow_script->get_path());
	}

	const int split_ofs = Math::round(float(main_control->get_split_offset()) / EDSCALE);
	p_layout->set_value("FlowScriptEditor", "flow_script_split_offset", split_ofs);
}


String FlowScriptEditorPlugin::get_unsaved_status(const String &p_for_scene) const
{
	const Vector<Ref<FlowScript>> open_list = get_alpha_sorted_edited_flow_script_list();
	int unsaved_count = 0;

	for (const Ref<FlowScript> &scr : open_list)
	{
		ERR_CONTINUE(!map_edited_roots.has(scr));
		const EditedRoot &edited_root = map_edited_roots[scr];
		if (edited_root.timestamp_last_save < edited_root.timestamp_last_change)
		{
			if (p_for_scene.is_empty() && !scr->is_built_in())
			{
				unsaved_count++;
			}
			else if (!p_for_scene.is_empty() && scr->is_built_in() && scr->get_path().begins_with(p_for_scene))
			{
				unsaved_count++;
			}
		}
	}

	if (unsaved_count > 0)
	{
		if (p_for_scene.is_empty())
		{
			return vformat(TTRN("There is %d unsaved FlowScript. Save before closing?", "There are %d unsaved FlowScripts. Save before closing?", unsaved_count), unsaved_count);
		}
		else
		{
			return vformat(TTRN("Scene \"%s\" has %d unsaved FlowScript. Save before closing?", "Scene \"%s\" has %d unsaved FlowScripts. Save before closing?", unsaved_count), p_for_scene.get_file(), unsaved_count);
		}
	}
	else
	{
		return String();
	}
}


void FlowScriptEditorPlugin::save_external_data()
{
	save_all_flow_scripts();
}


bool FlowScriptEditorPlugin::is_flow_script_open(const Ref<FlowScript> &p_flow_script) const
{
	return p_flow_script.is_valid() && map_edited_roots.has(p_flow_script);
}


bool FlowScriptEditorPlugin::is_flow_script_selected(const Ref<FlowScript> &p_flow_script) const
{
	return p_flow_script.is_valid() && current_selected_flow_script == p_flow_script;
}


void FlowScriptEditorPlugin::edit_flow_script(const Ref<FlowScript> &p_flow_script)
{
	ERR_FAIL_COND(p_flow_script.is_null());

	if (!map_edited_roots.has(p_flow_script))
	{
		ERR_FAIL_COND(!add_flow_script_to_edited_list(p_flow_script));
	}

	select_flow_script(p_flow_script);
}


void FlowScriptEditorPlugin::_notification(int p_what)
{
	if (p_what == NOTIFICATION_READY)
	{
		FlowScriptNodeTypeDB::get_singleton()->init_editor();

		connect(SNAME("resource_saved"), callable_mp(this, &FlowScriptEditorPlugin::on_this_resource_saved));
		connect(SNAME("scene_closed"), callable_mp(this, &FlowScriptEditorPlugin::on_this_scene_closed));
		connect(SNAME("scene_saved"), callable_mp(this, &FlowScriptEditorPlugin::on_this_scene_saved));
		FileSystemDock::get_singleton()->connect(SNAME("resource_removed"), callable_mp(this, &FlowScriptEditorPlugin::on_filesystem_dock_resource_removed));
	}
	else if (p_what == NOTIFICATION_EXIT_TREE)
	{
		memdelete(dialogs);
		memdelete(clipboard);
		memdelete(common);
	}
}


void FlowScriptEditorPlugin::_bind_methods()
{
}


FlowScriptEditorPlugin::FlowScriptEditorPlugin()
{
	msdf_theme.instantiate();
	clipboard = memnew(FlowScriptEditorClipboard);
	dialogs = memnew(FlowScriptEditorDialogs);
	common = memnew(FlowScriptEditorCommonObject);
	common->init_dependencies(msdf_theme, clipboard, dialogs);
	common->connect(SNAME("edit_request"), callable_mp(this, &FlowScriptEditorPlugin::on_common_edit_request), CONNECT_DEFERRED);

	empty_style.instantiate();

	window_wrapper = memnew(WindowWrapper);
	window_wrapper->set_window_title(vformat(TTR("%s - Godot Engine"), TTR("FlowScript Editor")));
	window_wrapper->set_margins_enabled(true);
	window_wrapper->connect("window_visibility_changed", callable_mp(this, &FlowScriptEditorPlugin::on_window_visibility_changed));

	bottom_panel_button = EditorNode::get_bottom_panel()->add_item(TTR("FlowScript Editor"), window_wrapper, ED_SHORTCUT_AND_COMMAND("bottom_panels/toggle_flow_script_editor_bottom_panel", TTR("Toggle FlowScript Editor Bottom Panel")));

	main_control = memnew(HSplitContainer);
	Ref<Shortcut> make_floating_shortcut = ED_SHORTCUT_AND_COMMAND("flow_script_editor/make_floating", TTR("Make Floating"));
	window_wrapper->set_wrapped_control(main_control, make_floating_shortcut);

	VBoxContainer *left_vbox = memnew(VBoxContainer);
	left_vbox->set_custom_minimum_size(Size2(200, 300) * EDSCALE);
	main_control->add_child(left_vbox);

	HBoxContainer *menu_hbox = memnew(HBoxContainer);
	left_vbox->add_child(menu_hbox);

	file_menu_button = memnew(MenuButton);
	file_menu_button->set_text(TTR("File"));
	file_menu_button->set_shortcut_context(main_control);

	PopupMenu *file_menu_popup = file_menu_button->get_popup();
	file_menu_popup->add_item(TTR("New FlowScript..."), FILE_OPTION_NEW);
	file_menu_popup->add_separator();
	file_menu_popup->add_item(TTR("Load FlowScript File..."), FILE_OPTION_OPEN);
	file_menu_popup->add_shortcut(ED_SHORTCUT("flow_script_editor/save", TTR("Save File"), KeyModifierMask::ALT | KeyModifierMask::CMD_OR_CTRL | Key::S), FILE_OPTION_SAVE);
	file_menu_popup->add_separator();
	file_menu_popup->add_item(TTR("Close"), FILE_OPTION_CLOSE);
	file_menu_popup->add_item(TTR("Close All"), FILE_OPTION_CLOSE_ALL);
	file_menu_popup->connect(SceneStringName(id_pressed), callable_mp(this, &FlowScriptEditorPlugin::submit_file_option));
	menu_hbox->add_child(file_menu_button);

	update_file_menu();

	Control *menu_middle_pad = memnew(Control);
	menu_middle_pad->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	menu_hbox->add_child(menu_middle_pad);

	make_floating_button = memnew(ScreenSelect);
	make_floating_button->set_flat(true);
	make_floating_button->connect("request_open_in_screen", callable_mp(this, &FlowScriptEditorPlugin::on_make_floating_button_request_open_in_screen));
	if (!make_floating_button->is_disabled())
	{
		make_floating_button->set_tooltip_text(TTR("Make the FlowScript editor floating."));
	}
	menu_hbox->add_child(make_floating_button);

	flow_script_item_list = memnew(ItemList);
	flow_script_item_list->set_auto_translate_mode(Node::AUTO_TRANSLATE_MODE_DISABLED);
	flow_script_item_list->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	flow_script_item_list->connect("item_selected", callable_mp(this, &FlowScriptEditorPlugin::on_flow_script_item_list_item_selected));
	flow_script_item_list->connect("item_clicked", callable_mp(this, &FlowScriptEditorPlugin::on_flow_script_item_list_item_clicked));
	left_vbox->add_child(flow_script_item_list);

	editor_tabs = memnew(TabContainer);
	editor_tabs->set_tabs_visible(false);
	editor_tabs->set_deselect_enabled(true);
	editor_tabs->add_theme_style_override(SceneStringName(panel), empty_style);
	main_control->add_child(editor_tabs);

	flow_script_edit_sync_timer = memnew(Timer);
	flow_script_edit_sync_timer->set_autostart(false);
	flow_script_edit_sync_timer->set_one_shot(true);
	flow_script_edit_sync_timer->set_wait_time(1.0); // Should the wait time be configurable?
	flow_script_edit_sync_timer->connect("timeout", callable_mp(this, &FlowScriptEditorPlugin::on_flow_script_edit_sync_timer_timeout), CONNECT_DEFERRED);
	add_child(flow_script_edit_sync_timer);

	inspector_plugin_nodes.instantiate();
	add_inspector_plugin(inspector_plugin_nodes);
}


FlowScriptEditorPlugin::~FlowScriptEditorPlugin()
{
}
