#ifndef FLOW_SCRIPT_EDITOR_PLUGIN_HPP
#define FLOW_SCRIPT_EDITOR_PLUGIN_HPP


#include "../flow_script_node_type_info.hpp"
#include "../../flow_script.hpp"
#include "../../flow_script_node.hpp"

#include "core/object/object.h"
#include "core/object/script_language.h"
#include "core/object/script_instance.h"
#include "core/io/resource.h"
#include "scene/resources/packed_scene.h"
#include "scene/resources/theme.h"
#include "scene/gui/graph_edit.h"
#include "scene/gui/graph_edit_arranger.h"
#include "scene/gui/graph_node.h"
#include "scene/gui/dialogs.h"
#include "editor/editor_inspector.h"
#include "editor/plugins/editor_plugin.h"


class Timer;
class Button;
class MenuButton;
class ItemList;
class TabContainer;
class HSplitContainer;
class MarginContainer;
class Tree;
class Label;
class LineEdit;
class WindowWrapper;
class ScreenSelect;
class EditorFileDialog;
class EditorHelpBit;


class EditorInspectorPluginFlowScriptNode final : public EditorInspectorPlugin
{
	GDCLASS(EditorInspectorPluginFlowScriptNode, EditorInspectorPlugin);

public:
	virtual bool can_handle(Object *p_object) override;
	virtual bool parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide = false) override;
};


class FlowScriptEditorNodeCreateDialog final : public ConfirmationDialog
{
	GDCLASS(FlowScriptEditorNodeCreateDialog, ConfirmationDialog);

private:
	enum FoldAction
	{
		FOLD_EXPAND_ALL = 0,
		FOLD_COLLAPSE_ALL,
	};

private:
	struct NodeTypeAlphaComparator final
	{
		_FORCE_INLINE_ bool operator()(const FlowScriptNodeTypeInfo &p_a, const FlowScriptNodeTypeInfo &p_b) const
		{
			return (
				p_a.type_category.count("/") > p_b.type_category.count("/")
				|| (p_a.type_category + "/" + p_a.type_name).naturalnocasecmp_to(p_b.type_category + "/" + p_b.type_name) < 0
			);
		}
	};

private:
	LocalVector<FlowScriptNodeTypeInfo> local_type_info_list;
	LocalVector<FlowScriptNodeTypeInfo> favorite_type_info_list;
	LocalVector<FlowScriptNodeTypeInfo> recent_type_info_list;

	ItemList *item_list_favorites;
	ItemList *item_list_recents;
	MarginContainer *description_visibility_parent;
	EditorHelpBit *help_bit;
	LineEdit *type_search_filter_line;
	MenuButton *fold_action_menu_button;
	Button *mark_type_favorite_button;
	Tree *type_tree;

	void update_mark_type_favorite_button();
	void update_type_description();
	void rebuild_type_tree_gui();
	void clear_type_tree_gui();
	void expand_type_tree_gui();
	void collapse_type_tree_gui();
	void clear_type_list();
	void refresh_type_list();
	static String create_type_tooltip_text(const FlowScriptNodeTypeInfo &p_type_info);
	static void quick_access_type_list_gui_add_type(ItemList *p_item_list, const FlowScriptNodeTypeInfo &p_type_info);
	static void update_quick_access_type_list_gui(ItemList *p_item_list, const LocalVector<FlowScriptNodeTypeInfo> &p_type_info_list);
	static String get_quick_access_type_list_filepath(const String &p_filename);
	static void save_quick_access_type_list(const LocalVector<FlowScriptNodeTypeInfo> &p_type_info_list, const String &p_filename);
	static void load_quick_access_type_list(LocalVector<FlowScriptNodeTypeInfo> *p_type_info_list, const String &p_filename);
	void save_favorite_types();
	void load_favorite_types();
	void save_recent_types();
	void load_recent_types();
	void choose_type(const FlowScriptNodeTypeInfo &p_type_info);

