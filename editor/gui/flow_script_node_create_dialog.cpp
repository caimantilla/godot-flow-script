#include "flow_script_node_create_dialog.hpp"
#include "editor/flow_script_node_type_info.hpp"
#include "editor/flow_script_node_type_db.hpp"
#include "editor/editor_paths.h"
#include "editor/editor_node.h"
#include "editor/themes/editor_scale.h"
#include "scene/gui/split_container.h"


void FlowScriptNodeCreateDialog::_bind_methods()
{
	ADD_SIGNAL(MethodInfo("type_chosen", PropertyInfo(Variant::STRING_NAME, "native_class"), PropertyInfo(Variant::STRING_NAME, "script_class")));
}


void FlowScriptNodeCreateDialog::_notification(int p_what)
{
	if (p_what == NOTIFICATION_VISIBILITY_CHANGED)
	{
		if (is_visible())
		{
			node_filter_line->call_deferred(SNAME("grab_focus"));
		}
	}
}


Tree *FlowScriptNodeCreateDialog::get_type_tree() const
{
	return type_tree;
}


bool FlowScriptNodeCreateDialog::is_node_type_selected() const
{
	TreeItem *selected_item = type_tree->get_selected();
	return tree_item_type_map.has(selected_item);
}


String FlowScriptNodeCreateDialog::get_current_search_filter_str() const
{
	return node_filter_line->get_text().strip_edges();
}


const FlowScriptNodeTypeInfo &FlowScriptNodeCreateDialog::get_selected_node_type() const
{
	TreeItem *selected_item = type_tree->get_selected();
	return local_node_type_list[tree_item_type_map[selected_item]];
}


void FlowScriptNodeCreateDialog::reload_local_type_list()
{
	List<FlowScriptNodeTypeInfo> temp_type_list;
	FlowScriptNodeTypeDB::get_singleton()->get_node_type_list(&temp_type_list);
	local_node_type_list.resize(temp_type_list.size());
	int curr_type_idx = 0;
	for (const FlowScriptNodeTypeInfo &type : temp_type_list)
	{
		local_node_type_list.write[curr_type_idx] = type;
		curr_type_idx++;
	}
	local_node_type_list.sort_custom<NodeTypeAlphaComparator>();
	refresh_type_tree();
}


void FlowScriptNodeCreateDialog::update_type_description_display()
{
	if (is_node_type_selected())
	{
		const FlowScriptNodeTypeInfo &type = get_selected_node_type();
		String description = type.description;
		if (description.is_empty())
		{
			description = TTR("No description available.");
		}
		help_bit->set_custom_text(TTR("Type:"), type.name, description);
		description_visibility_parent->show();
	}
	else
	{
		description_visibility_parent->hide();
	}
}


void FlowScriptNodeCreateDialog::emit_type_chosen(const FlowScriptNodeTypeInfo &p_type)
{
	emit_signal(SNAME("type_chosen"), p_type.node_class, p_type.node_script_class_name);
}


void FlowScriptNodeCreateDialog::emit_selected_type_chosen()
{
	if (!is_node_type_selected())
	{
		return;
	}
	const FlowScriptNodeTypeInfo &selected_type = get_selected_node_type();
	emit_type_chosen(selected_type);
}


void FlowScriptNodeCreateDialog::on_fold_action_button_item_pressed(int p_option_idx)
{
	switch (p_option_idx)
	{
		case FOLD_OPTION_EXPAND_ALL:
			type_tree_expand_all();
			break;
		case FOLD_OPTION_COLLAPSE_ALL:
			type_tree_collapse_all();
			break;
		default:
			CRASH_NOW_MSG(vformat(TTR("Invalid fold action: %d"), p_option_idx));
			break;
	}
}


void FlowScriptNodeCreateDialog::toggle_selected_node_favorite()
{
	if (!is_node_type_selected())
	{
		return;
	}
	const FlowScriptNodeTypeInfo &type = get_selected_node_type();
	String target_name;
	if (type.node_script_class_name == StringName())
	{
		target_name = type.node_class;
	}
	else
	{
		target_name = type.node_script_class_name;
	}
	bool removed = favorite_type_str_list.erase(target_name);
	if (!removed)
	{
		favorite_type_str_list.push_back(target_name);
	}
	save_favorite_types();
}


