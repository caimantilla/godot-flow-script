#include "execute_expression_flow_node_editor.hpp"
#include "scene/gui/label.h"


static const int INPUT_SLOT = 0;
static const int OUTPUT_SLOT = 0;


ExecuteExpressionFlowNode *ExecuteExpressionFlowNodeEditor::get_ee_flow_node() const
{
	return Object::cast_to<ExecuteExpressionFlowNode>(get_flow_node());
}


void ExecuteExpressionFlowNodeEditor::_flow_node_updated()
{
	expression_label->set_text(get_ee_flow_node()->get_expression());
}


void ExecuteExpressionFlowNodeEditor::_clean_up()
{
	expression_label->set_text("");
}


void ExecuteExpressionFlowNodeEditor::_set_out_going_connection(const Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection)
{
	ExecuteExpressionFlowNode *ee_node = get_ee_flow_node();

	if (p_out_going_connection->get_origin_slot() == OUTPUT_SLOT)
	{
		ee_node->set_next_flow_node_id(p_out_going_connection->get_target_flow_node_id());
	}
}


TypedArray<FlowNodeEditorOutGoingConnectionParameters> ExecuteExpressionFlowNodeEditor::_get_out_going_connections() const
{
	TypedArray<FlowNodeEditorOutGoingConnectionParameters> connections;
	ExecuteExpressionFlowNode *ee_node = get_ee_flow_node();

	if (ee_node->get_next_flow_node_id() != FLOW_NODE_ID_NIL)
	{
		connections.append(create_out_going_connection(ee_node->get_next_flow_node_id(), OUTPUT_SLOT));
	}

	return connections;
}


int ExecuteExpressionFlowNodeEditor::_get_input_slot() const
{
	return INPUT_SLOT;
}


ExecuteExpressionFlowNodeEditor::ExecuteExpressionFlowNodeEditor()
{
	expression_label = memnew(Label);
	expression_label->set_h_size_flags(SIZE_EXPAND_FILL);
	expression_label->set_v_size_flags(SIZE_EXPAND_FILL);
	expression_label->set_autowrap_mode(TextServer::AUTOWRAP_OFF);
	expression_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	expression_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	add_child(expression_label);

	set_slot_enabled_left(INPUT_SLOT, true);
	set_slot_enabled_right(OUTPUT_SLOT, true);
}