	void on_this_confirmed();
	// void on_flow_script_type_db_changed();
	void on_item_list_favorites_item_selected(const int p_item);
	void on_item_list_recents_item_selected(const int p_item);
	void on_type_search_filter_line_gui_input(const Ref<InputEvent> &p_event);
	void on_type_search_filter_line_text_changed(const String &p_new_text);
	void on_fold_action_menu_popup_id_pressed(const int p_id);
	void on_mark_type_favorite_button_toggled(const bool p_toggled_on);
	void on_type_tree_item_activated();
	void on_type_tree_item_selected();
	void on_type_tree_nothing_selected();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	bool is_any_type_selected() const;
	FlowScriptNodeTypeInfo get_selected_type_info() const;
	String get_search_filter() const;

	FlowScriptEditorNodeCreateDialog();
};


class FlowScriptEditorNodeRenameDialog final : public ConfirmationDialog
{
	GDCLASS(FlowScriptEditorNodeRenameDialog, ConfirmationDialog);

private:
	LineEdit *line_edit;

protected:
	void _notification(int p_what);

public:
	void set_new_node_name(const String &p_name);
	String get_new_node_name() const;

	FlowScriptEditorNodeRenameDialog();
};


class FlowScriptEditorSubIncludeNode final : public GraphNode
{
	GDCLASS(FlowScriptEditorSubIncludeNode, GraphNode);

public:
	enum
	{
		ANON_INPUT_PORT = 0,
	};

private:
	String filename;
	Label *file_label;

	void update_file_label();

public:
	void set_filename(const String &p_name);
	String get_filename() const;

	FlowScriptEditorSubIncludeNode();
};


class FlowScriptEditorIncludeFrame final : public GraphFrame
{
	GDCLASS(FlowScriptEditorIncludeFrame, GraphFrame);

private:
	FlowScriptIncludeID include_id = FlowScriptConstants::INCLUDE_ID_INVALID;
	Button *edit_button;
	Button *delete_button;

	void on_edit_button_pressed();
	void on_delete_button_pressed();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	Button *get_edit_button() const;
	Button *get_delete_button() const;
	void set_include_id(const FlowScriptIncludeID p_id);
	FlowScriptIncludeID get_include_id() const;

	FlowScriptEditorIncludeFrame();
};


class FlowScriptEditorGraph final : public GraphEdit
{
	GDCLASS(FlowScriptEditorGraph, GraphEdit);

private:
	Callable callback_is_node_hover_valid;

	void sync_editor_settings();

protected:
	void _notification(int p_what);

public:
	void init_dependencies(Callable p_callback_is_node_hover_valid);

	virtual bool is_node_hover_valid(const StringName &p_from, const int p_from_port, const StringName &p_to, const int p_to_port) override;

	FlowScriptEditorGraph();
};


class FlowScriptEditorClipboard final
{
public:
	struct Node final
	{
		Ref<FlowScriptNode> data;
		Rect2i data_rect;
		Point2 screen_center_offset;
	};

	struct Connection final
	{
		int from_node;
		FlowScriptNodeOutputConnection from_output;
		int to_node;
	};

private:
	LocalVector<Node, int> list_nodes;
	LocalVector<Connection, int> list_connections;

public:
	void clear();
	bool is_empty() const;

	int get_node_count() const;
	Node get_node(const int p_index) const;
	void add_node(const Node &p_node);

	int get_connection_count() const;
	Connection get_connection(const int p_index) const;
	void add_connection(const Connection &p_connection);
};


class FlowScriptEditorDialogs final
{
private:
#if 0
	const Window *main_window = nullptr;
#endif // 0

	FlowScriptEditorNodeCreateDialog *node_create_dialog;
	FlowScriptEditorNodeRenameDialog *node_rename_dialog;
	ConfirmationDialog *confirm_unsaved_flow_script_dialog;
	ConfirmationDialog *confirm_delete_elements_dialog;
	AcceptDialog *alert_cannot_add_include_dialog;
	AcceptDialog *alert_cannot_add_node_dialog;
	AcceptDialog *alert_cannot_rename_node_dialog;

	Callable cache_create_node_callback; // node_id: FlowScriptNodeID, type_info: Dictionary, position: Point2
	Ref<FlowScript> cache_create_node_target_flow_script;
	FlowScriptNodeID cache_create_node_id = FlowScriptConstants::NODE_ID_INVALID;
	Point2 cache_create_node_position;