void FlowScriptNodeCreateDialog::type_tree_expand_all()
{
	type_tree->get_root()->set_collapsed_recursive(false);
}


void FlowScriptNodeCreateDialog::type_tree_collapse_all()
{
	type_tree->get_root()->set_collapsed(true);
}


void FlowScriptNodeCreateDialog::clear_type_tree()
{
	tree_item_type_map.clear();
	type_tree->clear();
}


void FlowScriptNodeCreateDialog::refresh_type_tree()
{
	clear_type_tree();

	String filter = get_current_search_filter_str();
	List<int> displayed_type_idx_list;

	if (filter.is_empty())
	{
		for (int i = 0; i < local_node_type_list.size(); i++)
		{
			displayed_type_idx_list.push_back(i);
		}
	}
	else
	{
		for (int i = 0; i < local_node_type_list.size(); i++)
		{
			const FlowScriptNodeTypeInfo &type = local_node_type_list[i];
			if (type.name.containsn(filter))
			{
				displayed_type_idx_list.push_back(i);
			}
		}
	}

	HashSet<String> category_set;
	for (const int type_idx : displayed_type_idx_list)
	{
		const FlowScriptNodeTypeInfo &type = local_node_type_list[type_idx];
		if (!type.category.is_empty())
		{
			category_set.insert(type.category);
		}
	}
	Vector<String> category_list;
	category_list.resize(category_set.size());
	{
		int i = 0;
		for (const String &category : category_set)
		{
			category_list.write[i] = category;
			i++;
		}
	}

	HashMap<String, TreeItem *> category_item_map;
	for (const String &category : category_list)
	{
		TreeItem *super_item = type_tree->get_root();
		PackedStringArray split = category.split("/", false, 0);
		for (int i = 1; i < split.size(); i++)
		{
			String super_category = String("/").join(split.slice(0, i - 1));
			if (!category_item_map.has(super_category))
			{
				super_item = super_item->create_child();
				super_item->set_selectable(MAIN_COLUMN, false);
				super_item->set_text(MAIN_COLUMN, split[i - 1]);
				category_item_map.insert(super_category, super_item);
			}
		}
		TreeItem *last_item = super_item->create_child();
		last_item->set_selectable(MAIN_COLUMN, false);
		last_item->set_text(MAIN_COLUMN, split[split.size() - 1]);
		category_item_map.insert(category, super_item->create_child());
	}
	for (const int type_idx : displayed_type_idx_list)
	{
		const FlowScriptNodeTypeInfo &type = local_node_type_list[type_idx];
		TreeItem *parent_item = type_tree->get_root();
		if (!type.category.is_empty() && category_item_map.has(type.category))
		{
			parent_item = category_item_map[type.category];
		}
		TreeItem *type_item = parent_item->create_child();
		type_item->set_selectable(MAIN_COLUMN, true);
		type_item->set_text(MAIN_COLUMN, type.name);
		tree_item_type_map.insert(type_item, type_idx);
	}
	type_tree_expand_all();
}


void FlowScriptNodeCreateDialog::save_favorite_types()
{
	Ref<FileAccess> file = FileAccess::open(EditorPaths::get_singleton()->get_project_settings_dir().path_join("favorites.FlowScriptNode"), FileAccess::WRITE);
	if (file.is_valid())
	{
		for (const String &fav_type_name : favorite_type_str_list)
		{
			if (fav_type_name.is_valid_identifier() && EditorNode::get_editor_data().is_type_recognized(fav_type_name))
			{
				file->store_line(fav_type_name);
			}
		}
		file->close();
	}
}


void FlowScriptNodeCreateDialog::load_favorite_types()
{
	favorite_type_str_list.clear();
	Ref<FileAccess> file = FileAccess::open(EditorPaths::get_singleton()->get_project_data_dir().path_join("favorites.FlowScriptNode"), FileAccess::READ);
	if (file.is_valid())
	{
		while (!file->eof_reached())
		{
			String fav_type_name = file->get_line().strip_edges();
			if (fav_type_name.is_valid_identifier() && EditorNode::get_editor_data().is_type_recognized(fav_type_name))
			{
				favorite_type_str_list.push_back(fav_type_name);
			}
		}
	}
}


