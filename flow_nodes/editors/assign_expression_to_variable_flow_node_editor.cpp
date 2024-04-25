#include "assign_expression_to_variable_flow_node_editor.hpp"
#include "scene/gui/label.h"
#include "scene/gui/box_container.h"


static const int INPUT_SLOT = 0;
static const int OUTPUT_SLOT = 0;


AssignExpressionToVariableFlowNode *AssignExpressionToVariableFlowNodeEditor::get_aetv_flow_node() const
{
	return Object::cast_to<AssignExpressionToVariableFlowNode>(get_flow_node());
}


int AssignExpressionToVariableFlowNodeEditor::_get_input_slot() const
{
	return INPUT_SLOT;
}


void AssignExpressionToVariableFlowNodeEditor::_set_out_going_connection(const Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection)
{
	if (p_out_going_connection->get_origin_slot() == OUTPUT_SLOT)
	{
		get_aetv_flow_node()->set_next_flow_node_id(p_out_going_connection->get_target_flow_node_id());
	}
}


TypedArray<FlowNodeEditorOutGoingConnectionParameters> AssignExpressionToVariableFlowNodeEditor::_get_out_going_connections() const
{
	TypedArray<FlowNodeEditorOutGoingConnectionParameters> connections;
	FlowNodeID next_id = get_aetv_flow_node()->get_next_flow_node_id();

	if (next_id != FLOW_NODE_ID_NIL)
	{
		connections.append(create_out_going_connection(next_id, OUTPUT_SLOT));
	}

	return connections;
}


void AssignExpressionToVariableFlowNodeEditor::_flow_node_updated()
{
	variable_name_label->set_text(get_aetv_flow_node()->get_variable_name());
	expression_label->set_text(get_aetv_flow_node()->get_expression());
}


void AssignExpressionToVariableFlowNodeEditor::_clean_up()
{
	variable_name_label->set_text("???");
	expression_label->set_text("???");
}


AssignExpressionToVariableFlowNodeEditor::AssignExpressionToVariableFlowNodeEditor()
{
	HBoxContainer *hbox = memnew(HBoxContainer);
	hbox->set_h_size_flags(SIZE_EXPAND_FILL);
	hbox->set_v_size_flags(SIZE_EXPAND_FILL);
	add_child(hbox);

	variable_name_label = memnew(Label);
	variable_name_label->set_text("???");
	variable_name_label->set_h_size_flags(SIZE_SHRINK_BEGIN);
	variable_name_label->set_v_size_flags(SIZE_EXPAND_FILL);
	variable_name_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	variable_name_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	hbox->add_child(variable_name_label);

	assignment_label = memnew(Label);
	assignment_label->set_text(" = ");
	assignment_label->set_h_size_flags(SIZE_SHRINK_CENTER);
	assignment_label->set_v_size_flags(SIZE_SHRINK_CENTER);
	hbox->add_child(assignment_label);

	expression_label = memnew(Label);
	expression_label->set_text("???");
	expression_label->set_h_size_flags(SIZE_EXPAND_FILL);
	expression_label->set_v_size_flags(SIZE_EXPAND_FILL);
	expression_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	expression_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	hbox->add_child(expression_label);

	set_slot_enabled_left(INPUT_SLOT, true);
	set_slot_enabled_right(OUTPUT_SLOT, true);
}