	Callable cache_rename_node_callback; // node_id: FlowScriptNodeID, new_name: StringName
	Ref<FlowScript> cache_rename_node_target_flow_script;
	FlowScriptNodeID cache_rename_node_id = FlowScriptConstants::NODE_ID_INVALID;

	Callable cache_add_include_callback; // include_id: FlowScriptIncludeID, flow_script: FlowScript, position: Point2
	Ref<FlowScript> cache_add_include_target_flow_script;
	FlowScriptIncludeID cache_add_include_id = FlowScriptConstants::INCLUDE_ID_INVALID;
	Point2 cache_add_include_position;

	Callable confirm_unsaved_flow_script_save_callback; // (void)
	Callable confirm_unsaved_flow_script_discard_callback; // (void)

	Callable cache_delete_elements_callback; // include_id_list: PackedFlowScriptIncludeIDArray, node_id_list: PackedFlowScriptNodeIDArray
	Ref<FlowScript> cache_delete_elements_target_flow_script;
	PackedFlowScriptIncludeIDArray cache_delete_include_id_list;
	PackedFlowScriptNodeIDArray cache_delete_node_id_list;

	void on_node_create_dialog_type_chosen(const Dictionary &p_type_info);
	void on_node_rename_dialog_confirmed();
	void on_confirm_unsaved_flow_script_dialog_confirmed();
	void on_confirm_unsaved_flow_script_dialog_custom_action(const StringName &p_action);
	void on_confirm_delete_elements_dialog_confirmed();
	void on_add_include_quick_open_path_selected(const String &p_path);
	void on_alert_cannot_add_include_dialog_confirmed();

public:
#if 0
	void set_main_window(const Window *p_window);
	const Window *get_main_window() const;
#endif // 0

	void popup_create_node(Callable p_create_node_callback, const Ref<FlowScript> &p_flow_script, const FlowScriptNodeID p_node_id, const Point2 &p_at_position);
	void popup_rename_node(Callable p_rename_node_callback, const Ref<FlowScript> &p_flow_script, const FlowScriptNodeID p_node_id);
	void popup_add_include(Callable p_add_include_callback, const Ref<FlowScript> &p_root_flow_script, const FlowScriptIncludeID p_include_id, const Point2 &p_at_position);
	void confirm_close_unsaved_flow_script(Callable p_save_callback, Callable p_discard_callback, const Ref<FlowScript> &p_flow_script, const double p_last_save_time, const double p_last_edit_time);
	void confirm_delete_elements(Callable p_delete_confirmed_callback, const Ref<FlowScript> &p_flow_script, const PackedFlowScriptIncludeIDArray &p_include_id_list, const PackedFlowScriptNodeIDArray &p_node_id_list);

	void set_window_layout(Ref<ConfigFile> p_layout);
	void get_window_layout(Ref<ConfigFile> p_layout);

	FlowScriptEditorDialogs();
	~FlowScriptEditorDialogs();
};


class FlowScriptEditorCommonObject final : public Object
{
	GDCLASS(FlowScriptEditorCommonObject, Object);

private:
	Ref<Theme> msdf_theme;
	FlowScriptEditorClipboard *clipboard;
	FlowScriptEditorDialogs *dialogs;

protected:
	static void _bind_methods();

public:
	Ref<Theme> get_msdf_theme() const;
	FlowScriptEditorClipboard *get_clipboard() const;
	FlowScriptEditorDialogs *get_dialogs() const;

	void edit_flow_script(Ref<FlowScript> p_flow_script);
	void init_dependencies(Ref<Theme> p_msdf_theme, FlowScriptEditorClipboard *p_clipboard, FlowScriptEditorDialogs *p_dialogs);
};


class FlowScriptEditor final : public Control
{
	GDCLASS(FlowScriptEditor, Control);

public:
	enum EditorUpdateFlag
	{
		EDITOR_UPDATE_FLAG_REDRAW_CONNECTIONS = 1 << 0,
		EDITOR_UPDATE_FLAG_REBUILD_EVERY_INCLUDE = 1 << 1,
		EDITOR_UPDATE_FLAG_REBUILD_EVERY_NODE = 1 << 2,
	};

