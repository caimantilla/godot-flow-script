#include "flow_script_node_editor_wait_duration_expression_result.hpp"
#include "nodes/flow_script_node_wait_duration_expression_result.hpp"
#include "scene/gui/margin_container.h"
#include "scene/gui/box_container.h"


void FlowScriptNodeEditorWaitDurationExpressionResult::sync()
{
	FlowScriptNodeWaitDurationExpressionResult *node = Object::cast_to<FlowScriptNodeWaitDurationExpressionResult>(get_edited_node_ptr());

	String expression = node->get_expression();
	if (expression.strip_edges().is_empty())
	{
		expression_label->hide();
		no_expression_defined_label->show();
		expression_label->set_text("");
	}
	else
	{
		no_expression_defined_label->hide();
		expression_label->set_text(expression);
		expression_label->show();
	}
}


FlowScriptNodeOutputConnection FlowScriptNodeEditorWaitDurationExpressionResult::output_graph_slot_to_connection(const int p_graph_slot) const
{
	return FlowScriptNodeOutputConnection(FlowScriptNodeWaitDurationExpressionResult::CONNECTION_LIST_ADVANCE, 0);
}


int FlowScriptNodeEditorWaitDurationExpressionResult::output_connection_to_graph_slot(const FlowScriptNodeOutputConnection &p_connection) const
{
	ERR_FAIL_COND_V(p_connection.list != FlowScriptNodeWaitDurationExpressionResult::CONNECTION_LIST_ADVANCE, -1);
	return 1;
}


int FlowScriptNodeEditorWaitDurationExpressionResult::get_input_slot() const
{
	return 0;
}


FlowScriptNodeEditorWaitDurationExpressionResult::FlowScriptNodeEditorWaitDurationExpressionResult()
{
	prefix_label = memnew(Label);
	prefix_label->set_text(TTR("Wait evaluated time:"));
	add_child(prefix_label);

	HBoxContainer *bottom_hbox = memnew(HBoxContainer);
	bottom_hbox->add_spacer();
	add_child(bottom_hbox);

	MarginContainer *expression_parent = memnew(MarginContainer);
	bottom_hbox->add_child(expression_parent);

	no_expression_defined_label = memnew(Label);
	no_expression_defined_label->set_text(TTR("No expression defined."));
	expression_parent->add_child(no_expression_defined_label);

	expression_label = memnew(RichTextLabel);
	expression_label->set_auto_translate(false);
	expression_parent->add_child(expression_label);

	set_slot_enabled_left(0, true);
	set_slot_enabled_right(1, true);
}
