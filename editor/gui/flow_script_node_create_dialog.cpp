#include "flow_script_node_create_dialog.hpp"
#include "../flow_script_node_type_db.hpp"
#include "core/config/project_settings.h"
#include "scene/gui/split_container.h"
#include "editor/editor_paths.h"
#include "editor/editor_node.h"
#include "editor/editor_string_names.h"
#include "editor/themes/editor_scale.h"


void FlowScriptNodeCreateDialog::_bind_methods()
{
	GLOBAL_DEF(PropertyInfo(Variant::INT, "flow_script/editor/types/list_mode", PROPERTY_HINT_ENUM, "Blacklist,Whitelist"), CFG_BLACKLIST);
	GLOBAL_DEF(PropertyInfo(Variant::PACKED_STRING_ARRAY, "flow_script/editor/types/list"), PackedStringArray());

	ADD_SIGNAL(MethodInfo("type_chosen", PropertyInfo(Variant::STRING_NAME, "native_class"), PropertyInfo(Variant::STRING_NAME, "script_class")));
}


void FlowScriptNodeCreateDialog::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_POSTINITIALIZE: {
			connect("confirmed", callable_mp(this, &FlowScriptNodeCreateDialog::on_this_confirmed));
		} break;
		case NOTIFICATION_VISIBILITY_CHANGED: {
			if (is_visible())
			{
				if (reload_types_on_open_queued)
				{
					reload_types_on_open_queued = false;
					reload_local_type_list();
				}
				node_filter_line->call_deferred(SNAME("grab_focus"));
			}
		} break;
		case NOTIFICATION_THEME_CHANGED: {
			Ref<Texture2D> icon_fav = get_theme_icon(SNAME("Favorites"), EditorStringName(EditorIcons));
			Ref<Texture2D> icon_tool = get_theme_icon(SNAME("Tools"), EditorStringName(EditorIcons));
			mark_favorite_button->set_icon(icon_fav);
			fold_action_menu->set_icon(icon_tool);
		} break;
	}
}


Tree *FlowScriptNodeCreateDialog::get_type_tree() const
{
	return type_tree;
}


bool FlowScriptNodeCreateDialog::is_node_type_selected() const
{
	TreeItem *selected_item = type_tree->get_selected();
	return selected_item != nullptr && tree_item_type_map.has(selected_item);
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
	local_node_type_list.clear();

	int cfg_types_mode = GLOBAL_GET("flow_script/editor/types/list_mode");
	PackedStringArray cfg_types_list = GLOBAL_GET("flow_script/editor/types/list");

	HashSet<String> cfg_types_set;
	cfg_types_set.reserve(cfg_types_list.size());

	for (const String &type : cfg_types_list)
	{
		cfg_types_set.insert(type.strip_edges());
	}

	List<FlowScriptNodeTypeInfo> temp_type_list;
	FlowScriptNodeTypeDB::get_singleton()->get_node_type_list(&temp_type_list);

	for (const FlowScriptNodeTypeInfo &type : temp_type_list)
	{
		const String type_name = type.native ? type.node_class : type.node_script_class_name;
		bool skip_type = false;
		switch (cfg_types_mode)
		{
			case CFG_BLACKLIST: {
				skip_type = cfg_types_set.has(type_name);
			} break;
			case CFG_WHITELIST: {
				skip_type = !cfg_types_set.has(type_name);
			} break;
		}
		if (!skip_type)
		{
			local_node_type_list.push_back(type);
		}
	}

	local_node_type_list.sort_custom<NodeTypeAlphaComparator>();
	refresh_type_tree();

	load_favorite_types();
	load_recent_types();
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
	}
	else
	{
		help_bit->set_custom_text(TTR("Type:"), TTR("No type selected."), "");
	}
}