	enum NodeUpdateFlag
	{
		NODE_UPDATE_FLAG_INSTANTIATE = 1 << 0,
		NODE_UPDATE_FLAG_DELETE = 1 << 1,
		NODE_UPDATE_FLAG_RECT = 1 << 2,
		NODE_UPDATE_FLAG_TEXT = 1 << 3,
	};

	enum PastePlacementMode
	{
		PASTE_RECT = 0,
		PASTE_SCREEN_OFFSET,
	};

public:
	struct NodeMoveOp final
	{
		FlowScriptNodeID node_id;
		Point2 from;
		Point2 to;
	};

	struct IncludeMoveOp final
	{
		FlowScriptIncludeID include_id;
		Point2 from;
		Point2 to;
	};

private:
	struct EditedInclude final
	{
		FlowScriptEditorIncludeFrame *frame;
		HashMap<FlowScriptIncludeID, FlowScriptEditorSubIncludeNode *> map_sub_include_nodes;
		HashMap<FlowScriptNodeID, FlowScriptNodeEditor *>map_node_editors;

		LocalVector<FlowScriptIncludeID> get_editor_sub_include_id_list() const;
		LocalVector<FlowScriptNodeID> get_editor_node_id_list() const;
	};

private:
	Ref<FlowScript> flow_script;
	FlowScriptEditorCommonObject *common;

	FlowScriptEditorGraph *graph;
	Button *btn_create_node;
	Button *btn_instantiate_include;

	HashMap<FlowScriptIncludeID, EditedInclude> map_include_editors; // A map of include IDs to include editor data.
	HashMap<FlowScriptNodeID, FlowScriptNodeEditor *> map_node_editors; // A map of node IDs to node editors for the root FlowScript.

	int op_active_counter = 0;
	Vector2 element_paste_offset;
	Vector2 element_duplicate_offset;

	bool editor_total_refresh_queued = false;
	bool editor_update_queued = false;

	HashSet<FlowScriptIncludeID> rf_queue_change_include_set;
	HashSet<FlowScriptIncludeID> rf_queue_remove_include_set;
	HashSet<FlowScriptNodeID> rf_queue_change_node_set;
	HashSet<FlowScriptNodeID> rf_queue_remove_node_set;
	HashSet<FlowScriptConnectionInfo, FlowScriptConnectionInfoHasher> rf_queue_connection_create_set;
	HashSet<FlowScriptConnectionInfo, FlowScriptConnectionInfoHasher> rf_queue_connection_break_set;

	bool deferred_op_processing_queued = false;
	List<IncludeMoveOp> op_queue_include_moves;
	List<NodeMoveOp> op_queue_node_moves;

	void sync_editor_settings();
	void connect_extra_flow_script_signals();
	void disconnect_extra_flow_script_signals();
	void draw_connections();
	void update_editor_node_rect(const FlowScriptNodeReference &p_target);
	void update_editor_node(const FlowScriptNodeReference &p_target);
	void reload_include(const FlowScriptIncludeID p_include_id);
	void delete_edited_include(const FlowScriptIncludeID p_include_id);
	void request_create_node_at_position(const Point2 &p_position);
	void request_instantiate_include_at_position(const Point2 &p_position);
	FlowScriptNodeEditor *create_node_editor_instance(const FlowScriptNodeReference &p_target);
	void delete_node_editor_instance(const FlowScriptNodeReference &p_target);
	bool should_element_deletion_require_confirmation(const PackedFlowScriptIncludeIDArray &p_include_id_list, const PackedFlowScriptNodeIDArray &p_node_id_list) const;

	Object *get_undo_redo_context() const;
	EditorUndoRedoManager *get_undo_redo() const;
	void create_undo_redo_action(const String &p_name, const UndoRedo::MergeMode p_mode, const bool p_backwards_undo_ops);
	void commit_undo_redo_action();

