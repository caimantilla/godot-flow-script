#ifndef FLOW_SCRIPT_EDITOR_PLUGIN_HPP
#define FLOW_SCRIPT_EDITOR_PLUGIN_HPP


#include "../../flow_script.hpp"
#include "../../flow_script_node.hpp"
#include "../nodes/flow_script_node_editor.hpp"
#include "../gui/flow_script_node_create_dialog.hpp"
#include "core/templates/paged_array.h"
#include "core/io/image.h"
#include "editor/plugins/editor_plugin.h"
#include "editor/window_wrapper.h"
#include "editor/gui/editor_file_dialog.h"
#include "scene/resources/theme.h"
#include "scene/resources/texture.h"
#include "scene/resources/image_texture.h"
#include "scene/gui/graph_edit.h"
#include "scene/gui/graph_element.h"
#include "scene/gui/graph_frame.h"
#include "scene/gui/split_container.h"
#include "scene/gui/box_container.h"
#include "scene/gui/menu_button.h"
#include "scene/gui/item_list.h"
#include "scene/gui/tab_container.h"
#include "scene/gui/texture_rect.h"


class EditorInspectorPluginFlowScript;


class FlowScriptEditorPlugin final : public EditorPlugin
{
	GDCLASS(FlowScriptEditorPlugin, EditorPlugin);

	class EditedScript;

public:
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
	class NodeConnectionBreakElement final : public GraphElement
	{
		GDCLASS(NodeConnectionBreakElement, GraphElement);

	private:
		TextureRect *break_rect;

	protected:
		void _notification(int p_what);

	public:
		NodeConnectionBreakElement();
	};

	class IncludeFrame final : public GraphFrame
	{
		GDCLASS(IncludeFrame, GraphFrame);

	private:
		void on_edit_button_pressed();
		void on_delete_button_pressed();

	protected:
		void _notification(int p_what);

	public:
		EditedScript *edited_script;
		Button *edit_button;
		Button *delete_button;

		IncludeFrame(EditedScript *p_edited_script);
	};

	class ScriptGraph final : public GraphEdit
	{
		GDCLASS(ScriptGraph, GraphEdit);

	private:
		Ref<Theme> msdf_theme;

	protected:
		void _notification(int p_what);

	public:
		FlowScriptEditorPlugin *plugin;

		void add_node_editor(FlowScriptNodeEditor *p_editor);
		void add_include_frame(IncludeFrame *p_frame);
		void attach_node_editor_to_include_frame(FlowScriptNodeEditor *p_editor, IncludeFrame *p_frame);
		Point2 point_convert_data_to_graph(const Point2i &p_data_position) const;
		Point2i point_convert_graph_to_data(const Point2 &p_screen_position) const;
		// Returns the position of a GUI point to a general point (eg. the position of a mouse click)
		Point2 point_convert_rect_to_graph(const Point2 &p_rect_position) const;
		Point2 point_convert_graph_to_rect(const Point2 &p_graph_position) const;

		ScriptGraph(FlowScriptEditorPlugin *p_plugin);
	};

	class OpenScriptMetadata final
	{
	public:
		int index;
		String visible_name;
		Ref<FlowScript> flow_script;
		EditedScript *edited_script;
	};

	// An edited script instance.
	// Contains the parent script (if included) and all the child scripts.
	// Any level of nesting is supported.
	class EditedScript final
	{
	private:
		struct GraphItem final
		{
			enum Type
			{
				TYPE_NULL = 0,
				TYPE_NODE = 1,
				TYPE_INCLUDE = 2,
			};
			Type type = TYPE_NULL;
			union
			{
				FlowScriptNodeReference node;
				FlowScriptIncludeID include_id;
			};

			const bool is_null() const { return type == TYPE_NULL; }

			static GraphItem create_node(const FlowScriptNodeID p_node_id);
			static GraphItem create_node(const FlowScriptNodeReference p_node_reference);
			static GraphItem create_include(const FlowScriptIncludeID p_include_id);

			GraphItem() {}
			~GraphItem() {}
		};

		// used to track editor connections in a hashmap
		struct EditorNodeConnectionKey final
		{
			FlowScriptNodeID from_node_id;
			FlowScriptNodeOutputConnection from_node_output;

			EditorNodeConnectionKey(const FlowScriptNodeID p_node_id, const FlowScriptNodeOutputConnection p_output)
			{
				from_node_id = p_node_id;
				from_node_output = p_output;
			}
		};
		struct EditorNodeConnectionData final
		{
			FlowScriptNodeReference to_node_reference;
			Point2 connection_break_point; // scanned during mouse motion input
		};

