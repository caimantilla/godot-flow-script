#include "flow_script_node_editor_wait_duration_fixed_seconds.hpp"
#include "../../nodes/flow_script_node_wait_duration_fixed_seconds.hpp"


void FlowScriptNodeEditorWaitDurationFixedSeconds::sync()
{
	const Ref<FlowScriptNodeWaitDurationFixedSeconds> node = get_edited_node();
	ERR_FAIL_COND(node.is_null());

	const double duration = node->get_duration();
	const bool defined_duration_ok = duration > 0.0;

	if (defined_duration_ok)
	{
		label->set_modulate(Color::named("white"));
		label->set_text(vformat(TTR("Wait %s seconds."), String::num_real(duration, false)));
	}
	else
	{
		label->set_modulate(Color::named("red"));
		label->set_text(vformat(TTR("Invalid duration of %s seconds defined."), String::num_real(duration, false)));
	}
}


FlowScriptNodeOutputConnection FlowScriptNodeEditorWaitDurationFixedSeconds::output_graph_slot_to_connection(const int p_graph_slot) const
{
	return FlowScriptNodeOutputConnection(FlowScriptNodeWaitDurationFixedSeconds::CONNECTION_LIST_ADVANCE, 0);
}


int FlowScriptNodeEditorWaitDurationFixedSeconds::output_connection_to_graph_slot(const FlowScriptNodeOutputConnection &p_connection) const
{
	ERR_FAIL_COND_V(p_connection.list != FlowScriptNodeWaitDurationFixedSeconds::CONNECTION_LIST_ADVANCE, -1);
	return 0;
}


int FlowScriptNodeEditorWaitDurationFixedSeconds::get_input_slot() const
{
	return 0;
}


FlowScriptNodeEditorWaitDurationFixedSeconds::FlowScriptNodeEditorWaitDurationFixedSeconds()
{
	label = memnew(Label);
	add_child(label);

	set_slot_enabled_left(0, true);
	set_slot_enabled_right(0, true);
}