	void op_begin();
	void op_end();
	bool is_op_active() const;
	LocalVector<FlowScriptIncludeID> get_editor_include_id_list() const;
	LocalVector<FlowScriptNodeID> get_editor_node_id_list() const;
	void queue_update_editor();
	void immediate_update_editor();
	void total_refresh_editor();
	FlowScriptNodeEditor *get_node_editor_by_reference(const FlowScriptNodeReference &p_node) const;
	void get_selected_element_id_list(List<FlowScriptIncludeID> *p_include_id_list, List<FlowScriptNodeID> *p_node_id_list) const;
	void get_selected_include_id_list(List<FlowScriptIncludeID> *p_list) const;
	void get_selected_node_id_list(List<FlowScriptNodeID> *p_list) const;
	void push_node_set_to_clipboard(const HashSet<FlowScriptNodeID> &p_node_id_set, const bool p_clear, FlowScriptEditorClipboard *p_target_clipboard);
	Point2 get_element_graph_center(const GraphElement *p_element) const;
	void make_element_unclickable(GraphElement *p_element);
	void queue_process_deferred_ops();
	void immediate_process_deferred_ops();
	// void draw_node_connections_break(const FlowScriptNodeReference &p_from);
	// void draw_node_connections_create(const FlowScriptNodeReference &p_from);

	void queue_op_include_move(const IncludeMoveOp &p_op);
	void queue_op_node_move(const NodeMoveOp &p_op);

	void queue_rf_include_changed(const FlowScriptIncludeID p_include_id);
	void queue_rf_include_removed(const FlowScriptIncludeID p_include_id);
	void queue_rf_node_changed(const FlowScriptNodeID p_node_id);
	void queue_rf_node_removed(const FlowScriptNodeID p_node_id);
	void queue_rf_connection_create(const FlowScriptConnectionInfo &p_connection_info);
	void queue_rf_connection_break(const FlowScriptConnectionInfo &p_connection_info);

	void op_step_remove_element_sets_from_flow_script(const HashSet<FlowScriptIncludeID> &p_include_id_set, const HashSet<FlowScriptNodeID> &p_node_id_set);
	void op_step_paste_from_clipboard(const FlowScriptEditorClipboard *p_clipboard, const PastePlacementMode p_placement_mode, const Vector2 &p_placement_offset);

	bool hook_graph_is_node_hover_valid(const StringName &p_from, const int p_from_port, const StringName &p_to, const int p_to_port) const;
	void hook_exec_rename_node(const FlowScriptNodeID p_node_id, const StringName &p_new_name);
	void hook_delete_elements_confirmed(const PackedFlowScriptIncludeIDArray &p_include_id_list, const PackedFlowScriptNodeIDArray &p_node_id_list);
	bool hook_create_node(const FlowScriptNodeID p_node_id, const Dictionary &p_type_info_dict, const Point2 &p_position);
	void hook_create_node_and_connect(const FlowScriptNodeID p_node_id, const Dictionary &p_type_info_dict, const Point2 &p_position, const FlowScriptNodeID p_from_node_id, const FlowScriptNodeConnectionListNo p_from_list, const FlowScriptNodeConnectionListSlotNo p_from_slot);
	void hook_add_include(const FlowScriptIncludeID p_include_id, Ref<FlowScript> p_flow_script, const Point2 &p_position);

	void on_flow_script_changed();
	void on_flow_script_include_changed(const FlowScriptIncludeID p_include_id);
	void on_flow_script_include_rect_changed(const FlowScriptIncludeID p_include_id);
	void on_flow_script_include_added(const FlowScriptIncludeID p_include_id);
	void on_flow_script_removing_include(const FlowScriptIncludeID p_include_id);
	void on_flow_script_include_removed(const FlowScriptIncludeID p_include_id);
	void on_flow_script_node_changed(const FlowScriptNodeID p_node_id);
	void on_flow_script_node_renamed(const FlowScriptNodeID p_node_id);
	void on_flow_script_node_rect_changed(const FlowScriptNodeID p_node_id);
	void on_flow_script_node_connection_changed(const FlowScriptNodeID p_node_id, const FlowScriptNodeConnectionListNo p_list, const FlowScriptNodeConnectionListSlotNo p_slot, const FlowScriptIncludeID p_old_target_include_id, const FlowScriptNodeID p_old_target_node_id, const FlowScriptIncludeID p_new_target_include_id, const FlowScriptNodeID p_new_target_node_id);
	void on_flow_script_node_added(const FlowScriptNodeID p_node_id);
	void on_flow_script_removing_node(const FlowScriptNodeID p_node_id);
	void on_flow_script_node_removed(const FlowScriptNodeID p_node_id);