int FlowScriptNodeCreateDialog::get_node_type_index_by_class_name(const StringName &p_class_name) const
{
	for (int i = 0; i < local_node_type_list.size(); i++)
	{
		if (local_node_type_list[i].node_class == p_class_name || local_node_type_list[i].node_script_class_name == p_class_name)
		{
			return i;
		}
	}
	return -1;
}


void FlowScriptNodeCreateDialog::refresh_quick_access_node_item_list(ItemList *p_item_list, const PackedStringArray &p_type_class_name_list)
{
	p_item_list->clear();
	for (const String &class_name : p_type_class_name_list)
	{
		int type_idx = get_node_type_index_by_class_name(class_name);
		if (type_idx == -1)
		{
			continue;
		}
		const FlowScriptNodeTypeInfo &type = local_node_type_list[type_idx];
		int item_idx = p_item_list->add_item(type.name);
		p_item_list->set_item_metadata(item_idx, StringName(class_name));
		p_item_list->set_item_tooltip(item_idx, class_name);
		p_item_list->set_item_tooltip_enabled(item_idx, true);
	}
}


void FlowScriptNodeCreateDialog::gui_update_favorite_item_list()
{
	refresh_quick_access_node_item_list(item_list_favorites, favorite_type_str_list);
}


void FlowScriptNodeCreateDialog::on_mark_favorite_button_toggled(bool p_toggled_on)
{
	if (!is_node_type_selected())
	{
		return;
	}
	const FlowScriptNodeTypeInfo &type = get_selected_node_type();
	String to_add;
	if (type.node_script_class_name == StringName())
	{
		to_add = type.node_class;
	}
	else
	{
		to_add = type.node_script_class_name;
	}
	if (p_toggled_on)
	{
		ERR_FAIL_COND(favorite_type_str_list.has(to_add));
		favorite_type_str_list.push_back(to_add);
	}
	else
	{
		bool erase_ok = favorite_type_str_list.erase(to_add);
		ERR_FAIL_COND(!erase_ok);
	}
}


void FlowScriptNodeCreateDialog::update_mark_favorite_button_toggle_state()
{
	if (!is_node_type_selected())
	{
		mark_favorite_button->set_disabled(true);
		mark_favorite_button->set_pressed_no_signal(false);
		return;
	}
	mark_favorite_button->set_disabled(false);
	const FlowScriptNodeTypeInfo &type = get_selected_node_type();
	bool is_selected_favorite = false;
	for (const String &class_name : favorite_type_str_list)
	{
		if (class_name == type.node_class || class_name == type.node_script_class_name)
		{
			is_selected_favorite = true;
			break;
		}
	}
	mark_favorite_button->set_pressed_no_signal(is_selected_favorite);
}


void FlowScriptNodeCreateDialog::handle_quick_access_node_item_list_item_selected(ItemList *p_item_list, int p_item_idx)
{
	Variant meta = p_item_list->get_item_metadata(p_item_idx);
	ERR_FAIL_COND(meta.get_type() != Variant::STRING_NAME);
	StringName class_name = meta;
	int type_idx = get_node_type_index_by_class_name(class_name);
	ERR_FAIL_COND(type_idx == -1);
	emit_type_chosen(local_node_type_list[type_idx]);
}


void FlowScriptNodeCreateDialog::on_favorite_list_item_selected(int p_item_idx)
{
	handle_quick_access_node_item_list_item_selected(item_list_favorites, p_item_idx);
}


void FlowScriptNodeCreateDialog::on_recent_list_item_selected(int p_item_idx)
{
	handle_quick_access_node_item_list_item_selected(item_list_recents, p_item_idx);
}


void FlowScriptNodeCreateDialog::on_node_filter_search_line_text_changed(const String &p_text)
{
	refresh_type_tree();
}


void FlowScriptNodeCreateDialog::handle_node_filter_search_line_gui_input_event(const Ref<InputEvent> &p_event)
{
	// do some stuff later idk like use ui_accept to choose the closest-matching node or something
}