void FlowScriptNodeCreateDialog::emit_type_chosen(const FlowScriptNodeTypeInfo &p_type)
{
	add_node_type_to_recent(p_type);
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


void FlowScriptNodeCreateDialog::add_node_type_to_recent(const FlowScriptNodeTypeInfo &p_type)
{
	String target_name;
	if (p_type.node_script_class_name == StringName())
	{
		target_name = p_type.node_class;
	}
	else
	{
		target_name = p_type.node_script_class_name;
	}
	recent_type_str_list.erase(target_name);
	recent_type_str_list.push_back(target_name);
	if (recent_type_str_list.size() > RECENT_HISTORY_MAX_SIZE)
	{
		recent_type_str_list.remove_at(0);
	}
	save_recent_types();
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


void FlowScriptNodeCreateDialog::clear_type_tree()
{
	tree_item_type_map.clear();
	type_tree->clear();
}


void FlowScriptNodeCreateDialog::refresh_type_tree()
{
	clear_type_tree();
	type_tree->create_item(); // init the root

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
			const String super_category = String("/").join(split.slice(0, i - 1));
			if (!super_category.is_empty() && !category_item_map.has(super_category))
			{
				const String super_item_name = split[i - 1];

				super_item = super_item->create_child();
				super_item->set_selectable(MAIN_COLUMN, false);
				super_item->set_text(MAIN_COLUMN, super_item_name);
				category_item_map.insert(super_category, super_item);
			}
		}
		String last_item_name = split[split.size() - 1];

		TreeItem *last_item = super_item->create_child();
		last_item->set_selectable(MAIN_COLUMN, false);
		last_item->set_text(MAIN_COLUMN, last_item_name);
		category_item_map.insert(category, last_item);
	}

	// variables for search filter
	TreeItem *item_to_auto_select = nullptr;
	real_t last_item_similarity = INFINITY;

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

		if (!filter.is_empty())
		{
			real_t curr_similarity = filter.similarity(type.name);
			if (curr_similarity < last_item_similarity)
			{
				last_item_similarity = curr_similarity;
				item_to_auto_select = type_item;
			}
		}
	}

	// Move categories to be displayed after types
	for (KeyValue<String, TreeItem *> &E : category_item_map)
	{
		TreeItem *item = E.value;
		TreeItem *parent = item->get_parent();
		int child_count = parent->get_child_count();
		if (child_count > 1)
		{
			item->move_after(parent->get_child(child_count - 1));
		}
	}

	type_tree_expand_all();

	if (item_to_auto_select != nullptr)
	{
		type_tree->set_selected(item_to_auto_select, MAIN_COLUMN);
	}
}


void FlowScriptNodeCreateDialog::save_quick_access_type_list(const PackedStringArray &p_class_list, const String &p_filename)
{
	Ref<FileAccess> file = FileAccess::open(EditorPaths::get_singleton()->get_project_settings_dir().path_join(p_filename), FileAccess::WRITE);
	if (file.is_valid())
	{
		for (const String &type_name : p_class_list)
		{
			if (type_name.is_valid_identifier() && EditorNode::get_editor_data().is_type_recognized(type_name))
			{
				file->store_line(type_name);
			}
		}
		file->close();
	}
}


void FlowScriptNodeCreateDialog::load_quick_access_type_list(PackedStringArray &p_class_list, const String &p_filename)
{
	p_class_list.clear();
	Ref<FileAccess> file = FileAccess::open(EditorPaths::get_singleton()->get_project_settings_dir().path_join(p_filename), FileAccess::READ);
	if (file.is_valid())
	{
		while (!file->eof_reached())
		{
			String type_name = file->get_line().strip_edges().get_slicec(' ', 0);
			if (type_name.is_valid_identifier() && EditorNode::get_editor_data().is_type_recognized(type_name))
			{
				p_class_list.push_back(type_name);
			}
		}
	}
}


void FlowScriptNodeCreateDialog::save_favorite_types()
{
	save_quick_access_type_list(favorite_type_str_list, "favorites.FlowScriptNode");
}


