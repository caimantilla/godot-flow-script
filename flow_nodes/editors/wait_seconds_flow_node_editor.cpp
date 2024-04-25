#include "wait_seconds_flow_node_editor.hpp"


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


void WaitSecondsFlowNodeEditor::_initialize()
{
	duration_spinner->connect(SNAME("value_changed"), callable_mp(this, &WaitSecondsFlowNodeEditor::on_duration_spinner_value_changed));

	WaitSecondsFlowNode *node = get_ws_flow_node();
	duration_spinner->set_value_no_signal(node->get_timer_duration());
}


void WaitSecondsFlowNodeEditor::_clean_up()
{
	duration_spinner->disconnect(SNAME("value_changed"), callable_mp(this, &WaitSecondsFlowNodeEditor::on_duration_spinner_value_changed));
}


void WaitSecondsFlowNodeEditor::_flow_node_updated()
{
	duration_spinner->set_value_no_signal(get_ws_flow_node()->get_timer_duration());
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
	TypedArray<FlowNodeEditorOutGoingConnectionParameters> connections;
	FlowNodeID next_id = get_ws_flow_node()->get_next_flow_node_id();

	if (next_id != FLOW_NODE_ID_NIL)
	{
		Ref<FlowNodeEditorOutGoingConnectionParameters> connection = create_out_going_connection(next_id, 0);
		connections.append(connection);
	}

	return connections;
}


WaitSecondsFlowNodeEditor::WaitSecondsFlowNodeEditor()
{
	duration_spinner = memnew(SpinBox);
	duration_spinner->set_min(WaitSecondsFlowNode::MIN_DURATION);
	duration_spinner->set_max(WaitSecondsFlowNode::MAX_DURATION);
	duration_spinner->set_step(WaitSecondsFlowNode::STEP);
	duration_spinner->set_suffix("Seconds(s)");
	add_child(duration_spinner);

	set_slot_enabled_left(INPUT_SLOT, true);
	set_slot_enabled_right(OUTPUT_SLOT, true);
}
