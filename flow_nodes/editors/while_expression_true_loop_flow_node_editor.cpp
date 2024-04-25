#include "while_expression_true_loop_flow_node_editor.hpp"
#include "scene/gui/label.h"
#include "scene/gui/box_container.h"
#include "scene/gui/separator.h"


static const int INPUT_SLOT = 0;
static const int LOOP_OUTPUT_SLOT = 0;
static const int FINISHED_OUTPUT_SLOT = 2;


WhileExpressionTrueLoopFlowNode *WhileExpressionTrueLoopFlowNodeEditor::get_wetl_flow_node() const
{
	return Object::cast_to<WhileExpressionTrueLoopFlowNode>(get_flow_node());
}


int WhileExpressionTrueLoopFlowNodeEditor::_get_input_slot() const
{
	return INPUT_SLOT;
}


void WhileExpressionTrueLoopFlowNodeEditor::_set_out_going_connection(const Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection)
{
	if (p_out_going_connection->get_origin_slot() == LOOP_OUTPUT_SLOT)
	{
		get_wetl_flow_node()->set_loop_flow_node_id(p_out_going_connection->get_target_flow_node_id());
	}

	if (p_out_going_connection->get_origin_slot() == FINISHED_OUTPUT_SLOT)
	{
		get_wetl_flow_node()->set_finished_flow_node_id(p_out_going_connection->get_target_flow_node_id());
	}
}


TypedArray<FlowNodeEditorOutGoingConnectionParameters> WhileExpressionTrueLoopFlowNodeEditor::_get_out_going_connections() const
{
	TypedArray<FlowNodeEditorOutGoingConnectionParameters> connections;

	FlowNodeID loop_id = get_wetl_flow_node()->get_loop_flow_node_id();
	FlowNodeID finished_id = get_wetl_flow_node()->get_finished_flow_node_id();

	if (loop_id != FLOW_NODE_ID_NIL)
	{
		connections.append(create_out_going_connection(loop_id, LOOP_OUTPUT_SLOT));
	}

	if (finished_id != FLOW_NODE_ID_NIL)
	{
		connections.append(create_out_going_connection(finished_id, FINISHED_OUTPUT_SLOT));
	}

	return connections;
}


void WhileExpressionTrueLoopFlowNodeEditor::_flow_node_updated()
{
	expression_label->set_text(get_wetl_flow_node()->get_expression());
}


void WhileExpressionTrueLoopFlowNodeEditor::_clean_up()
{
	expression_label->set_text("???");
}


WhileExpressionTrueLoopFlowNodeEditor::WhileExpressionTrueLoopFlowNodeEditor()
{
	HBoxContainer *loop_hbox = memnew(HBoxContainer);
	loop_hbox->set_h_size_flags(SIZE_EXPAND_FILL);
	loop_hbox->set_v_size_flags(SIZE_SHRINK_BEGIN);
	add_child(loop_hbox);

	Label *loop_label_left = memnew(Label);
	loop_label_left->set_text("While ");
	loop_label_left->set_h_size_flags(SIZE_SHRINK_END);
	loop_label_left->set_v_size_flags(SIZE_EXPAND_FILL);
	loop_label_left->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);
	loop_label_left->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	loop_hbox->add_child(loop_label_left);

	expression_label = memnew(Label);
	expression_label->set_h_size_flags(SIZE_EXPAND_FILL);
	expression_label->set_v_size_flags(SIZE_EXPAND_FILL);
	expression_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	expression_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	loop_hbox->add_child(expression_label);

	Label *loop_label_right = memnew(Label);
	loop_label_right->set_text(" is true, execute:");
	loop_label_right->set_h_size_flags(SIZE_SHRINK_END);
	loop_label_right->set_v_size_flags(SIZE_EXPAND_FILL);
	loop_label_right->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);
	loop_label_right->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	loop_hbox->add_child(loop_label_right);

	HSeparator *mid_sep = memnew(HSeparator);
	mid_sep->set_v_size_flags(SIZE_SHRINK_CENTER);
	add_child(mid_sep);

	Label *finished_label = memnew(Label);
	finished_label->set_text("Once false, break out to:");
	finished_label->set_h_size_flags(SIZE_EXPAND_FILL);
	finished_label->set_v_size_flags(SIZE_SHRINK_END);
	finished_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);
	finished_label->set_vertical_alignment(VERTICAL_ALIGNMENT_TOP);
	add_child(finished_label);

	set_slot_enabled_left(INPUT_SLOT, true);
	set_slot_enabled_right(LOOP_OUTPUT_SLOT, true);
	set_slot_enabled_right(FINISHED_OUTPUT_SLOT, true);
}
