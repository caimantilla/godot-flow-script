#include "flow_script_node_editor_set_expression_result_to_variable.hpp"
#include "../../nodes/flow_script_node_set_expression_result_to_variable.hpp"
#include "scene/gui/label.h"
#include "scene/gui/box_container.h"


void FlowScriptNodeEditorSetExpressionResultToVariable::sync()
{
	FlowScriptNodeSetExpressionResultToVariable *node = Object::cast_to<FlowScriptNodeSetExpressionResultToVariable>(get_edited_node_ptr());
	lbl_whitespace_warning->set_visible(node->get_variable() != node->get_variable().strip_edges());
	if (node->get_variable().is_empty())
	{
		lbl_variable->set_text("Undefined Variable");
		lbl_variable->set_modulate(Color::named("red"));
	}
	else
	{
		lbl_variable->set_text(node->get_variable());
		lbl_variable->set_modulate(Color::named("white"));
	}
	expression_box->set_expression(node->get_expression());
}


FlowScriptNodeEditorSetExpressionResultToVariable::FlowScriptNodeEditorSetExpressionResultToVariable()
{
	HBoxContainer *hbox = memnew(HBoxContainer);
	hbox->set_h_size_flags(SIZE_SHRINK_CENTER);
	hbox->set_v_size_flags(SIZE_SHRINK_CENTER);
	add_child(hbox);

	lbl_variable = memnew(Label);
	lbl_variable->set_auto_translate(false);
	lbl_variable->set_h_size_flags(SIZE_SHRINK_END);
	lbl_variable->set_v_size_flags(SIZE_SHRINK_CENTER);
	lbl_variable->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);
	lbl_variable->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	hbox->add_child(lbl_variable);

	Label *lbl_equals = memnew(Label);
	lbl_equals->set_auto_translate(false);
	lbl_equals->set_text(" = ");
	lbl_equals->set_h_size_flags(SIZE_SHRINK_CENTER);
	lbl_equals->set_v_size_flags(SIZE_SHRINK_CENTER);
	lbl_equals->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	lbl_equals->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	hbox->add_child(lbl_equals);

	expression_box = memnew(FlowScriptEditorExpressionDisplayBox);
	expression_box->set_h_size_flags(SIZE_SHRINK_BEGIN);
	expression_box->set_v_size_flags(SIZE_SHRINK_CENTER);
	hbox->add_child(expression_box);

	lbl_whitespace_warning = memnew(Label);
	lbl_whitespace_warning->set_text("WARNING: Whitespace found at variable key edge(s).");
	add_child(lbl_whitespace_warning);

	set_slot_enabled_left(0, true);
	set_slot_enabled_right(0, true);
}