		// operation structs
		struct MutateOperation final
		{
			// possible types
			enum Type
			{
				TYPE_NULL,
				TYPE_ITEM_DELETE,
				TYPE_ITEM_DRAG,
				TYPE_NODE_ADD,
				TYPE_NODE_RENAME,
				TYPE_NODE_CONNECT,
				TYPE_NODE_DISCONNECT,
				TYPE_NODE_PASTE,
				TYPE_NODE_DUPLICATE,
				TYPE_INCLUDE_ADD,
			};

			// per-type data
			struct ItemDelete final
			{
				GraphItem item;
			};
			struct ItemDrag final
			{
				GraphItem item;
				Point2 from;
				Point2 to;
			};
			struct NodeAdd final
			{
				Ref<FlowScriptNode> node;
				Point2 position;
			};
			struct NodeDelete final
			{
				FlowScriptNodeID node_id;
			};
			struct NodeRename final
			{
				FlowScriptNodeID node_id;
				String new_name;
			};
			struct NodeConnect final
			{
				FlowScriptNodeID from_node_id;
				FlowScriptNodeOutputConnection from_node_output;
				FlowScriptNodeReference to_node;
			};
			struct NodeDisconnect final
			{
				FlowScriptNodeID from_node_id;
				FlowScriptNodeOutputConnection from_node_output;
			};
			struct NodePaste final
			{
				Ref<FlowScriptNode> node;
				Point2 offset;
			};
			struct NodeDuplicate final
			{
				FlowScriptNodeID node_id;
			};
			struct IncludeAdd final
			{
				Ref<FlowScript> flow_script;
				Point2 position;
			};

			// root struct data
			Type type;
			union Data
			{
				ItemDelete item_delete;
				ItemDrag item_drag;
				NodeAdd node_add;
				NodeDelete node_delete;
				NodeRename node_rename;
				NodeConnect node_connect;
				NodeDisconnect node_disconnect;
				IncludeAdd include_add;

				Data() {}
				~Data() {}
			} data;
		};

		struct ReflectOperation final
		{
			// possible types
			enum Type
			{
				TYPE_NULL,
				TYPE_ITEM_ADD,
				TYPE_ITEM_DELETE,
				TYPE_ITEM_SYNC,
				TYPE_NODE_CONNECT,
				TYPE_NODE_DISCONNECT,
			};

			// structs with parameters for each operation type
			struct ItemAdd final
			{
				GraphItem item;
			};
			struct ItemDelete final
			{
				GraphItem item;
			};
			struct ItemSync final
			{
				GraphItem item;
			};
			struct NodeConnect final
			{
				FlowScriptNodeID node_id;
				FlowScriptNodeOutputConnection output;
			};
			struct NodeDisconnect final
			{
				FlowScriptNodeID node_id;
				FlowScriptNodeOutputConnection output;
			};

			// actual struct data
			Type type;
			union Data
			{
				ItemAdd item_add;
				ItemDelete item_delete;
				ItemSync item_sync;
				NodeConnect node_connect;
				NodeDisconnect node_disconnect;

				Data() {}
				~Data() {}
			} data;
		};

	private:
		HashSet<GraphItem> selected_item_set;
		HashMap<EditorNodeConnectionKey, EditorNodeConnectionData> editor_node_connection_map;

		bool buffers_dirty = false; // only relevant to the root script
		PagedArray<MutateOperation> buffer_mutate;
		PagedArray<ReflectOperation> buffer_reflect;

		void copy_selected_nodes();
		void connect_graph(); // invoke during construction of root
		void connect_flow_script(); // invoke during construction of both root and includes
		const GraphItem get_graph_node_as_item_by_ptr(Node *node) const;
		const GraphItem get_graph_node_as_item_by_name(const StringName &p_name) const;
		Point2i get_graph_item_data_position(const GraphItem &p_item) const;
		void graph_item_copy_position_to_data(const GraphItem &p_item);

		// op buffer stufff
		void queue_mutate_operation(const MutateOperation &p_operation);
		void queue_reflect_operation(const ReflectOperation &p_operation);
		void queue_handle_buffers(); // queues buffer execution (bubble up to root)
		static void root_make_buffer_clean_recursive(EditedScript *p_current_level);
		static void root_handle_mutate_buffer_recursive(EditedScript *p_current_level);
		static void root_handle_reflect_buffer_early_recursive(EditedScript *p_current_level);
		static void root_handle_reflect_buffer_late_recursive(EditedScript *p_current_level);
		void root_init_buffer_handle();
		void handle_buffers();
		void handle_mutate_buffer();
		void handle_reflect_buffer_early();
		void handle_reflect_buffer_late();

