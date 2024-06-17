#ifndef FLOW_SCRIPT_EDITOR_PLUGIN_HPP
#define FLOW_SCRIPT_EDITOR_PLUGIN_HPP

#include "editor/plugins/editor_plugin.h"
#include "flow_editor_constants.hpp"
#include "editor_inspector_plugin_flow_node.hpp"
#include "flow_script_editor_panel.hpp"


class FlowScriptEditorPlugin final : public EditorPlugin
{
	GDCLASS(FlowScriptEditorPlugin, EditorPlugin);

private:
	FlowNodeID current_inspected_flow_node_id;
	Ref<FlowScript> edited_flow_script;
	// Store the editor location so that it can be used even if the user changes, since a restart is required for the location change to apply.
	FlowEditorConstants::FlowGUIEditorLocation flow_script_editor_panel_location;
	FlowScriptEditorPanel *flow_script_editor_panel;
	Button *plugin_bottom_panel_button = nullptr;

	void refresh_flow_type_db();
	void inspect_flow_node(const FlowNodeID p_flow_node_id);
	void clear_flow_node_inspector();

	void on_flow_script_removing_flow_node(const FlowNodeID p_flow_node_id);
	void on_flow_script_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to_id);
	void on_flow_script_tree_exiting();

	Callable on_flow_script_removing_flow_node_callback;
	Callable on_flow_script_flow_node_id_changed_callback;
	Callable on_flow_script_tree_exiting_callback;

	void on_flow_script_editor_panel_flow_node_selected(const FlowNodeID p_flow_node_id);

	Callable on_flow_script_editor_panel_flow_node_selected_callback;

protected:
	void _notification(int p_what);

public:
	virtual String get_name() const override;
	virtual const Ref<Texture2D> get_icon() const override;
	virtual String get_plugin_version() const override;
	bool has_main_screen() const override;
	virtual void edit(Object *p_object) override;
	virtual bool handles(Object *p_object) const override;
	virtual void make_visible(bool p_visible) override;

	void set_edited_flow_script(const Ref<FlowScript> &p_flow_script);
	Ref<FlowScript> get_edited_flow_script() const;

	FlowScriptEditorPlugin();
	~FlowScriptEditorPlugin();
};


#endif
