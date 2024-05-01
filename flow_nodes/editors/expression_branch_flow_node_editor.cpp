#include "expression_branch_flow_node_editor.hpp"


static const int SLOT_TRUE = 0;
static const int SLOT_INPUT = 1;
static const int SLOT_FALSE = 2;


ExpressionBranchFlowNode *ExpressionBranchFlowNodeEditor::get_eb_flow_node() const
{
	return Object::cast_to<ExpressionBranchFlowNode>(get_flow_node());
}


TypedArray<FlowNodeEditorOutGoingConnectionParameters> ExpressionBranchFlowNodeEditor::_get_out_going_connections() const
{
	TypedArray<FlowNodeEditorOutGoingConnectionParameters> ret;

	if (get_eb_flow_node()->get_true_flow_node_id() != FLOW_NODE_ID_NIL)
	{
		ret.append(create_out_going_connection(get_eb_flow_node()->get_true_flow_node_id(), SLOT_TRUE));
	}
	if (get_eb_flow_node()->get_false_flow_node_id() != FLOW_NODE_ID_NIL)
	{
		ret.append(create_out_going_connection(get_eb_flow_node()->get_false_flow_node_id(), SLOT_FALSE));
	}

	return ret;
}


int ExpressionBranchFlowNodeEditor::_get_input_slot() const
{
	return SLOT_INPUT;
}


void ExpressionBranchFlowNodeEditor::_set_out_going_connection(const Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection)
{
	ExpressionBranchFlowNode *eb_node = get_eb_flow_node();

	if (p_out_going_connection->get_origin_slot() == SLOT_TRUE)
	{
		eb_node->set_true_flow_node_id(p_out_going_connection->get_target_flow_node_id());
	}
	else if (p_out_going_connection->get_origin_slot() == SLOT_FALSE)
	{
		eb_node->set_false_flow_node_id(p_out_going_connection->get_target_flow_node_id());
	}
}


void ExpressionBranchFlowNodeEditor::_flow_node_updated()
{
	expression_label->set_text(get_eb_flow_node()->get_expression());
}


void ExpressionBranchFlowNodeEditor::_clean_up()
{
	expression_label->set_text("");
}


ExpressionBranchFlowNodeEditor::ExpressionBranchFlowNodeEditor()
{
	Color green = Color::named("GREEN");
	Color red = Color::named("RED");

	Label *true_label = memnew(Label);
	true_label->set_text("True");
	true_label->set_self_modulate(green);
	true_label->set_h_size_flags(SIZE_SHRINK_END);
	add_child(true_label);

	expression_label = memnew(Label);
	expression_label->set_h_size_flags(SIZE_EXPAND_FILL);
	expression_label->set_v_size_flags(SIZE_SHRINK_CENTER);
	expression_label->set_autowrap_mode(TextServer::AUTOWRAP_OFF);
	expression_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);
	expression_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	add_child(expression_label);

	Label *false_label = memnew(Label);
	false_label->set_text("False");
	false_label->set_self_modulate(red);
	false_label->set_h_size_flags(SIZE_SHRINK_END);
	add_child(false_label);

	set_slot_enabled_left(SLOT_INPUT, true);

	set_slot_enabled_right(SLOT_TRUE, true);
	set_slot_color_right(SLOT_TRUE, green);

	set_slot_enabled_right(SLOT_FALSE, true);
	set_slot_color_right(SLOT_FALSE, red);
}
