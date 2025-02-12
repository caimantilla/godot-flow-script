#include "flow_script_node_editor_multi_branch_execute.hpp"


void FlowScriptNodeEditorMultiBranchExecute::sync()
{
	const Ref<FlowScriptNodeMultiBranchExecute> node = get_edited_node();
	ERR_FAIL_COND(node.is_null());

	const int branch_count = MIN(node->get_connection_count(), output_control_list.size());

	for (int i = 0; i < branch_count; i++)
	{
		output_control_show(i);
	}
	for (int i = branch_count; i < output_control_list.size(); i++)
	{
		output_control_hide(i);
	}
}


FlowScriptNodeOutputConnection FlowScriptNodeEditorMultiBranchExecute::output_graph_slot_to_connection(const int p_graph_slot) const
{
	if (p_graph_slot == 0)
	{
		return FlowScriptNodeOutputConnection(FlowScriptNodeMultiBranchExecute::CONNECTION_LIST_ADVANCE, 0);
	}
	else
	{
		return FlowScriptNodeOutputConnection(FlowScriptNodeMultiBranchExecute::CONNECTION_LIST_BRANCHES, graph_slot_get_output_control(p_graph_slot));
	}
}


int FlowScriptNodeEditorMultiBranchExecute::output_connection_to_graph_slot(const FlowScriptNodeOutputConnection &p_connection) const
{
	switch (p_connection.list)
	{
		case FlowScriptNodeMultiBranchExecute::CONNECTION_LIST_ADVANCE:
			return 0;
		case FlowScriptNodeMultiBranchExecute::CONNECTION_LIST_BRANCHES:
			return output_control_get_graph_slot(p_connection.slot);
		default:
			ERR_FAIL_V(-1);
	}
}


int FlowScriptNodeEditorMultiBranchExecute::get_input_slot() const
{
	return 0;
}


int FlowScriptNodeEditorMultiBranchExecute::output_control_get_graph_slot(const int p_idx) const
{
	return (p_idx + 1) * 2;
}


int FlowScriptNodeEditorMultiBranchExecute::graph_slot_get_output_control(const int p_graph_slot) const
{
	return (p_graph_slot / 2) - 1;
}


void FlowScriptNodeEditorMultiBranchExecute::output_control_show(const int p_idx)
{
	output_control_list[p_idx].separator->show();
	output_control_list[p_idx].label->show();
	set_slot_enabled_right(output_control_get_graph_slot(p_idx), true);
}


void FlowScriptNodeEditorMultiBranchExecute::output_control_hide(const int p_idx)
{
	set_slot_enabled_right(output_control_get_graph_slot(p_idx), false);
	output_control_list[p_idx].separator->hide();
	output_control_list[p_idx].label->hide();
}


FlowScriptNodeEditorMultiBranchExecute::FlowScriptNodeEditorMultiBranchExecute()
{
	Label *main_output_label = memnew(Label);
	main_output_label->set_text(TTR("When finished:"));
	main_output_label->set_h_size_flags(Control::SIZE_SHRINK_END);
	add_child(main_output_label);

	set_slot_enabled_left(0, true);
	set_slot_enabled_right(0, true);

	for (int i = 0; i < output_control_list.size(); i++)
	{
		HSeparator *separator = memnew(HSeparator);
		separator->hide();
		add_child(separator);

		Label *label = memnew(Label);
		label->hide();
		label->set_text(vformat(TTR("Branch %d:"), i));
		label->set_h_size_flags(Control::SIZE_SHRINK_END);
		add_child(label);

		output_control_list[i].separator = separator;
		output_control_list[i].label = label;
	}
	for (int i = 0; i < output_control_list.size(); i++)
	{
		output_control_hide(i);
	}
}