void FlowScriptNodeCreateDialog::load_favorite_types()
{
	load_quick_access_type_list(favorite_type_str_list, "favorites.FlowScriptNode");
	refresh_quick_access_node_item_list(item_list_favorites, favorite_type_str_list, false);
}


void FlowScriptNodeCreateDialog::save_recent_types()
{
	save_quick_access_type_list(recent_type_str_list, "create_recent.FlowScriptNode");
	refresh_quick_access_node_item_list(item_list_recents, recent_type_str_list, true);
}


void FlowScriptNodeCreateDialog::load_recent_types()
{
	load_quick_access_type_list(recent_type_str_list, "create_recent.FlowScriptNode");
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


void FlowScriptNodeCreateDialog::step_add_class_to_quick_access_node_item_list(ItemList *p_item_list, const StringName &p_class_name)
{
	int type_idx = get_node_type_index_by_class_name(p_class_name);
	if (type_idx == -1)
	{
		return;
	}
	const FlowScriptNodeTypeInfo &type = local_node_type_list[type_idx];
	int item_idx = p_item_list->add_item(type.name);
	p_item_list->set_item_metadata(item_idx, p_class_name);
	p_item_list->set_item_tooltip(item_idx, p_class_name);
	p_item_list->set_item_tooltip_enabled(item_idx, true);
}


void FlowScriptNodeCreateDialog::refresh_quick_access_node_item_list(ItemList *p_item_list, const PackedStringArray &p_type_class_name_list, const bool p_reverse)
{
	p_item_list->clear();

	if (p_reverse)
	{
		for (int i = 0; i < p_type_class_name_list.size(); i++)
		{
			step_add_class_to_quick_access_node_item_list(p_item_list, p_type_class_name_list[i]);
		}
	}
	else
	{
		for (int i = p_type_class_name_list.size() - 1; i > -1; i--)
		{
			step_add_class_to_quick_access_node_item_list(p_item_list, p_type_class_name_list[i]);
		}
	}
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
	p_item_list->deselect_all();

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
	Ref<InputEventKey> key_event = p_event;
	if (key_event.is_valid())
	{
		if (key_event->is_action(SNAME("ui_up")) || key_event->is_action(SNAME("ui_down")) || key_event->is_action(SNAME("ui_accept")))
		{
			type_tree->gui_input(key_event);
			node_filter_line->accept_event();
		}
	}
}


void FlowScriptNodeCreateDialog::on_this_confirmed()
{
	emit_selected_type_chosen();
}


// rebuilt the type db if needed otherwise queue it for when the window opens
void FlowScriptNodeCreateDialog::reload_types_if_visible()
{
	if (is_visible())
	{
		reload_types_on_open_queued = false;
		reload_local_type_list();
	}
	else
	{
		reload_types_on_open_queued = true;
	}
}


FlowScriptNodeCreateDialog::FlowScriptNodeCreateDialog()
{
	FlowScriptNodeTypeDB::get_singleton()->connect(CoreStringName(changed), callable_mp(this, &FlowScriptNodeCreateDialog::reload_types_if_visible));
	ProjectSettings::get_singleton()->connect("settings_changed", callable_mp(this, &FlowScriptNodeCreateDialog::reload_types_if_visible));

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
	item_list_favorites->set_allow_reselect(true);
	item_list_favorites->connect("item_selected", callable_mp(this, &FlowScriptNodeCreateDialog::on_favorite_list_item_selected));
	favorites_vbox->add_margin_child(TTR("Favorites:"), item_list_favorites, true);

	VBoxContainer *recents_vbox = memnew(VBoxContainer);
	recents_vbox->set_custom_minimum_size(Size2(150, 100) * EDSCALE);
	recents_vbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	left_vsplit->add_child(recents_vbox);

	item_list_recents = memnew(ItemList);
	item_list_recents->set_allow_reselect(true);
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
	node_filter_line->set_editable(true);
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
	type_tree->set_focus_mode(Control::FOCUS_NONE); // inputs SHOULD be passed to the type tree from the search bar
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
	update_type_description_display();
}