		// op buffer list handlers
		void handle_mutate_operation_list_item_delete(const Vector<MutateOperation::ItemDelete> &p_deletes);
		void handle_mutate_operation_list_item_drag(const Vector<MutateOperation::ItemDrag> &p_drags);
		void handle_mutate_operation_list_node_add(const Vector<MutateOperation::NodeAdd> &p_adds);
		void handle_mutate_operation_list_node_rename(const Vector<MutateOperation::NodeRename> &p_renames);
		void handle_mutate_operation_list_node_connect(const Vector<MutateOperation::NodeConnect> &p_connections);
		void handle_mutate_operation_list_node_disconnect(const Vector<MutateOperation::NodeDisconnect> &p_disconnections);
		void handle_mutate_operation_list_node_paste(const Vector<MutateOperation::NodePaste> &p_pastes);
		void handle_mutate_operation_list_node_duplicate(const Vector<MutateOperation::NodeDuplicate> &p_duplicates);
		void handle_mutate_operation_list_include_add(const Vector<MutateOperation::IncludeAdd> &p_adds);

		void reflect_item_add(const ReflectOperation::ItemAdd &op);
		void reflect_item_delete(const ReflectOperation::ItemDelete &op);
		void reflect_item_sync(const ReflectOperation::ItemSync &op);
		void reflect_node_connect(const ReflectOperation::NodeConnect &op);
		void reflect_node_disconnect(const ReflectOperation::NodeDisconnect &op);

		void handle_reflect_operation_list_item_add(const Vector<ReflectOperation::ItemAdd> &p_adds);
		void handle_reflect_operation_list_item_delete(const Vector<ReflectOperation::ItemDelete> &p_deletes);
		void handle_reflect_operation_list_item_sync(const Vector<ReflectOperation::ItemSync> &p_syncs);
		void handle_reflect_operation_list_node_connect(const Vector<ReflectOperation::NodeConnect> &p_connections);

		// FlowScript Hooks
		void on_script_changed();
		void on_script_node_added(const FlowScriptNodeID p_node_id);
		void on_script_node_removed(const FlowScriptNodeID p_node_id);
		void on_script_node_position_changed(const FlowScriptNodeID p_node_id);
		void on_script_node_connection_changed(const FlowScriptNodeID p_node_id, const uint8_t p_list, const int64_t p_slot);

		void on_script_include_added(const FlowScriptIncludeID p_include_id);
		void on_script_include_removed(const FlowScriptIncludeID p_include_id);
		void on_script_include_position_changed(const FlowScriptIncludeID p_include_id);

		// FlowScriptNodeEditor Hooks
		void on_node_changed(const FlowScriptNodeID p_node_id); // not actually hooked to the editor, but to the node data
		void on_node_resized(const FlowScriptNodeID p_node_id);
		void on_node_dragged(const Point2 &p_from, const Point2 &p_to, const FlowScriptNodeID p_node_id);
		void on_node_selected(const FlowScriptNodeID p_node_id);
		void on_node_deselected(const FlowScriptNodeID p_node_id);
		void on_node_raise_request(const FlowScriptNodeID p_node_id);
		void on_node_delete_request(const FlowScriptNodeID p_node_id);

		// IncludeFrame Hooks
		void on_include_changed(const FlowScriptIncludeID p_include_id);
		void on_include_resized(const FlowScriptIncludeID p_include_id);
		void on_include_dragged(const Point2 &p_from, const Point2 &p_to, const FlowScriptIncludeID p_include_id);
		void on_include_selected(const FlowScriptIncludeID p_include_id);
		void on_include_deselected(const FlowScriptIncludeID p_include_id);
		void on_include_delete_request(const FlowScriptIncludeID p_include_id);

		// ScriptGraph Hooks
		void on_graph_visibility_changed();
		void on_graph_connection_drag_ended();
		void on_graph_connection_drag_started(const StringName &p_from_node_name, const int p_from_port, const bool p_is_output);
		void on_graph_connection_from_empty(const StringName &p_to_node_name, const int p_to_port, const Point2 &p_release_position);
		void on_graph_connection_request(const StringName &p_from_node_name, const int p_from_port, const StringName &p_to_node_name, const int p_to_port);
		void on_graph_connection_to_empty(const StringName &p_from_node, const int p_from_port, const Point2 &p_release_position);
		void on_graph_delete_nodes_request(TypedArray<StringName> &p_item_names);
		void on_graph_paste_nodes_request();
		void on_graph_duplicate_nodes_request();
		void on_graph_disconnection_request(const StringName &p_from_node_name, const int p_from_port, const StringName &p_to_node_name, const int p_to_port);
		void on_graph_frame_rect_changed(GraphFrame *p_frame, const Size2 &p_new_rect);
		void on_graph_node_deselected(Node *p_node);
		void on_graph_node_selected(Node *p_node);
		void on_graph_popup_request(const Point2 &p_at_position);
		void on_graph_scroll_offset_changed(const Point2 &p_offset);

