#include "flow_script_node_editor.hpp"


void FlowScriptNodeEditor::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_edited_flow_script"), &FlowScriptNodeEditor::get_edited_flow_script_ref);
	ClassDB::bind_method(D_METHOD("get_edited_node_id"), &FlowScriptNodeEditor::get_edited_node_id);
	ClassDB::bind_method(D_METHOD("get_edited_node"), &FlowScriptNodeEditor::get_edited_node_ref);
	
	GDVIRTUAL_BIND(_startup);
	GDVIRTUAL_BIND(_cleanup);
	GDVIRTUAL_BIND(_sync);
	GDVIRTUAL_BIND(_update_theme);
	GDVIRTUAL_BIND(_get_new_title);
	GDVIRTUAL_BIND(_get_new_tooltip_text);
	GDVIRTUAL_BIND(_set_outgoing_connection, "connection");
	GDVIRTUAL_BIND(_get_outgoing_connections);
	GDVIRTUAL_BIND(_get_input_slot);
}


void FlowScriptNodeEditor::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_THEME_CHANGED:
			if (is_editable())
				update_theme();
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


void FlowScriptNodeEditor::set_edited_flow_script(FlowScript *p_flow_script)
{
	edited_flow_script = p_flow_script;
}


Ref<FlowScript> FlowScriptNodeEditor::get_edited_flow_script_ref() const
{
	return Ref<FlowScript>(edited_flow_script);
}


FlowScript *FlowScriptNodeEditor::get_edited_flow_script_ptr() const
{
	return edited_flow_script;
}


Ref<FlowScriptNode> FlowScriptNodeEditor::get_edited_node_ref() const
{
	return edited_flow_script->get_node_ref(edited_node_id);
}


 FlowScriptNode *FlowScriptNodeEditor::get_edited_node_ptr() const
{
	return edited_flow_script->get_node_ptr(edited_node_id);
}


void FlowScriptNodeEditor::set_edited_node_id(const FlowScriptNodeID p_node_id)
{
	edited_node_id = p_node_id;
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
	String ret;
	if (!GDVIRTUAL_CALL(_get_new_title, ret))
	{
		FlowScriptNode *node = get_edited_node_ptr();
		if (node == nullptr)
		{
			ret = "ERROR: NO NODE";
		}
		else
		{
			ret = node->get_type_name() + " #" + itos(edited_node_id);
			String node_res_name = node->get_name();
			if (!node_res_name.is_empty())
			{
				ret = node_res_name + ": " + ret;
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
		PackedStringArray lines;
		FlowScriptNode *node = get_edited_node_ptr();
		String node_res_name = node->get_name();
		if (!node_res_name.is_empty())
		{
			lines.push_back("Name: " + node_res_name);
		}
		lines.push_back("ID: " + itos(edited_node_id));
		lines.push_back("Type: " + node->get_type_name());

		const static String nl_sep = "\n";
		ret = nl_sep.join(lines);
	}
	return ret;
}


void FlowScriptNodeEditor::set_outgoing_connection(FlowScriptNodeEditorOutgoingConnectionParameters p_connection)
{
	Dictionary dict = p_connection.to_dictionary();
	GDVIRTUAL_CALL(_set_outgoing_connection, dict);
}


void FlowScriptNodeEditor::get_outgoing_connections(List<FlowScriptNodeEditorOutgoingConnectionParameters> *p_list) const
{
	TypedArray<Dictionary> virtual_connections;
	GDVIRTUAL_CALL(_get_outgoing_connections, virtual_connections);

	for (int i = 0; i < virtual_connections.size(); i++)
	{
		Dictionary curr_connection_virtual = virtual_connections.get(i);
		FlowScriptNodeEditorOutgoingConnectionParameters curr_connection = FlowScriptNodeEditorOutgoingConnectionParameters::create_from_dictionary(curr_connection_virtual);
		p_list->push_back(curr_connection);
	}
}


int FlowScriptNodeEditor::get_input_slot() const
{
	int ret = -1;
	GDVIRTUAL_CALL(_get_input_slot, ret);
	return ret;
}


int FlowScriptNodeEditor::input_port_to_slot(const int p_port) const
{
	return get_input_port_slot(p_port);
}


int FlowScriptNodeEditor::input_slot_to_port(const int p_slot) const
{
	int port = p_slot;
	for (int i = 0; i < p_slot; i++)
	{
		if (!is_slot_enabled_left(i))
			port--;
	}
	return port;
}


int FlowScriptNodeEditor::output_port_to_slot(const int p_port) const
{
	return get_output_port_slot(p_port);
}


int FlowScriptNodeEditor::output_slot_to_port(const int p_slot) const
{
	int port = p_slot;
	for (int i = 0; i < p_slot; i++)
	{
		if (!is_slot_enabled_right(i))
			port--;
	}
	return port;
}


FlowScriptNodeEditor::FlowScriptNodeEditor()
{
}