void FlowScriptNodeCreateDialog::on_this_confirmed()
{
	emit_selected_type_chosen();
}


FlowScriptNodeCreateDialog::FlowScriptNodeCreateDialog()
{
	connect("confirmed", callable_mp(this, &FlowScriptNodeCreateDialog::on_this_confirmed));

	set_flag(FLAG_RESIZE_DISABLED, false);
	set_wrap_controls(true);
	set_min_size((Size2(400, 250) * EDSCALE));

	HSplitContainer *main_split = memnew(HSplitContainer);
	add_child(main_split);

	VSplitContainer *left_vsplit = memnew(VSplitContainer);
	main_split->add_child(left_vsplit);

	VBoxContainer *favorites_vbox = memnew(VBoxContainer);
	favorites_vbox->set_custom_minimum_size(Size2(150, 100) * EDSCALE);
	favorites_vbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	left_vsplit->add_child(favorites_vbox);

	item_list_favorites = memnew(ItemList);
	item_list_favorites->connect("item_selected", callable_mp(this, &FlowScriptNodeCreateDialog::on_favorite_list_item_selected));
	favorites_vbox->add_margin_child(TTR("Favorites:"), item_list_favorites, true);

	VBoxContainer *recents_vbox = memnew(VBoxContainer);
	recents_vbox->set_custom_minimum_size(Size2(150, 100) * EDSCALE);
	recents_vbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	left_vsplit->add_child(recents_vbox);

	item_list_recents = memnew(ItemList);
	item_list_recents->connect("item_selected", callable_mp(this, &FlowScriptNodeCreateDialog::on_recent_list_item_selected));
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

	node_filter_line = memnew(LineEdit);
	node_filter_line->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	node_filter_line->set_editable(false);
	node_filter_line->set_placeholder(TTR("Search"));
	node_filter_line->set_tooltip_text(TTR("The search filter will be enabled in a future release."));
	node_filter_line->connect("text_changed", callable_mp(this, &FlowScriptNodeCreateDialog::on_node_filter_search_line_text_changed));
	node_filter_line->connect(SceneStringName(gui_input), callable_mp(this, &FlowScriptNodeCreateDialog::handle_node_filter_search_line_gui_input_event));
	top_hbox->add_child(node_filter_line);

	fold_action_menu = memnew(MenuButton);
	fold_action_menu->set_tooltip_text(TTR("Fold Actions"));
	fold_action_menu->get_popup()->connect(SceneStringName(id_pressed), callable_mp(this, &FlowScriptNodeCreateDialog::on_fold_action_button_item_pressed));
	fold_action_menu->get_popup()->add_item(TTR("Expand All"), FOLD_OPTION_EXPAND_ALL);
	fold_action_menu->get_popup()->add_item(TTR("Collapse All"), FOLD_OPTION_COLLAPSE_ALL);
	top_hbox->add_child(fold_action_menu);

	mark_favorite_button = memnew(Button);
	mark_favorite_button->set_toggle_mode(true);
	mark_favorite_button->connect("toggled", callable_mp(this, &FlowScriptNodeCreateDialog::on_mark_favorite_button_toggled));
	top_hbox->add_child(mark_favorite_button);

	type_tree = memnew(Tree);
	type_tree->connect("item_selected", callable_mp(this, &FlowScriptNodeCreateDialog::update_mark_favorite_button_toggle_state));
	type_tree->connect("item_selected", callable_mp(this, &FlowScriptNodeCreateDialog::update_type_description_display));
	type_tree->connect("item_activated", callable_mp(this, &FlowScriptNodeCreateDialog::emit_selected_type_chosen));
	type_tree->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	type_tree->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	type_tree->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	type_tree->set_focus_mode(Control::FOCUS_CLICK);
	type_tree->set_hide_root(true);
	type_tree->set_allow_reselect(true);
	right_vbox->add_child(type_tree);

	help_bit = memnew(EditorHelpBit);
	description_visibility_parent = right_vbox->add_margin_child(TTR("Description:"), help_bit, false);
	description_visibility_parent->hide();
}
