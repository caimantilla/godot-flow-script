#include "flow_script_node_editor_return_expression_result.hpp"
#include "nodes/flow_script_node_return_expression_result.hpp"
#include "scene/gui/box_container.h"


void FlowScriptNodeEditorReturnExpressionResult::sync()
{
	FlowScriptNodeReturnExpressionResult *node = Object::cast_to<FlowScriptNodeReturnExpressionResult>(get_edited_node_ptr());
	expression_box->set_expression(node->get_expression());
}


int FlowScriptNodeEditorReturnExpressionResult::get_input_slot() const
{
	return 0;
}


FlowScriptNodeEditorReturnExpressionResult::FlowScriptNodeEditorReturnExpressionResult()
{
	Label *header_label = memnew(Label);
	header_label->set_text(TTR("Evaluate and return result:"));
	add_child(header_label);

	HBoxContainer *hbox = memnew(HBoxContainer);
	hbox->add_spacer();
	add_child(hbox);

	expression_box = memnew(FlowScriptEditorExpressionDisplayBox);
	expression_box->set_h_size_flags(SIZE_EXPAND_FILL);
	hbox->add_child(expression_box);

	set_slot_enabled_left(0, true);
}
