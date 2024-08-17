#ifndef FLOW_SCRIPT_EDITOR_PLUGIN_HPP
#define FLOW_SCRIPT_EDITOR_PLUGIN_HPP


#include "flow_script.hpp"
#include "flow_script_graph.hpp"
#include "flow_script_node.hpp"
#include "editor/nodes/flow_script_node_editor.hpp"
#include "editor/gui/flow_script_include_editor_frame.hpp"
#include "flow_script_node_create_dialog.hpp"
#include "core/templates/paged_array.h"
#include "editor_inspector_plugin_flow_script.hpp"
#include "editor/plugins/editor_plugin.h"
#include "editor/window_wrapper.h"
#include "scene/resources/theme.h"
#include "scene/gui/split_container.h"
#include "scene/gui/box_container.h"
#include "scene/gui/menu_button.h"
#include "scene/gui/item_list.h"


class FlowScriptEditorPlugin final : public EditorPlugin
{
	GDCLASS(FlowScriptEditorPlugin, EditorPlugin);

private:
	enum
	{
		FILE_NEW,
		FILE_OPEN,
		FILE_SAVE,
		FILE_SAVE_AS,
		FILE_CLOSE,
		FILE_MAX,
	};

private:
	class EditedNode final
	{
	private:
		void on_delete_request();
		void on_dragged(Point2 p_from, Point2 p_to);
		void on_node_deselected();
		void on_node_selected();
		void on_position_offset_changed();
		void on_raise_request();
	
	public:
		FlowScriptEditorPlugin *plugin;
		FlowScriptNodeEditor *editor;

		void copy_position_to_node();

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

	class EditedScript final
	{
	private:
		void on_changed();
	
	public:
		FlowScriptEditorPlugin *plugin;
		Ref<FlowScript> flow_script;
		int index;

		bool is_selected() const;

		EditedScript(FlowScriptEditorPlugin *p_plugin, Ref<FlowScript> &p_flow_script);
	};

private:
	Ref<Theme> graph_theme;
	PagedArray<EditedScript *> open_script_list;
	int current_edited_script_idx = -1;

	Button *bottom_panel_button = nullptr; // default to nullptr in case not in use
	WindowWrapper *window_wrapper;
	HSplitContainer *main_split;
	MenuButton *file_menu;
	ScreenSelect *make_floating_button;
	ItemList *script_item_list;
	FlowScriptGraph *graph;
	FlowScriptNodeCreateDialog *node_create_dialog;

	void file_menu_update_clickable();
	void update_graph_theme();
	void update_script_item_list();

	void clear_graph();
	void redraw_graph_connections();
	void recreate_graph();

	void on_node_create_dialog_type_chosen();
	void on_file_menu_item_pressed(int p_idx);
	void on_make_floating_button_open_to_screen_request(int p_screen_id);
	void on_window_visibility_changed(bool p_visible);
	void on_script_item_list_item_selected(int p_idx);
	void on_script_item_list_item_clicked(int p_item, Point2 p_local_mouse_pos, MouseButton p_btn_idx);

public:
	virtual String get_name() const override;
	virtual const Ref<Texture2D> get_icon() const override;
	virtual String get_plugin_version() const override;
	virtual void make_visible(bool p_visible) override;
	virtual void edit(Object *p_object) override;
	virtual bool handles(Object *p_object) const override;
	virtual bool can_auto_hide() const override;

	Ref<FlowScript> get_edited_flow_script();

	Point2 graph_data_to_screen_position(const Point2i &p_data_position) const;
	Point2i graph_screen_to_data_position(const Point2 &p_screen_position) const;
	Point2 graph_rect_position_to_screen_position(const Point2 &p_rect_position) const;

	FlowScriptEditorPlugin();
	~FlowScriptEditorPlugin();
};


#endif // FLOW_SCRIPT_EDITOR_PLUGIN_HPP
