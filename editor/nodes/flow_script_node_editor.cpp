#include "flow_script_node_editor.hpp"
#include "../plugins/flow_script_editor_plugin.hpp"
#include "editor/editor_string_names.h"
#include "scene/gui/label.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"


void FlowScriptNodeEditor::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("is_edited"), &FlowScriptNodeEditor::is_edited);
	ClassDB::bind_method(D_METHOD("is_include"), &FlowScriptNodeEditor::is_include);
	ClassDB::bind_method(D_METHOD("get_root_flow_script"), &FlowScriptNodeEditor::get_root_flow_script);
	ClassDB::bind_method(D_METHOD("get_owner_flow_script"), &FlowScriptNodeEditor::get_owner_flow_script);
	ClassDB::bind_method(D_METHOD("get_edited_include_id"), &FlowScriptNodeEditor::get_edited_include_id);
	ClassDB::bind_method(D_METHOD("get_edited_node_id"), &FlowScriptNodeEditor::get_edited_node_id);
	ClassDB::bind_method(D_METHOD("get_edited_node"), &FlowScriptNodeEditor::get_edited_node);

	// I'm not sure if these should be accessible as properties or not...
#if 0
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "flow_script_editor", PROPERTY_HINT_NODE_TYPE, "FlowScriptEditor", PROPERTY_USAGE_NONE, "FlowScriptEditor"), "", "get_flow_script_editor");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "edited_include_id", PROPERTY_HINT_NONE, String(), PROPERTY_USAGE_NONE), "", "get_edited_include_id");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "edited_node_id", PROPERTY_HINT_NONE, String(), PROPERTY_USAGE_NONE), "", "get_edited_node_id");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "edited_node", PROPERTY_HINT_RESOURCE_TYPE, "FlowScriptNode", PROPERTY_USAGE_NONE, "FlowScriptNode"), "", "get_edited_node");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "edited_flow_script", PROPERTY_HINT_RESOURCE_TYPE, "FlowScript", PROPERTY_USAGE_NONE, "FlowScript"), "", "get_edited_flow_script");
#endif // 0
	
	GDVIRTUAL_BIND(_startup);
	GDVIRTUAL_BIND(_cleanup);
	GDVIRTUAL_BIND(_sync);
	GDVIRTUAL_BIND(_get_input_slot);
	GDVIRTUAL_BIND(_output_graph_slot_to_connection, "graph_slot");
	GDVIRTUAL_BIND(_output_connection_to_graph_slot, "list", "slot");

	ADD_SIGNAL(MethodInfo("rename_request"));
	// NOTE: The delete_request signal is already a part of GraphElement, so don't add it.
}


bool FlowScriptNodeEditor::is_edited() const
{
	return edited_node.is_valid();
}


bool FlowScriptNodeEditor::is_include() const
{
	return edited_node_reference.include_id != FlowScriptConstants::INCLUDE_ID_INVALID;
}


bool FlowScriptNodeEditor::is_placeholder() const
{
	return false;
}


void FlowScriptNodeEditor::startup()
{
	GDVIRTUAL_CALL(_startup);
}


void FlowScriptNodeEditor::cleanup()
{
	GDVIRTUAL_CALL(_cleanup);
}


void FlowScriptNodeEditor::sync()
{
	GDVIRTUAL_CALL(_sync);
}


int FlowScriptNodeEditor::get_input_slot() const
{
	int ret = -1;
	GDVIRTUAL_CALL(_get_input_slot, ret);
	return ret;
}


FlowScriptNodeOutputConnection FlowScriptNodeEditor::output_graph_slot_to_connection(const int p_graph_slot) const
{
	Dictionary virtual_ret;
	GDVIRTUAL_CALL(_output_graph_slot_to_connection, p_graph_slot, virtual_ret);
	FlowScriptNodeOutputConnection ret = FlowScriptNodeOutputConnection::create_from_dictionary(virtual_ret);
	return ret;
}


