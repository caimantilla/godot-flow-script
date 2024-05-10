#include "multi_branch_execution_flow_node_editor.hpp"
#include "scene/gui/box_container.h"
#include "scene/gui/label.h"


static const int INPUT_SLOT = 0;
static const int MAIN_OUTPUT_SLOT = 0;
static const int FIRST_STACK_SLOT = 2;


class MultiBranchExecutionFlowNodeEditor::ExecutionStackEntryGUIComponent : public VBoxContainer
{
	GDCLASS(ExecutionStackEntryGUIComponent, VBoxContainer);

public:
	int stack_idx;

	ExecutionStackEntryGUIComponent(const int p_idx, const bool p_last)
	{
		stack_idx = p_idx;
		
		set_h_size_flags(SIZE_SHRINK_END);
		set_v_size_flags(SIZE_SHRINK_BEGIN);

		Label *label = memnew(Label);
		label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);
		label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
		add_child(label);

		label->set_text(vformat("Branch %d:", p_idx));
		if (!p_last)
		{
			add_child(memnew(HSeparator));
		}
	}
};


int MultiBranchExecutionFlowNodeEditor::_get_input_slot() const
{
	return INPUT_SLOT;
}


TypedArray<FlowNodeEditorOutGoingConnectionParameters> MultiBranchExecutionFlowNodeEditor::_get_out_going_connections() const
{
	TypedArray<FlowNodeEditorOutGoingConnectionParameters> ret;

	if (get_mbe_flow_node()->get_next_flow_node_id() != FLOW_NODE_ID_NIL)
	{
		ret.append(create_out_going_connection(get_mbe_flow_node()->get_next_flow_node_id(), MAIN_OUTPUT_SLOT));
	}

	for (int i = 0; i < get_mbe_flow_node()->get_execution_stack_size(); i++)
	{
		FlowNodeID entry_next_node_id = get_mbe_flow_node()->get_execution_stack_entry_at(i);
		if (entry_next_node_id != FLOW_NODE_ID_NIL)
		{
			ret.append(create_out_going_connection(entry_next_node_id, FIRST_STACK_SLOT + i));
		}
	}

	return ret;
}


void MultiBranchExecutionFlowNodeEditor::_set_out_going_connection(const Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection)
{
	if (p_out_going_connection->get_origin_slot() == MAIN_OUTPUT_SLOT)
	{
		get_mbe_flow_node()->set_next_flow_node_id(p_out_going_connection->get_target_flow_node_id());
	}
	else
	{
		int slot_stack_idx = p_out_going_connection->get_origin_slot() - FIRST_STACK_SLOT;

		if (slot_stack_idx > -1 && slot_stack_idx < get_mbe_flow_node()->get_execution_stack_size())
		{
			get_mbe_flow_node()->set_execution_stack_entry_at(slot_stack_idx, p_out_going_connection->get_target_flow_node_id());
		}
	}
}


void MultiBranchExecutionFlowNodeEditor::_initialize()
{
	refresh_gui_stack();
}


void MultiBranchExecutionFlowNodeEditor::_flow_node_updated_immediate()
{
	refresh_gui_stack_if_necessary();
}


void MultiBranchExecutionFlowNodeEditor::_flow_node_updated()
{
	main_label->set_text(_get_main_label_text());
}


void MultiBranchExecutionFlowNodeEditor::_clean_up()
{
	clear_gui_stack();
}


MultiBranchExecutionFlowNode *MultiBranchExecutionFlowNodeEditor::get_mbe_flow_node() const
{
	return Object::cast_to<MultiBranchExecutionFlowNode>(get_flow_node());
}


MultiBranchExecutionFlowNodeEditor::ExecutionStackEntryGUIComponent *MultiBranchExecutionFlowNodeEditor::create_execution_stack_entry_gui_component(const int p_idx, const bool p_last)
{
	ExecutionStackEntryGUIComponent *ret = memnew(ExecutionStackEntryGUIComponent(p_idx, p_last));
	execution_stack_gui_components.push_back(ret);
	add_child(ret);

	return ret;
}


void MultiBranchExecutionFlowNodeEditor::clear_gui_stack()
{
	for (int i = execution_stack_gui_components.size() - 1; i > -1; i--)
	{
		int slot_idx = FIRST_STACK_SLOT + i;

		set_slot_enabled_right(slot_idx, false);

		ExecutionStackEntryGUIComponent *current_gui_component = execution_stack_gui_components.get(i);
		current_gui_component->hide();
		current_gui_component->queue_free();
	}

	execution_stack_gui_components.clear();
	main_separator->hide();
}


void MultiBranchExecutionFlowNodeEditor::refresh_gui_stack()
{
	clear_gui_stack();

	int stack_size = get_mbe_flow_node()->get_execution_stack_size();
	if (stack_size < 1)
	{
		main_separator->hide();
	}
	else
	{
		main_separator->show();
	}
	for (int i = 0; i < stack_size; i++)
	{
		int slot_idx = FIRST_STACK_SLOT + i;

		ExecutionStackEntryGUIComponent *current_gui_component = create_execution_stack_entry_gui_component(i, i >= stack_size);
		set_slot_enabled_right(slot_idx, true);
	}
}


void MultiBranchExecutionFlowNodeEditor::refresh_gui_stack_if_necessary()
{
	if (get_mbe_flow_node()->get_execution_stack_size() != execution_stack_gui_components.size())
	{
		refresh_gui_stack();
	}
	else
	{
		int stack_size = get_mbe_flow_node()->get_execution_stack_size();
		for (int i = 0; i < stack_size; i++)
		{
			if (execution_stack_gui_components.get(i)->stack_idx != i)
			{
				refresh_gui_stack();
				return;
			}
		}
	}
}


String MultiBranchExecutionFlowNodeEditor::_get_main_label_text() const
{
	return "Generic MBE Node?";
}


MultiBranchExecutionFlowNodeEditor::MultiBranchExecutionFlowNodeEditor()
{
	main_label = memnew(Label);
	main_label->set_h_size_flags(SIZE_SHRINK_END);
	main_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);
	add_child(main_label);

	set_slot_enabled_left(INPUT_SLOT, true);
	set_slot_enabled_right(MAIN_OUTPUT_SLOT, true);

	main_separator = memnew(HSeparator);
	main_separator->hide();
	add_child(main_separator);
}
