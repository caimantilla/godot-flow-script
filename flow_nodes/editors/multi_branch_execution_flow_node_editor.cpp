#include "multi_branch_execution_flow_node_editor.hpp"
#include "scene/gui/box_container.h"
#include "scene/gui/label.h"
#include "scene/gui/separator.h"


static const int INPUT_SLOT = 0;
static const int MAIN_OUTPUT_SLOT = 0;
static const int FIRST_STACK_SLOT = 2;


class MultiBranchExecutionFlowNodeEditor::ExecutionStackEntryGUIComponent : public VBoxContainer
{
	GDCLASS(ExecutionStackEntryGUIComponent, VBoxContainer);

public:
	ExecutionStackEntryGUIComponent(const int p_idx, const bool p_last)
	{
		set_h_size_flags(SIZE_EXPAND_FILL);
		set_v_size_flags(SIZE_SHRINK_BEGIN);

		Label *label = memnew(Label);
		label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);
		label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
		add_child(label);

		if (p_last)
		{
			label->set_text("New Branch:");
		}
		else
		{
			label->set_text(vformat("Branch %d:", p_idx));
			add_child(memnew(HSeparator));
		}
	}
};


MultiBranchExecutionFlowNode *MultiBranchExecutionFlowNodeEditor::get_mbe_flow_node() const
{
	return Object::cast_to<MultiBranchExecutionFlowNode>(get_flow_node());
}


void MultiBranchExecutionFlowNodeEditor::_flow_node_updated()
{
	reload_execution_stack_gui_components();
}


MultiBranchExecutionFlowNodeEditor::ExecutionStackEntryGUIComponent *MultiBranchExecutionFlowNodeEditor::create_execution_stack_entry_gui_component(const int p_idx, const bool p_last)
{
	MultiBranchExecutionFlowNodeEditor::ExecutionStackEntryGUIComponent *component = memnew(MultiBranchExecutionFlowNodeEditor::ExecutionStackEntryGUIComponent(p_idx, p_last));
	execution_stack_gui_components.push_back(component);
	add_child(component);
	return component;
}


void MultiBranchExecutionFlowNodeEditor::clear_execution_stack_gui_components()
{
	for (MultiBranchExecutionFlowNodeEditor::ExecutionStackEntryGUIComponent *component : execution_stack_gui_components)
	{
		memdelete(component);
	}

	execution_stack_gui_components.clear();
}


void MultiBranchExecutionFlowNodeEditor::reload_execution_stack_gui_components()
{
	clear_execution_stack_gui_components();

	MultiBranchExecutionFlowNode *mbe_node = get_mbe_flow_node();

	int stack_size = mbe_node->get_execution_stack_size();
	int iter_range = MIN(mbe_node->get_max_execution_stack_size(), stack_size + 1);

	for (int i = 0; i < iter_range; i++)
	{
		bool is_last = (i == stack_size);
		MultiBranchExecutionFlowNodeEditor::ExecutionStackEntryGUIComponent *component = create_execution_stack_entry_gui_component(i, is_last);
	}

	for (int i = FIRST_STACK_SLOT; i < get_child_count(); i++)
	{
		set_slot_enabled_right(i, true);
	}
}


int MultiBranchExecutionFlowNodeEditor::_get_input_slot() const
{
	return INPUT_SLOT;
}


void MultiBranchExecutionFlowNodeEditor::_set_out_going_connection(const Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection)
{
	MultiBranchExecutionFlowNode *mbe_node = get_mbe_flow_node();

	if (p_out_going_connection->get_origin_slot() == MAIN_OUTPUT_SLOT)
	{
		mbe_node->set_next_flow_node_id(p_out_going_connection->get_target_flow_node_id());
	}
	else
	{
		int current_stack_size = mbe_node->get_execution_stack_size();
		int entry_idx = p_out_going_connection->get_origin_slot() - FIRST_STACK_SLOT;
		FlowNodeID target_node_id = p_out_going_connection->get_target_flow_node_id();

		if (target_node_id != FLOW_NODE_ID_NIL)
		{
			if (entry_idx < current_stack_size)
			{
				mbe_node->remove_execution_stack_entry_at(entry_idx);
			}
		}
		else
		{
			if (entry_idx >= current_stack_size)
			{
				mbe_node->resize_execution_stack(entry_idx + 1);
			}
			mbe_node->set_execution_stack_entry_at(entry_idx, target_node_id);
		}
	}
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
		FlowNodeID entry_id = get_mbe_flow_node()->get_execution_stack_entry_at(i);
		if (entry_id != FLOW_NODE_ID_NIL)
		{
			ret.append(create_out_going_connection(entry_id, FIRST_STACK_SLOT + i));
		}
	}

	return ret;
}


MultiBranchExecutionFlowNodeEditor::MultiBranchExecutionFlowNodeEditor()
{
	Label *main_slot_label = memnew(Label);
	main_slot_label->set_text("Once finished, execute:");
	main_slot_label->set_h_size_flags(SIZE_EXPAND_FILL);
	main_slot_label->set_v_size_flags(SIZE_SHRINK_BEGIN);
	main_slot_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);
	main_slot_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	add_child(main_slot_label);

	add_child(memnew(HSeparator));

	set_slot_enabled_left(INPUT_SLOT, true);
	set_slot_enabled_right(MAIN_OUTPUT_SLOT, true);
}
