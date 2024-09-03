#include "flow_script_node_editor.hpp"
#include "../flow_script_node_type_db.hpp"
#include "../flow_script_node_type_info.hpp"
#include "editor/editor_string_names.h"
#include "scene/gui/label.h"
#include "scene/gui/box_container.h"


void FlowScriptNodeEditor::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_edited_node_id"), &FlowScriptNodeEditor::get_edited_node_id);
	ClassDB::bind_method(D_METHOD("get_edited_node"), &FlowScriptNodeEditor::get_edited_node_ref);
	ClassDB::bind_method(D_METHOD("get_edited_flow_script"), &FlowScriptNodeEditor::get_edited_flow_script_ref);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "edited_node_id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "", "get_edited_node_id");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "edited_node", PROPERTY_HINT_RESOURCE_TYPE, "FlowScriptNode", PROPERTY_USAGE_NONE, "FlowScriptNode"), "", "get_edited_node");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "edited_flow_script", PROPERTY_HINT_RESOURCE_TYPE, "FlowScript", PROPERTY_USAGE_NONE, "FlowScript"), "", "get_edited_flow_script");
	
	GDVIRTUAL_BIND(_startup);
	GDVIRTUAL_BIND(_cleanup);
	GDVIRTUAL_BIND(_sync);
	GDVIRTUAL_BIND(_update_theme);
	GDVIRTUAL_BIND(_get_new_title);
	GDVIRTUAL_BIND(_get_new_tooltip_text);
	GDVIRTUAL_BIND(_get_input_slot);
	GDVIRTUAL_BIND(_output_graph_slot_to_connection, "graph_slot");
	GDVIRTUAL_BIND(_output_connection_to_graph_slot, "list", "slot");

	ADD_SIGNAL(MethodInfo("rename_request"));
}


void FlowScriptNodeEditor::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_THEME_CHANGED:
			Ref<Texture2D> rename_icon = get_theme_icon(SNAME("Edit"), EditorStringName(EditorIcons));
			Ref<Texture2D> delete_icon = get_theme_icon(SNAME("Remove"), EditorStringName(EditorIcons));
			rename_button->set_icon(rename_icon);
			delete_button->set_icon(delete_icon);
			if (is_editable())
			{
				update_theme();
			}
			break;
	}
}


void FlowScriptNodeEditor::block_editing()
{
	current_editable = false;
}


void FlowScriptNodeEditor::permit_editing()
{
	current_editable = true;
}


bool FlowScriptNodeEditor::is_editable() const
{
	return current_editable;
}


Ref<FlowScript> FlowScriptNodeEditor::get_root_flow_script_ref() const
{
	return root_flow_script;
}


FlowScript *FlowScriptNodeEditor::get_root_flow_script_ptr() const
{
	return root_flow_script.ptr();
}


Ref<FlowScript> FlowScriptNodeEditor::get_edited_flow_script_ref() const
{
	return edited_flow_script;
}


FlowScript *FlowScriptNodeEditor::get_edited_flow_script_ptr() const
{
	return edited_flow_script.ptr();
}


Ref<FlowScriptNode> FlowScriptNodeEditor::get_edited_node_ref() const
{
	return edited_flow_script->get_node_ref(edited_node_id);
}


 FlowScriptNode *FlowScriptNodeEditor::get_edited_node_ptr() const
{
	return edited_flow_script->get_node_ptr(edited_node_id);
}


bool FlowScriptNodeEditor::is_edited_flow_script_root() const
{
	return edited_flow_script != nullptr && edited_flow_script == root_flow_script;
}


FlowScriptNodeID FlowScriptNodeEditor::get_edited_node_id() const
{
	return edited_node_id;
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


void FlowScriptNodeEditor::update_theme()
{
	GDVIRTUAL_CALL(_update_theme);
}


String FlowScriptNodeEditor::get_new_title() const
{
	String ret = "ERROR: NO NODE";
	if (!GDVIRTUAL_CALL(_get_new_title, ret))
	{
		FlowScriptNode *node = get_edited_node_ptr();
		if (node != nullptr)
		{
			const FlowScriptNodeTypeInfo &type_info = FlowScriptNodeTypeDB::get_singleton()->get_type_of_node(node);
			if (type_info.enabled)
			{
				String node_res_name = node->get_name();
				if (node_res_name.is_empty())
				{
					ret = vformat("%d. %s", edited_node_id, type_info.name);
				}
				else
				{
					ret = vformat("%d. %s - %s", edited_node_id, type_info.name, node_res_name);
				}
			}
		}
	}
	return ret;
}


String FlowScriptNodeEditor::get_new_tooltip_text() const
{
	String ret;
	if (!GDVIRTUAL_CALL(_get_new_tooltip_text, ret))
	{
		FlowScriptNode *node = get_edited_node_ptr();
		Ref<Script> script = node->get_script();
		if (script.is_valid() && script->get_global_name() != StringName())
		{
			ret = script->get_global_name();
		}
		else
		{
			ret = node->get_class();
		}
	}
	return ret;
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


FlowScriptNodeEditor::FlowScriptNodeEditor()
{
	rename_button = memnew(Button);
	rename_button->connect(SceneStringName(pressed), callable_mp(this, &FlowScriptNodeEditor::on_rename_button_pressed));
	get_titlebar_hbox()->add_child(rename_button);

	delete_button = memnew(Button);
	delete_button->connect(SceneStringName(pressed), callable_mp(this, &FlowScriptNodeEditor::on_delete_button_pressed));
	get_titlebar_hbox()->add_child(delete_button);
}