int FlowScriptNodeEditor::output_connection_to_graph_slot(const FlowScriptNodeOutputConnection &p_connection) const
{
	int ret = -1;
	GDVIRTUAL_CALL(_output_connection_to_graph_slot, p_connection.list, p_connection.slot, ret);
	return ret;
}


void FlowScriptNodeEditor::on_rename_button_pressed()
{
	emit_signal(SNAME("rename_request"));
}


void FlowScriptNodeEditor::on_delete_button_pressed()
{
	emit_signal(SNAME("delete_request"));
}


Button *FlowScriptNodeEditor::get_rename_button() const
{
	return rename_button;
}


Button *FlowScriptNodeEditor::get_delete_button() const
{
	return delete_button;
}


FlowScriptNodeReference FlowScriptNodeEditor::get_edited_node_reference() const
{
	return edited_node_reference;
}


FlowScriptIncludeID FlowScriptNodeEditor::get_edited_include_id() const
{
	return edited_node_reference.include_id;
}


FlowScriptNodeID FlowScriptNodeEditor::get_edited_node_id() const
{
	return edited_node_reference.node_id;
}


Ref<FlowScriptNode> FlowScriptNodeEditor::get_edited_node() const
{
	return edited_node;
}


FlowScriptNodeTypeInfo FlowScriptNodeEditor::get_type_info() const
{
	return type_info;
}


Ref<FlowScript> FlowScriptNodeEditor::get_root_flow_script() const
{
	return root_flow_script;
}


Ref<FlowScript> FlowScriptNodeEditor::get_owner_flow_script() const
{
	return owner_flow_script;
}


void FlowScriptNodeEditor::set_show_rename_button(const bool p_visible)
{
	rename_button->set_visible(p_visible);
}


void FlowScriptNodeEditor::set_show_delete_button(const bool p_visible)
{
	delete_button->set_visible(p_visible);
}


void FlowScriptNodeEditor::init_dependencies(const FlowScriptNodeTypeInfo &p_type_info, const Ref<FlowScript> &p_root_flow_script, const FlowScriptNodeReference &p_edited_node_reference, const Ref<Theme> &p_msdf_theme)
{
	set_theme(p_msdf_theme);

	type_info = p_type_info;
	root_flow_script = p_root_flow_script;
	edited_node_reference = p_edited_node_reference;

	if (p_edited_node_reference.include_id == FlowScriptConstants::INCLUDE_ID_INVALID)
	{
		owner_flow_script = p_root_flow_script;
	}
	else
	{
		owner_flow_script = p_root_flow_script->get_include_flow_script(p_edited_node_reference.include_id);
	}

	edited_node = owner_flow_script->get_node_data(p_edited_node_reference.node_id);
}


void FlowScriptNodeEditor::_notification(int p_what)
{
	if (p_what == NOTIFICATION_THEME_CHANGED)
	{
		const Ref<Texture2D> icon_edit = get_editor_theme_icon(SNAME("Edit"));
		const Ref<Texture2D> icon_remove = get_editor_theme_icon(SNAME("Remove"));

		rename_button->set_button_icon(icon_edit);
		delete_button->set_button_icon(icon_remove);
	}
}


FlowScriptNodeEditor::FlowScriptNodeEditor()
{
	set_h_size_flags(Control::SIZE_SHRINK_BEGIN);
	set_v_size_flags(Control::SIZE_SHRINK_BEGIN);

	rename_button = memnew(Button);
	rename_button->hide();
	rename_button->set_tooltip_text(TTR("Rename node."));
	rename_button->connect(SceneStringName(pressed), callable_mp(this, &FlowScriptNodeEditor::on_rename_button_pressed));
	get_titlebar_hbox()->add_child(rename_button);

	delete_button = memnew(Button);
	delete_button->hide();
	delete_button->set_tooltip_text(TTR("Delete node."));
	delete_button->connect(SceneStringName(pressed), callable_mp(this, &FlowScriptNodeEditor::on_delete_button_pressed));
	get_titlebar_hbox()->add_child(delete_button);
}
