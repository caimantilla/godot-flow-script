#include "procedure_flow_node_editor.hpp"
#include "scene/gui/label.h"


static const int OUTPUT_SLOT = 0;


ProcedureFlowNode *ProcedureFlowNodeEditor::get_procedure_flow_node() const
{
	return Object::cast_to<ProcedureFlowNode>(get_flow_node());
}


void ProcedureFlowNodeEditor::_set_out_going_connection(const Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection)
{
	if (p_out_going_connection->get_origin_slot() == OUTPUT_SLOT)
	{
		get_procedure_flow_node()->set_next_flow_node_id(p_out_going_connection->get_target_flow_node_id());
	}
}


TypedArray<FlowNodeEditorOutGoingConnectionParameters> ProcedureFlowNodeEditor::_get_out_going_connections() const
{
	TypedArray<FlowNodeEditorOutGoingConnectionParameters> ret;

	if (get_procedure_flow_node()->get_next_flow_node_id() != FLOW_NODE_ID_NIL)
	{
		ret.append(create_out_going_connection(get_procedure_flow_node()->get_next_flow_node_id(), OUTPUT_SLOT));
	}

	return ret;
}


ProcedureFlowNodeEditor::ProcedureFlowNodeEditor()
{
	Label *start_label = memnew(Label);
	start_label->set_text("Start!");
	start_label->set_h_size_flags(SIZE_SHRINK_END);
	start_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);
	add_child(start_label);

	set_slot_enabled_right(OUTPUT_SLOT, true);
}
