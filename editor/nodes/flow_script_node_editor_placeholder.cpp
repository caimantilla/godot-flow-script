#include "flow_script_node_editor_placeholder.hpp"
#include "scene/gui/label.h"


bool FlowScriptNodeEditorPlaceholder::is_placeholder() const
{
	return true;
}


void FlowScriptNodeEditorPlaceholder::sync()
{
	if (get_edited_include_id() == FlowScriptConstants::INCLUDE_ID_INVALID)
	{
		placeholder_indicate_label->set_text(vformat(TTR("Failed to instantiate editor for Node #%d..."), get_edited_node_id()));
	}
	else
	{
		placeholder_indicate_label->set_text(vformat(TTR("Failed to instantiate editor for Node #%d of Include #%d..."), get_edited_node_id(), get_edited_include_id()));
	}
}


int FlowScriptNodeEditorPlaceholder::get_input_slot() const
{
	return ANON_INPUT_PORT;
}


int FlowScriptNodeEditorPlaceholder::output_connection_to_graph_slot(const FlowScriptNodeOutputConnection &p_connection) const
{
	return ANON_OUTPUT_PORT;
}


FlowScriptNodeOutputConnection FlowScriptNodeEditorPlaceholder::output_graph_slot_to_connection(const int p_graph_slot) const
{
	return FlowScriptNodeOutputConnection::create_connection(0, 0);
}


FlowScriptNodeEditorPlaceholder::FlowScriptNodeEditorPlaceholder()
{
	placeholder_indicate_label = memnew(Label);
	add_child(placeholder_indicate_label);

	set_slot_enabled_left(0, true);
	set_slot_enabled_right(0, true);
}
