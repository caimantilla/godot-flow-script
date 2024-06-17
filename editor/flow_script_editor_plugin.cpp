#include "flow_script_editor_plugin.hpp"
#include "../singletons/flow_config_manager.hpp"
#include "../singletons/flow_type_db.hpp"
#include "editor/editor_interface.h"
#include "editor/editor_inspector.h"
#include "scene/resources/image_texture.h"
#include "scene/scene_string_names.h"
#include "editor/editor_string_names.h"


void FlowScriptEditorPlugin::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_WM_WINDOW_FOCUS_IN:
			refresh_flow_type_db();
			break;
		
		case NOTIFICATION_ENTER_TREE:
			flow_script_editor_panel_location = FlowConfigManager::get_singleton()->get_flow_editor_gui_location();
			switch (flow_script_editor_panel_location)
			{
				case FlowEditorConstants::FLOW_EDITOR_GUI_LOCATION_BOTTOM_PANEL:
					plugin_bottom_panel_button = add_control_to_bottom_panel(flow_script_editor_panel, "FlowScript");
					break;
				case FlowEditorConstants::FLOW_EDITOR_GUI_LOCATION_MAIN_SCREEN:
				default:
					EditorInterface::get_singleton()->get_editor_main_screen()->add_child(flow_script_editor_panel, true);
					make_visible(false);
					break;
			}

			break;
		
		case NOTIFICATION_EXIT_TREE:
			break;
	}
}


String FlowScriptEditorPlugin::get_name() const
{
	return "FlowScript";
}


const Ref<Texture2D> FlowScriptEditorPlugin::get_icon() const
{
	if (!EditorInterface::get_singleton()->get_base_control()->has_theme_icon(SNAME("GraphEdit"), EditorStringNames::get_singleton()->EditorIcons))
	{
		return Ref<Texture2D>();
	}

	Ref<Texture2D> base = EditorInterface::get_singleton()->get_base_control()->get_theme_icon(SNAME("GraphEdit"), EditorStringNames::get_singleton()->EditorIcons);
	Ref<Image> img = base->get_image();
	img->adjust_bcs(1.0f, 1.0f, 0.0f); // desaturate the icon

	return ImageTexture::create_from_image(img);
}


String FlowScriptEditorPlugin::get_plugin_version() const
{
	return "1.0";
}


bool FlowScriptEditorPlugin::has_main_screen() const
{
	return flow_script_editor_panel_location == FlowEditorConstants::FLOW_EDITOR_GUI_LOCATION_MAIN_SCREEN;
}


void FlowScriptEditorPlugin::edit(Object *p_object)
{
	Ref<FlowScript> res;
	res = p_object;

	if (!res.is_valid())
		return;

	set_edited_flow_script(res);
}


bool FlowScriptEditorPlugin::handles(Object *p_object) const
{
	return Object::cast_to<FlowScript>(p_object) != nullptr;
}


void FlowScriptEditorPlugin::make_visible(bool p_visible)
{
	switch (flow_script_editor_panel_location)
	{
		case FlowEditorConstants::FLOW_EDITOR_GUI_LOCATION_BOTTOM_PANEL:
			if (p_visible)
			{
				make_bottom_panel_item_visible(flow_script_editor_panel);
			}
			break;
		case FlowEditorConstants::FLOW_EDITOR_GUI_LOCATION_MAIN_SCREEN:
		default:
			if (p_visible)
			{
				flow_script_editor_panel->show();
			}
			else
			{
				flow_script_editor_panel->hide();
			}
	}
}


