#ifndef FLOW_SCRIPT_EDITOR_PLUGIN_HPP
#define FLOW_SCRIPT_EDITOR_PLUGIN_HPP


#include "flow_script.hpp"
#include "flow_script_graph.hpp"
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
	class EditedFlowScript
	{
		Ref<FlowScript> flow_script;
		FlowScriptGraph *graph = nullptr;
		String path;
		String name;
	};

private:
	Button *bottom_panel_button = nullptr; // default to nullptr in case not in use
	WindowWrapper *window_wrapper;
	HSplitContainer *main_split;
	MenuButton *file_menu;
	ScreenSelect *make_floating_button;
	ItemList *script_list;
	Ref<Theme> graph_theme;

	void file_menu_update_clickable();
	void update_graph_theme();

	void on_file_menu_item_pressed(int p_idx);
	void on_make_floating_button_open_to_screen_request(int p_screen_id);
	void on_window_visibility_changed(bool p_visible);
	void on_script_list_item_selected(int p_idx);
	void on_script_list_item_clicked(int p_item, Point2 p_local_mouse_pos, MouseButton p_btn_idx);

public:
	virtual String get_name() const override;
	virtual const Ref<Texture2D> get_icon() const override;
	virtual String get_plugin_version() const override;
	virtual void make_visible(bool p_visible) override;
	virtual void edit(Object *p_object) override;
	virtual bool handles(Object *p_object) const override;
	virtual bool can_auto_hide() const override;

	FlowScriptEditorPlugin();
	~FlowScriptEditorPlugin();
};


#endif // FLOW_SCRIPT_EDITOR_PLUGIN_HPP
