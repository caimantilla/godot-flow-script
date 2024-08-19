#ifndef FLOW_SCRIPT_EDITOR_PLUGIN_HPP
#define FLOW_SCRIPT_EDITOR_PLUGIN_HPP


#include "flow_script.hpp"
#include "flow_script_node.hpp"
#include "core/io/image.h"
#include "editor/nodes/flow_script_node_editor.hpp"
#include "editor/gui/flow_script_include_editor_frame.hpp"
#include "editor/gui/flow_script_node_create_dialog.hpp"
#include "editor/plugins/editor_plugin.h"
#include "editor/window_wrapper.h"
#include "scene/resources/theme.h"
#include "scene/resources/texture.h"
#include "scene/resources/image_texture.h"
#include "scene/gui/graph_edit.h"
#include "scene/gui/graph_element.h"
#include "scene/gui/split_container.h"
#include "scene/gui/box_container.h"
#include "scene/gui/menu_button.h"
#include "scene/gui/item_list.h"


class FlowScriptEditorPlugin final : public EditorPlugin
{
	GDCLASS(FlowScriptEditorPlugin, EditorPlugin);

private:
	enum FileOption
	{
		FILE_NEW,
		FILE_OPEN,
		FILE_SAVE,
		FILE_SAVE_AS,
		FILE_CLOSE,
		FILE_CLOSE_ALL,
		FILE_MAX,
	};

	enum DragState
	{
		DRAG_NONE = 0,
		DRAG_NODE_EDITOR = 1,
		DRAG_INCLUDE_SCRIPT_FRAME = 2,
		DRAG_NODE_OUTPUT_CONNECTION = 3,
		DRAG_NODE_INPUT_CONNECTION = 4,
	};

private:
	class EditedNode final
	{
	private:
		uint8_t edit_block_counter = 0;

		void on_resized();
		void on_delete_request();
		void on_dragged(Point2 p_from, Point2 p_to);
		void on_node_deselected();
		void on_node_selected();
		void on_position_offset_changed();
		void on_raise_request();

		void inspect_edited_node();
		void nullify_current_node_inspection();
		bool is_edit_permitted() const;

	public:
		FlowScriptEditorPlugin *plugin;
		FlowScriptNodeEditor *editor;

		void copy_position_to_node();
		void match_position_of_node();

		EditedNode(FlowScriptEditorPlugin *p_plugin, FlowScriptNodeEditor *p_editor);
	};
	
	class EditedIncludeScript final
	{
	private:
		void on_dragged(Point2 p_from, Point2 p_to);

	public:
		FlowScriptEditorPlugin *plugin;
		FlowScript *parent_script;
		FlowScriptIncludeID include_script_id;
		FlowScriptIncludeEditorFrame *frame;

		EditedIncludeScript(FlowScriptEditorPlugin *p_plugin, FlowScriptIncludeID p_include_id, FlowScriptIncludeEditorFrame *p_frame);
	};

	class OpenScriptMetadata final
	{
	};

	// An edited script instance.
	// Contains the parent script (if included) and all the child scripts.
	// Any level of nesting is supported.
	class EditedScript final
	{
	private:
		void on_changed();
		void on_frame_dragged();

	public:
		FlowScriptEditorPlugin *plugin; // plugin reference needed for stuff
		Ref<FlowScript> flow_script; // the flowscript being edited
		EditedScript *parent = nullptr; // superscript. this is null for the root, and used to denote include scripts
		EditedScript *children[FlowScript::INCLUDE_FLOW_SCRIPT_MAX]; // Flat array of pointers to child scripts!!
		GraphFrame *frame = nullptr; // Only used for instances with parents.
		HashMap<FlowScriptNodeID, EditedNode *> node_editor_map; // Maps node ID to edited node instance, same as in the flowscript itself.

		EditedNode *get_edited_node_by_reference(const FlowScriptNodeReference &p_reference) const;
		// returns true if there's no parent.
		// this is the script that's actually editable
		bool is_root() const;
		// returns true if there's a parent but the parent has no parent
		// forming a connection from one node to another should only be possible to the next level deeper.
		bool is_first_level_include() const;

		EditedScript(EditedScript *p_parent, FlowScriptEditorPlugin *p_plugin, const Ref<FlowScript> &p_flow_script);
		~EditedScript();
	};

	class GraphHoverConnectionBreakSpot final
	{
	public:
		bool enabled = false;
		Point2 position;
		FlowScriptNodeID node_id;
		FlowScriptNodeOutputConnection connection;
	};

	class GraphHoverConnectionBreakElement final : public GraphElement
	{
		GDCLASS(GraphHoverConnectionBreakElement, GraphElement);

	protected:
		void _notification(int p_what);

	public:
		GraphHoverConnectionBreakElement();
	};

	// Copy of a node stored for undo/redo
	class UndoRedoCacheNode final
	{
	public:
		FlowScriptNodeID node_id;
		FlowScriptNodeInstance node_instance_data;
	};

	class ScriptItemList final : public ItemList
	{
		GDCLASS(ScriptItemList, ItemList);

	private:
		void on_item_selected(int p_idx);
		void on_item_clicked(int p_idx, const Point2 &p_position, int p_button);
		void on_item_activated(int p_idx);

	public:
		FlowScriptEditorPlugin *plugin;

		ScriptItemList(FlowScriptEditorPlugin *p_plugin);
	};

	class ScriptGraph final : public GraphEdit
	{
		GDCLASS(ScriptGraph, GraphEdit);