	void on_btn_instantiate_include_pressed();
	void on_btn_create_node_pressed();

	void on_graph_gui_input(const Ref<InputEvent> &p_event);
	void on_graph_begin_node_move();
	void on_graph_connection_drag_ended();
	void on_graph_connection_drag_started(const StringName &p_from_node, const int p_from_port, const bool p_is_output);
	void on_graph_connection_from_empty(const StringName &p_to_node, const int p_to_port, const Point2 &p_release_position);
	void on_graph_connection_request(const StringName &p_from_node, const int p_from_port, const StringName &p_to_node, const int p_to_port);
	void on_graph_connection_to_empty(const StringName &p_from_node, const int p_from_port, const Point2 &p_release_position);
	void on_graph_copy_nodes_request();
	void on_graph_cut_nodes_request();
	void on_graph_delete_nodes_request(const TypedArray<StringName> &p_nodes);
	void on_graph_disconnection_request(const StringName &p_from_node, const int p_from_port, const StringName &p_to_node, const int p_to_port);
	void on_graph_duplicate_nodes_request();
	void on_graph_end_node_move();
	void on_graph_frame_rect_changed(GraphFrame *p_frame, const Size2 &p_new_rect);
	void on_graph_graph_elements_linked_to_frame_request(const Array &p_elements, const StringName &p_frame);
	void on_graph_nodes_arranged();
	void on_graph_node_deselected(Node *p_node);
	void on_graph_node_selected(Node *p_node);
	void on_graph_paste_nodes_request();
	void on_graph_popup_request(const Point2 &p_at_position);
	void on_graph_scroll_offset_changed(const Point2 &p_offset);

	void on_node_resized(const FlowScriptNodeID p_node_id);
	void on_node_delete_request(const FlowScriptNodeID p_node_id);
	void on_node_dragged(const Point2 &p_from, const Point2 &p_to, const FlowScriptNodeID p_node_id);
	void on_node_deselected(const FlowScriptNodeID p_node_id);
	void on_node_selected(const FlowScriptNodeID p_node_id);
	void on_node_position_offset_changed(const FlowScriptNodeID p_node_id);
	void on_node_raise_request(const FlowScriptNodeID p_node_id);
	void on_node_resize_end(const Size2 &p_new_size, const FlowScriptNodeID p_node_id);
	void on_node_resize_request(const Size2 &p_new_size, const FlowScriptNodeID p_node_id);
	void on_node_rename_request(const FlowScriptNodeID p_node_id);

	void on_include_resized(const FlowScriptIncludeID p_include_id);
	void on_include_delete_request(const FlowScriptIncludeID p_include_id);
	void on_include_dragged(const Point2 &p_from, const Point2 &p_to, const FlowScriptIncludeID p_include_id);
	void on_include_deselected(const FlowScriptIncludeID p_include_id);
	void on_include_selected(const FlowScriptIncludeID p_include_id);
	void on_include_position_offset_changed(const FlowScriptIncludeID p_include_id);
	void on_include_raise_request(const FlowScriptIncludeID p_include_id);
	void on_include_resize_end(const Size2 &p_new_size, const FlowScriptIncludeID p_include_id);
	void on_include_resize_request(const Size2 &p_new_size, const FlowScriptIncludeID p_include_id);
	void on_include_edit_request(const FlowScriptIncludeID p_include_id);

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	Ref<FlowScript> get_flow_script() const;
	FlowScriptEditorGraph *get_graph() const;

	virtual Size2 get_minimum_size() const override;

	void init_dependencies(FlowScriptEditorCommonObject *p_common, Ref<FlowScript> p_flow_script);