void FlowScriptEditorPlugin::set_edited_flow_script(const Ref<FlowScript> &p_flow_script)
{
	if (p_flow_script == edited_flow_script)
	{
		return;
	}

	clear_flow_node_inspector();

	if (edited_flow_script.is_valid())
	{
		edited_flow_script->disconnect(SceneStringNames::get_singleton()->tree_exiting, on_flow_script_tree_exiting_callback);
		edited_flow_script->disconnect(SNAME("removing_flow_node"), on_flow_script_removing_flow_node_callback);
		// edited_flow_script->disconnect(SNAME("flow_node_id_changed"), on_flow_script_flow_node_id_changed_callback);
	}

	edited_flow_script = p_flow_script;

	if (edited_flow_script.is_valid())
	{
		edited_flow_script->connect(SNAME("removing_flow_node"), on_flow_script_removing_flow_node_callback);
		edited_flow_script->connect(SNAME("flow_node_id_changed"), on_flow_script_flow_node_id_changed_callback);
		// edited_flow_script->connect(SceneStringNames::get_singleton()->tree_exiting, on_flow_script_tree_exiting_callback);
	}

	flow_script_editor_panel->set_edited_flow_script(p_flow_script);
}


Ref<FlowScript> FlowScriptEditorPlugin::get_edited_flow_script() const
{
	return edited_flow_script;
}


void FlowScriptEditorPlugin::refresh_flow_type_db()
{
	FlowTypeDB::get_singleton()->refresh();
}


void FlowScriptEditorPlugin::inspect_flow_node(const FlowNodeID p_flow_node_id)
{
	current_inspected_flow_node_id = p_flow_node_id;
	FlowNode *node_to_inspect = nullptr;

	if (edited_flow_script.is_valid() && edited_flow_script->has_flow_node(p_flow_node_id))
	{
		node_to_inspect = edited_flow_script->get_flow_node(p_flow_node_id);
	}

	EditorInterface::get_singleton()->inspect_object(node_to_inspect, "", true); // should inspector_only be true??
}


void FlowScriptEditorPlugin::clear_flow_node_inspector()
{
	inspect_flow_node(FLOW_NODE_ID_NIL);
}


void FlowScriptEditorPlugin::on_flow_script_removing_flow_node(const FlowNodeID p_flow_node_id)
{
	if (current_inspected_flow_node_id == p_flow_node_id)
	{
		clear_flow_node_inspector();
	}
}


void FlowScriptEditorPlugin::on_flow_script_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to)
{
	if (current_inspected_flow_node_id == p_from)
	{
		inspect_flow_node(p_to);
	}
}


void FlowScriptEditorPlugin::on_flow_script_tree_exiting()
{
	set_edited_flow_script(Ref<FlowScript>());
}


void FlowScriptEditorPlugin::on_flow_script_editor_panel_flow_node_selected(const FlowNodeID p_flow_node_id)
{
	inspect_flow_node(p_flow_node_id);
}


FlowScriptEditorPlugin::FlowScriptEditorPlugin()
{
	current_inspected_flow_node_id = FLOW_NODE_ID_NIL;

	on_flow_script_removing_flow_node_callback = callable_mp(this, &FlowScriptEditorPlugin::on_flow_script_removing_flow_node);
	on_flow_script_flow_node_id_changed_callback = callable_mp(this, &FlowScriptEditorPlugin::on_flow_script_flow_node_id_changed);
	on_flow_script_tree_exiting_callback = callable_mp(this, &FlowScriptEditorPlugin::on_flow_script_tree_exiting);

	on_flow_script_editor_panel_flow_node_selected_callback = callable_mp(this, &FlowScriptEditorPlugin::on_flow_script_editor_panel_flow_node_selected);

	FlowConfigManager::get_singleton()->initialize_editor_settings();
	flow_script_editor_panel_location = FlowConfigManager::get_singleton()->get_flow_editor_gui_location();

	Ref<EditorInspectorPluginFlowNode> node_inspector_plugin;
	node_inspector_plugin.instantiate();
	add_inspector_plugin(node_inspector_plugin);

	flow_script_editor_panel = memnew(FlowScriptEditorPanel);
	flow_script_editor_panel->set_name("FlowScript");
	flow_script_editor_panel->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	flow_script_editor_panel->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	flow_script_editor_panel->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	flow_script_editor_panel->connect(SNAME("flow_node_selected"), on_flow_script_editor_panel_flow_node_selected_callback);
}


FlowScriptEditorPlugin::~FlowScriptEditorPlugin()
{
	flow_script_editor_panel->queue_free();
	flow_script_editor_panel = nullptr;
}
