#ifndef FLOW_SCRIPT_NODE_CREATE_DIALOG_HPP
#define FLOW_SCRIPT_NODE_CREATE_DIALOG_HPP


#include "core/templates/hash_map.h"
#include "core/templates/list.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/tree.h"
#include "scene/gui/margin_container.h"
#include "scene/gui/item_list.h"
#include "editor/editor_help.h"


class FlowScriptNodeCreateDialog final : public ConfirmationDialog
{
	GDCLASS(FlowScriptNodeCreateDialog, ConfirmationDialog);

private:
	enum
	{
		MAIN_COLUMN = 0,
	};
	enum
	{
		FOLD_OPTION_EXPAND_ALL = 0,
		FOLD_OPTION_COLLAPSE_ALL = 1,
	};

	struct NodeTypeAlphaComparator final
	{
		_FORCE_INLINE_ bool operator()(const FlowScriptNodeTypeInfo &p_a, const FlowScriptNodeTypeInfo &p_b) const
		{
			return p_a.category.count("/") > p_b.category.count("/") || (p_a.category + "/" + p_a.name).naturalnocasecmp_to(p_b.category + "/" + p_b.name) < 0;
		}
	};

private:
	Vector<FlowScriptNodeTypeInfo> local_node_type_list;
	HashMap<TreeItem *, int> tree_item_type_map;
	PackedStringArray favorite_type_str_list;
	PackedStringArray recent_type_str_list;
	LineEdit *node_filter_line;
	MenuButton *fold_action_menu;
	Button *mark_favorite_button;
	Tree *type_tree;
	MarginContainer *description_visibility_parent;
	EditorHelpBit *help_bit;
	ItemList *item_list_favorites;
	ItemList *item_list_recents;

	void reload_local_type_list();
	void clear_type_tree();
	void refresh_type_tree();
	void update_type_description_display();
	void emit_type_chosen(const FlowScriptNodeTypeInfo &p_type);
	void emit_selected_type_chosen();
	void type_tree_expand_all();
	void type_tree_collapse_all();
	void on_fold_action_button_item_pressed(int p_option_idx);
	void toggle_selected_node_favorite();
	void save_favorite_types();
	void load_favorite_types();
	void gui_update_favorite_item_list();
	void on_mark_favorite_button_toggled(bool p_toggled_on);
	void update_mark_favorite_button_toggle_state();
	void handle_quick_access_node_item_list_item_selected(ItemList *p_item_list, int p_item_idx);
	void on_favorite_list_item_selected(int p_item_idx);
	void on_recent_list_item_selected(int p_item_idx);
	int get_node_type_index_by_class_name(const StringName &p_class_name) const;
	// Used to refresh the recent and favorite node lists
	void refresh_quick_access_node_item_list(ItemList *p_item_list, const PackedStringArray &p_type_class_name_list);
	void on_node_filter_search_line_text_changed(const String &p_text);
	void handle_node_filter_search_line_gui_input_event(const Ref<InputEvent> &p_event);
	void on_this_confirmed();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	Tree *get_type_tree() const;
	bool is_node_type_selected() const;
	const FlowScriptNodeTypeInfo &get_selected_node_type() const;
	String get_current_search_filter_str() const;

	FlowScriptNodeCreateDialog();
};


#endif // FLOW_SCRIPT_NODE_CREATE_DIALOG_HPP
