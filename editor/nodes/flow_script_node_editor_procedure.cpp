#include "flow_script_node_editor_procedure.hpp"
#include "scene/gui/label.h"


int FlowScriptNodeEditorProcedure::get_input_slot() const
{
	return 0;
}


FlowScriptNodeOutputConnection FlowScriptNodeEditorProcedure::output_graph_slot_to_connection(const int p_graph_slot) const
{
	return FlowScriptNodeOutputConnection(0, 0);
}


int FlowScriptNodeEditorProcedure::output_connection_to_graph_slot(const FlowScriptNodeOutputConnection &p_connection) const
{
	return 0;
}


FlowScriptNodeEditorProcedure::FlowScriptNodeEditorProcedure()
{
	Label *label = memnew(Label);
	label->set_text("Start!");
	label->set_h_size_flags(SIZE_SHRINK_END);
	add_child(label);
	set_slot_enabled_left(0, true);
	set_slot_enabled_right(0, true);
}
