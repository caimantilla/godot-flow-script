#include "procedure.hpp"
#include "scene/gui/label.h"


void FlowScriptNodeEditorProcedure::startup()
{
	if (!is_edited_flow_script_root())
	{
		set_slot_enabled_left(0, true);
	}
}


int FlowScriptNodeEditorProcedure::get_input_slot() const
{
	if (is_edited_flow_script_root())
	{
		return FlowScriptNodeEditor::get_input_slot();
	}
	// Support input when in the "include" context
	else
	{
		return 0;
	}
}


void FlowScriptNodeEditorProcedure::set_outgoing_connection(const FlowScriptNodeEditorOutgoingConnectionParameters &p_connection)
{
	if (p_connection.origin_slot == 0)
	{
		get_edited_flow_script_ptr()->set_node_connection(get_edited_node_id(), 0, 0, p_connection.target_node_id);
	}
}


void FlowScriptNodeEditorProcedure::get_outgoing_connections(List<FlowScriptNodeEditorOutgoingConnectionParameters> *p_list) const
{
	p_list->push_back(FlowScriptNodeEditorOutgoingConnectionParameters::create_from_slot(0, get_edited_flow_script_ptr()->get_node_connection(get_edited_node_id(), 0, 0)));
}


FlowScriptNodeEditorProcedure::FlowScriptNodeEditorProcedure()
{
	Label *label = memnew(Label);
	label->set_text("Start!");
	add_child(label);
	set_slot_enabled_right(0, true);
}
