#include "wait_seconds_flow_node_editor.hpp"
#include "scene/gui/label.h"


static const int INPUT_SLOT = 0;
static const int OUTPUT_SLOT = 0;


WaitSecondsFlowNode *WaitSecondsFlowNodeEditor::get_ws_flow_node() const
{
	return Object::cast_to<WaitSecondsFlowNode>(get_flow_node());
}


void WaitSecondsFlowNodeEditor::on_duration_spinner_value_changed(float p_value)
{
	get_ws_flow_node()->set_timer_duration_no_signal(p_value);
}


void WaitSecondsFlowNodeEditor::_clean_up()
{
	text_label->set_text("No node to wait for.");
}


void WaitSecondsFlowNodeEditor::_flow_node_updated()
{
	String new_lbl_str;
	float curr_duration = get_ws_flow_node()->get_timer_duration();

	if (Math::is_equal_approx(1.0f, curr_duration))
	{
		new_lbl_str = "Wait 1 second.";
	}
	else
	{
		new_lbl_str = vformat("Wait %.1f seconds.", curr_duration);
	}

	text_label->set_text(new_lbl_str);
}


int WaitSecondsFlowNodeEditor::_get_input_slot() const
{
	return INPUT_SLOT;
}


void WaitSecondsFlowNodeEditor::_set_out_going_connection(const Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection)
{
	if (p_out_going_connection->get_origin_slot() == 0)
	{
		get_ws_flow_node()->set_next_flow_node_id(p_out_going_connection->get_target_flow_node_id());
	}
}


TypedArray<FlowNodeEditorOutGoingConnectionParameters> WaitSecondsFlowNodeEditor::_get_out_going_connections() const
{
	TypedArray<FlowNodeEditorOutGoingConnectionParameters> ret;

	if (get_ws_flow_node()->get_next_flow_node_id() != FLOW_NODE_ID_NIL)
	{
		ret.append(create_out_going_connection(get_ws_flow_node()->get_next_flow_node_id(), 0));
	}

	return ret;
}


WaitSecondsFlowNodeEditor::WaitSecondsFlowNodeEditor()
{
	text_label = memnew(Label);
	add_child(text_label);

	set_slot_enabled_left(INPUT_SLOT, true);
	set_slot_enabled_right(OUTPUT_SLOT, true);
}