	private:
		Ref<Theme> msdf_theme;
		Vector<EditedNode *> selected_nodes;

		void on_begin_node_move();
		void on_connection_drag_ended();
		void on_connection_drag_started(const StringName &p_from_node_name, const int p_from_port, const bool p_is_output);
		void on_connection_from_empty(const StringName &p_to_node_name, const int p_to_port, const Point2 &p_release_position);
		void on_connection_request(const StringName &p_from_node_name, const int p_from_port, const StringName &p_to_node_name, const int p_to_port);
		void on_connection_to_empty(const StringName &p_from_node, const int p_from_port, const Point2 &p_release_position);
		void on_copy_nodes_request();
		void on_delete_nodes_request(const TypedArray<StringName> &p_node_names);
		void on_disconnection_request(const StringName &p_from_node_name, const int p_from_port, const StringName &p_to_node_name, const int p_to_port);
		void on_duplicate_nodes_request();
		void on_end_node_move();
		void on_frame_rect_changed(GraphFrame *p_frame, const Size2 &p_new_rect);
		void on_node_deselected(Node *p_node);
		void on_node_selected(Node *p_node);
		void on_paste_nodes_request();
		void on_popup_request(const Point2 &p_at_position);
		void on_scroll_offset_changed(const Point2 &p_offset);

	protected:
		void _notification(int p_what);
	
	public:
		FlowScriptEditorPlugin *plugin;

		void add_node_editor(FlowScriptNodeEditor *p_editor);
		Point2 point_convert_data_to_graph(const Point2i &p_data_position) const;
		Point2i point_convert_graph_to_data(const Point2 &p_screen_position) const;
		// Returns the position of a GUI point to a general point (eg. the position of a mouse click)
		Point2 point_convert_rect_to_graph(const Point2 &p_rect_position) const;
		Point2 point_convert_graph_to_rect(const Point2 &p_graph_position) const;
		const Vector<EditedNode *> &get_selected_nodes() const;

		ScriptGraph(FlowScriptEditorPlugin *p_plugin);
	};

	class ClipboardHandler final
	{
	private:
		class NodeCopy final
		{
		public:
			// The offset from the current center of the graph rect should be saved rather than the graph position itself
			// So, if nodes are copied, they should carry the same layout that they were in, just relative to the current graph scroll offset
			Point2 editor_screen_offset;
			// A duplicated copy of the node resource
			// Though it's not too important, I think that the resource name should be cleared when duplicating!! do not forget this!
			Ref<FlowScriptNode> node_data;
		};

	private:
		Vector<NodeCopy> current_copied_nodes;

		NodeCopy create_copy_of_node(FlowScriptNodeID p_node_id);

	public:
		FlowScriptEditorPlugin *plugin;

		void copy_selected_nodes();
		void cut_selected_nodes();
		void duplicate_selected_nodes();
		void paste_nodes();

		void init(FlowScriptEditorPlugin *p_plugin);
	};

private:
	Ref<Texture2D> connection_hover_break_icon_texture;
	Vector<EditedScript *> open_script_list;
	int current_edited_script_idx = -1;
	// Let those port reach out for life!
	DragState current_drag_state = DRAG_NONE;
	// The point that the next node should be created at
	// Basically, adjust this as needed before displaying the node creation dialog
	Point2 next_node_create_point;
	Vector<GraphHoverConnectionBreakSpot> connection_hover_break_spot_list;

	Button *bottom_panel_button = nullptr; // default to nullptr in case not in use
	WindowWrapper *window_wrapper;
	HSplitContainer *main_split;
	MenuButton *file_menu;
	ScreenSelect *make_floating_button;
	ScriptItemList *script_item_list;
	ScriptGraph *graph;
	FlowScriptNodeCreateDialog *node_create_dialog;
	Button *create_node_prompt_button;
	Button *script_include_manager_popup_button;
	ClipboardHandler clipboard_handler;

	void close_edited_script();
	void close_all_scripts();
	void file_menu_update_clickable();
	void update_script_item_list();
	void set_drag_state(DragState p_state);
	void copy_selected_nodes_to_clipboard();
	void cut_selected_nodes_to_clipboard();
	void paste_nodes_from_clipboard();

	void clear_graph();
	void refresh_connection_hover_break_spots();
	void redraw_graph_connections();
	void recreate_graph();
	void reset_graph_input_state();
	bool is_editing_script() const;
	EditedScript &get_edited_script_at(const int p_idx) const;
	EditedScript &get_current_edited_script() const;

	void on_node_create_dialog_type_chosen();
	void on_file_menu_item_pressed(int p_idx);
	void on_make_floating_button_open_to_screen_request(int p_screen_id);
	void on_window_visibility_changed(bool p_visible);
	void on_script_item_list_item_selected(int p_idx);
	void on_script_item_list_item_clicked(int p_item, Point2 p_local_mouse_pos, MouseButton p_btn_idx);
	void on_node_create_dialog_type_chosen(const StringName &p_native_class, const StringName &p_script_class);

public:
	virtual String get_name() const override;
	virtual const Ref<Texture2D> get_icon() const override;
	virtual String get_plugin_version() const override;
	virtual void make_visible(bool p_visible) override;
	virtual void edit(Object *p_object) override;
	virtual bool handles(Object *p_object) const override;
	virtual bool can_auto_hide() const override;

	Ref<FlowScript> get_edited_flow_script();

	FlowScriptEditorPlugin();
	~FlowScriptEditorPlugin();
};


#endif // FLOW_SCRIPT_EDITOR_PLUGIN_HPP