	public:
		FlowScriptEditorPlugin *plugin; // plugin reference needed for stuff
		ScriptGraph *graph; // THE GRAPH!!
		Ref<FlowScript> flow_script; // the flowscript being edited
		FlowScriptIncludeID include_id = FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID; // include id of this script if it's not the root
		EditedScript *parent = nullptr; // superscript. this is null for the root, and used to denote include scripts
		EditedScript *children[FlowScript::INCLUDE_FLOW_SCRIPT_MAX]; // Flat array of pointers to child scripts!!
		IncludeFrame *frame = nullptr; // Only used for instances with parents.
		HashMap<FlowScriptNodeID, FlowScriptNodeEditor *> node_editor_map; // Maps node IDs to node editor instances, same as in the flowscript itself.

		FlowScriptNodeEditor *get_node_editor_by_reference(const FlowScriptNodeReference &p_reference) const;
		// returns true if there's no parent.
		// this is the script that's actually editable
		bool is_root() const;
		// returns true if there's a parent but the parent has no parent
		// forming a connection from one node to another should only be possible to the next level deeper.
		bool is_first_level_include() const;
		EditedScript *get_root();

		static EditedScript *create_root_edited_script(FlowScriptEditorPlugin *p_plugin, const Ref<FlowScript> &p_flow_script, ScriptGraph *p_graph);
		static EditedScript *create_include_edited_script(FlowScriptEditorPlugin *p_plugin, EditedScript *p_parent, const FlowScriptIncludeID p_include_id);

		EditedScript();
		~EditedScript();
	};

	class ClipboardHandler final
	{
	private:
		Vector<FlowScriptNodeInstance> current_copied_nodes;

	public:
		FlowScriptEditorPlugin *plugin;

		void copy_nodes(const EditedScript *p_edited_script, const List<const FlowScriptNodeEditor *> p_node_editors);

		void copy_selected_nodes();
		void cut_selected_nodes();
		void duplicate_selected_nodes();
		void paste_nodes();
	};

private:
	Ref<EditorInspectorPluginFlowScript> inspector_plugin;
	Vector<EditedScript *> open_script_list;
	int current_edited_script_idx = -1;
	// The point that the next node should be created at
	// Basically, adjust this as needed before displaying the node creation dialog
	Point2 next_node_create_point;

	Button *bottom_panel_button = nullptr; // default to nullptr in case not in use
	WindowWrapper *window_wrapper;
	HSplitContainer *main_split;
	MenuButton *file_menu;
	ScreenSelect *make_floating_button;
	ItemList *script_item_list;
	TabContainer *graph_tab_container;
	FlowScriptNodeCreateDialog *node_create_dialog;
	Button *create_node_prompt_button;
	Button *script_include_manager_popup_button;
	ClipboardHandler clipboard_handler;
	EditorFileDialog *file_dialog;

	void close_edited_script();
	void close_all_scripts();
	void file_menu_update_clickable();
	void update_script_item_list();
	void copy_selected_nodes_to_clipboard();
	void cut_selected_nodes_to_clipboard();
	void paste_nodes_from_clipboard();

	void clear_graph();
	void refresh_connection_hover_break_spots();
	void redraw_graph_connections();
	void recreate_graph();
	void reset_graph_input_state();
	bool is_editing_script() const;
	EditedScript *get_edited_script_at(const int p_idx) const;
	EditedScript *get_current_edited_script() const;

	void on_node_create_dialog_type_chosen();
	void on_file_menu_item_pressed(int p_idx);
	void on_make_floating_button_open_to_screen_request(int p_screen_id);
	void on_window_visibility_changed(bool p_visible);
	void on_script_item_list_item_selected(int p_idx);
	void on_script_item_list_item_clicked(int p_item, Point2 p_local_mouse_pos, MouseButton p_btn_idx);
	void on_node_create_dialog_type_chosen(const StringName &p_native_class, const StringName &p_script_class);

public:
	FileOption current_file_option = FILE_OPEN;

	virtual String get_name() const override;
	virtual const Ref<Texture2D> get_icon() const override;
	virtual String get_plugin_version() const override;
	virtual void make_visible(bool p_visible) override;
	virtual void edit(Object *p_object) override;
	virtual bool handles(Object *p_object) const override;

	void prompt_script_file_create_new();
	void prompt_script_file_load_existing();
	void prompt_script_file_save_as();
	void immediate_script_file_save();

	void edit_flow_script_if_not_open(FlowScript *p_script);
	Ref<FlowScript> get_edited_flow_script();

	FlowScriptEditorPlugin();
	~FlowScriptEditorPlugin();
};


#endif // FLOW_SCRIPT_EDITOR_PLUGIN_HPP
