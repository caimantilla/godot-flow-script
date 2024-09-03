#include "flow_script_node_editor_loop_while_expression_result_true.hpp"
#include "../../nodes/flow_script_node_loop_while_expression_result_true.hpp"
#include "scene/gui/separator.h"
#include "scene/gui/box_container.h"


void FlowScriptNodeEditorLoopWhileExpressionResultTrue::sync()
{
	FlowScriptNodeLoopWhileExpressionResultTrue *node = Object::cast_to<FlowScriptNodeLoopWhileExpressionResultTrue>(get_edited_node_ptr());
	expression_box->set_expression(node->get_expression());
}


FlowScriptNodeOutputConnection FlowScriptNodeEditorLoopWhileExpressionResultTrue::output_graph_slot_to_connection(const int p_graph_slot) const
{
	switch (p_graph_slot)
	{
		case 0:
			return FlowScriptNodeOutputConnection(FlowScriptNodeLoopWhileExpressionResultTrue::CONNECTION_LIST_BREAK, 0);
		case 2:
			return FlowScriptNodeOutputConnection(FlowScriptNodeLoopWhileExpressionResultTrue::CONNECTION_LIST_LOOP, 0);
		default:
			ERR_FAIL_V(FlowScriptNodeOutputConnection());
	}
}


int FlowScriptNodeEditorLoopWhileExpressionResultTrue::output_connection_to_graph_slot(const FlowScriptNodeOutputConnection &p_connection) const
{
	switch (p_connection.list)
	{
		case FlowScriptNodeLoopWhileExpressionResultTrue::CONNECTION_LIST_BREAK:
			return 0;
		case FlowScriptNodeLoopWhileExpressionResultTrue::CONNECTION_LIST_LOOP:
			return 2;
		default:
			ERR_FAIL_V(-1);
	}
}


int FlowScriptNodeEditorLoopWhileExpressionResultTrue::get_input_slot() const
{
	return 0;
}


FlowScriptNodeEditorLoopWhileExpressionResultTrue::FlowScriptNodeEditorLoopWhileExpressionResultTrue()
{
	Label *advance_label = memnew(Label);
	advance_label->set_text(TTR("Once false, break out to:"));
	add_child(advance_label);

	add_child(memnew(HSeparator));

	Label *expression_header_label = memnew(Label);
	expression_header_label->set_text(TTR("While true, execute:"));

	HBoxContainer *expression_space = memnew(HBoxContainer);
	expression_space->add_spacer();
	add_child(expression_space);

	expression_box = memnew(FlowScriptEditorExpressionDisplayBox);
	expression_space->add_child(expression_box);

	set_slot_enabled_left(0, true);
	set_slot_enabled_right(0, true);
	set_slot_enabled_right(2, true);
}
