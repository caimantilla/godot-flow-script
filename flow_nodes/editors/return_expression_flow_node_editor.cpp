#include "return_expression_flow_node_editor.hpp"
#include "scene/gui/label.h"


static const int INPUT_SLOT = 0;


ReturnExpressionFlowNode *ReturnExpressionFlowNodeEditor::get_re_flow_node() const
{
	return Object::cast_to<ReturnExpressionFlowNode>(get_flow_node());
}


void ReturnExpressionFlowNodeEditor::_flow_node_updated()
{
	expression_label->set_text(get_re_flow_node()->get_expression());
}


void ReturnExpressionFlowNodeEditor::_clean_up()
{
	expression_label->set_text("");
}


int ReturnExpressionFlowNodeEditor::_get_input_slot() const
{
	return INPUT_SLOT;
}


ReturnExpressionFlowNodeEditor::ReturnExpressionFlowNodeEditor()
{
	expression_label = memnew(Label);
	expression_label->set_h_size_flags(SIZE_EXPAND_FILL);
	expression_label->set_v_size_flags(SIZE_EXPAND_FILL);
	expression_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	expression_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	add_child(expression_label);

	set_slot_enabled_left(INPUT_SLOT, true);
}
