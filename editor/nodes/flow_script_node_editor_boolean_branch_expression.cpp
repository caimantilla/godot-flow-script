#include "flow_script_node_editor_boolean_branch_expression.hpp"
#include "nodes/flow_script_node_boolean_branch_expression.hpp"


void FlowScriptNodeEditorBooleanBranchExpression::sync()
{
	FlowScriptNodeBooleanBranchExpression *node = Object::cast_to<FlowScriptNodeBooleanBranchExpression>(get_edited_node_ptr());
	if (node->get_expression().strip_edges().is_empty())
	{
		lbl_condition->set_text("");
		lbl_condition->add_text(TTR("No condition defined.", "FlowScript"));
	}
	else
	{
		lbl_condition->set_text("");
		lbl_condition->push_list(0, RichTextLabel::LIST_ROMAN, false);
		lbl_condition->add_text(node->get_expression());
		lbl_condition->pop();
	}
}


int FlowScriptNodeEditorBooleanBranchExpression::get_input_slot() const
{
	return 1;
}


FlowScriptNodeOutputConnection FlowScriptNodeEditorBooleanBranchExpression::output_graph_slot_to_connection(const int p_graph_slot) const
{
	switch (p_graph_slot)
	{
		case 0:
			return FlowScriptNodeOutputConnection(FlowScriptNodeBooleanBranchExpression::CONNECTION_LIST_TRUE, 0);
		case 2:
			return FlowScriptNodeOutputConnection(FlowScriptNodeBooleanBranchExpression::CONNECTION_LIST_FALSE, 0);
		default:
			ERR_FAIL_V(FlowScriptNodeOutputConnection());
	}
}


int FlowScriptNodeEditorBooleanBranchExpression::output_connection_to_graph_slot(const FlowScriptNodeOutputConnection &p_connection) const
{
	switch (p_connection.list)
	{
		case FlowScriptNodeBooleanBranchExpression::CONNECTION_LIST_TRUE:
			return 0;
		case FlowScriptNodeBooleanBranchExpression::CONNECTION_LIST_FALSE:
			return 2;
		default:
			ERR_FAIL_V(-1);
	}
}


Label *FlowScriptNodeEditorBooleanBranchExpression::create_result_label(const String &p_text, const Color &p_color)
{
	Label *ret = memnew(Label);
	ret->set_text(p_text);
	ret->set_modulate(p_color);
	ret->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);
	ret->set_h_size_flags(SIZE_SHRINK_END);
	ret->set_v_size_flags(SIZE_SHRINK_CENTER);
	return ret;
}


FlowScriptNodeEditorBooleanBranchExpression::FlowScriptNodeEditorBooleanBranchExpression()
{
	lbl_true = create_result_label("True:", Color::named("green"));
	lbl_false = create_result_label("False:", Color::named("red"));

	lbl_condition = memnew(RichTextLabel);
	lbl_condition->set_h_size_flags(SIZE_EXPAND_FILL);
	lbl_condition->set_v_size_flags(SIZE_EXPAND_FILL);
	lbl_condition->set_autowrap_mode(TextServer::AUTOWRAP_OFF);
	lbl_condition->set_fit_content(true);
	lbl_condition->set_use_bbcode(true);
	lbl_condition->set_scroll_active(false);

	add_child(lbl_true);
	add_child(lbl_condition);
	add_child(lbl_false);

	set_slot_enabled_left(1, true);
	set_slot_enabled_right(0, true);
	set_slot_enabled_right(2, true);
}