	void set_state(const Dictionary &p_state);
	Dictionary get_state() const;

	Point2 get_screen_center() const;
#if 0
	Rect2 get_node_graph_rect(const FlowScriptNodeReference &p_node) const;
#endif // 0
	Rect2i get_node_data_rect(const FlowScriptNodeReference &p_node) const;
#if 0
	Rect2 get_include_graph_rect(const FlowScriptIncludeID p_include_id) const;
	Rect2i get_include_data_rect(const FlowScriptIncludeID p_include_id) const;
#endif // 0
	Point2 convert_point_data_to_graph(const Point2i &p_data_point) const;
	Point2 convert_point_data_to_screen(const Point2i &p_data_point) const;
	Point2i convert_point_graph_to_data(const Point2 &p_graph_point) const;
	Point2 convert_point_graph_to_screen(const Point2 &p_graph_point) const;
	Point2 convert_point_screen_to_graph(const Point2 &p_screen_point) const;
	Point2i convert_point_screen_to_data(const Point2 &p_screen_point) const;

	bool op_instantiate_include(const FlowScriptIncludeID p_include_id, const Ref<FlowScript> &p_include_flow_script, const Point2i &p_position);
	bool op_create_node(const FlowScriptNodeID p_node_id, const Ref<FlowScriptNode> &p_data, const Point2i &p_position);
	bool op_delete_include(const FlowScriptIncludeID p_include_id);
	bool op_delete_node(const FlowScriptNodeID p_node_id);
	bool op_delete_element_sets(const HashSet<FlowScriptIncludeID> &p_include_id_set, const HashSet<FlowScriptNodeID> &p_node_id_set);
	bool op_connect_node(const FlowScriptConnectionInfo &p_connection_info);
	bool op_disconnect_node(const FlowScriptConnectionInfo &p_connection_info);
	bool op_rename_node(const FlowScriptNodeID p_node_id, const StringName &p_new_name);
	bool op_move_elements(const List<IncludeMoveOp> &p_include_moves, const List<NodeMoveOp> &p_node_moves);
	bool op_cut_node_set(const HashSet<FlowScriptNodeID> &p_node_id_set);
	bool op_paste_clipboard();
	bool op_duplicate_node_set(const HashSet<FlowScriptNodeID> &p_node_id_set);
	bool op_resize_node(const FlowScriptNodeID p_node_id, const Rect2i &p_new_rect);

	FlowScriptEditor();
	~FlowScriptEditor();
};


class FlowScriptEditorPlugin final : public EditorPlugin
{
	GDCLASS(FlowScriptEditorPlugin, EditorPlugin);

private:
	enum FileOption
	{
		FILE_OPTION_NEW = 0,
		FILE_OPTION_OPEN,
		FILE_OPTION_SAVE,
		FILE_OPTION_SAVE_ALL,
		FILE_OPTION_CLOSE,
		FILE_OPTION_CLOSE_ALL,
		FILE_OPTION_MAX,
	};

	enum FileAction
	{
		FILE_ACTION_NONE = 0,
		FILE_ACTION_CREATE_SCRIPT,
		FILE_ACTION_EDIT_SCRIPT,
		FILE_ACTION_SAVE_SCRIPT,
		FILE_ACTION_MAX,
	};

	enum DragState
	{
		DRAG_STATE_NONE = 0,
		DRAG_STATE_NODE,
		DRAG_STATE_INCLUDE,
		DRAG_STATE_NODE_OUTPUT_CONNECTION,
		DRAG_STATE_NODE_INPUT_CONNECTION,
	};

	enum FlowScriptFileDialogMode
	{
		FS_FILE_MODE_NONE = 0,
		FS_FILE_MODE_CREATE_NEW,
		FS_FILE_MODE_LOAD_ROOT,
		FS_FILE_MODE_LOAD_INCLUDE,
		FS_FILE_MODE_SAVE_TO_LOCATION,
	};

private:
	struct EditedRoot final
	{
		FlowScriptEditor *editor;
		double timestamp_last_save;
		double timestamp_last_change;
	};

private:
	Ref<EditorInspectorPluginFlowScriptNode> inspector_plugin_nodes;

	Button *bottom_panel_button = nullptr; // Default to nullptr since this isn't always used.
	WindowWrapper *window_wrapper;
	ScreenSelect *make_floating_button;
	HSplitContainer *main_control;
	MenuButton *file_menu_button;
	Ref<FlowScript> flow_script_file_dialog_context_flow_script;
	TabContainer *editor_tabs;
	ItemList *flow_script_item_list;
	Timer *flow_script_edit_sync_timer;

	Ref<Theme> msdf_theme;
	FlowScriptEditorDialogs *dialogs;
	FlowScriptEditorClipboard *clipboard;
	FlowScriptEditorCommonObject *common;

	Ref<StyleBoxEmpty> empty_style;
	HashMap<Ref<FlowScript>, EditedRoot> map_edited_roots;
	Ref<FlowScript> current_selected_flow_script;
	FlowScriptFileDialogMode current_flow_script_file_dialog_mode = FS_FILE_MODE_NONE;
	bool flow_script_item_list_dirty = false;
	bool visible_tab_dirty = false;

	void submit_file_option(const int p_option);
	void update_file_menu();
	// void open_file_at_path(const String &p_path);
	bool add_flow_script_to_edited_list(const Ref<FlowScript> &p_flow_script);
	void select_flow_script(const Ref<FlowScript> &p_flow_script);
	void update_theme();
	Vector<Ref<FlowScript>> get_alpha_sorted_edited_flow_script_list() const;
	void queue_refresh_flow_script_item_list();
	void immediate_refresh_flow_script_item_list();
	void queue_update_visible_tab();
	void immediate_update_visible_tab();
	void trigger_flow_script_edit_sync_timer();
	void handle_flow_script_saved(const Ref<FlowScript> &p_flow_script);
	bool save_flow_script(Ref<FlowScript> p_flow_script);
	void save_all_flow_scripts();
	bool close_flow_script(const Ref<FlowScript> &p_flow_script, const bool p_ignore_unsaved);
	void close_all_flow_scripts(const bool p_ignore_unsaved);
	void clear_edit_history();

	void hook_pre_close_flow_script_save(Ref<FlowScript> p_flow_script);
	void hook_pre_close_flow_script_discard(Ref<FlowScript> p_flow_script);

	void on_this_resource_saved(Ref<Resource> p_resource);
	void on_this_scene_closed(const String &p_filepath);
	void on_this_scene_saved(const String &p_filepath);
	void on_filesystem_dock_resource_removed(Ref<Resource> p_resource);
	void on_common_edit_request(Ref<FlowScript> p_flow_script);
	void on_make_floating_button_request_open_in_screen(const int p_screen_id);
	void on_window_visibility_changed(const bool p_visible);
	// void on_flow_script_changed();
	void on_flow_script_item_list_item_selected(const int p_item);
	void on_flow_script_item_list_item_clicked(const int p_item, const Point2 &p_local_mouse_position, const MouseButton p_button_index);
	void on_flow_script_edit_sync_timer_timeout();
	void on_current_selected_flow_script_changed();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	virtual String get_plugin_name() const override;
	virtual const Ref<Texture2D> get_plugin_icon() const override;
	virtual String get_plugin_version() const override;
	virtual void make_visible(bool p_visible) override;
	virtual bool handles(Object *p_object) const override;
	virtual void edit(Object *p_object) override;
	virtual void set_window_layout(Ref<ConfigFile> p_layout) override;
	virtual void get_window_layout(Ref<ConfigFile> p_layout) override;
	virtual String get_unsaved_status(const String &p_for_scene = "") const override;
	virtual void save_external_data() override;

	bool is_flow_script_open(const Ref<FlowScript> &p_flow_script) const;
	bool is_flow_script_selected(const Ref<FlowScript> &p_flow_script) const;
	void edit_flow_script(const Ref<FlowScript> &p_flow_script);

	FlowScriptEditorPlugin();
	~FlowScriptEditorPlugin();
};


#endif // FLOW_SCRIPT_EDITOR_PLUGIN_HPP
