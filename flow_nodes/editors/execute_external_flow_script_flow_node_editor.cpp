#include "execute_external_flow_script_flow_node_editor.hpp"
#include "scene/gui/label.h"
#include "core/variant/variant_utility.h"


static const int INPUT_SLOT = 0;
static const int OUTPUT_SLOT = 0;


int ExecuteExternalFlowScriptFlowNodeEditor::_get_input_slot() const
{
	return INPUT_SLOT;
}


void ExecuteExternalFlowScriptFlowNodeEditor::_set_out_going_connection(Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection)
{
	if (p_out_going_connection->get_origin_slot() == OUTPUT_SLOT)
	{
		get_eefs_node()->set_next_flow_node_id(p_out_going_connection->get_target_flow_node_id());
	}
}


TypedArray<FlowNodeEditorOutGoingConnectionParameters> ExecuteExternalFlowScriptFlowNodeEditor::_get_out_going_connections() const
{
	TypedArray<FlowNodeEditorOutGoingConnectionParameters> ret;
	if (get_eefs_node()->get_next_flow_node_id() != FLOW_NODE_ID_NIL)
	{
		ret.append(create_out_going_connection(get_eefs_node()->get_next_flow_node_id(), OUTPUT_SLOT));
	}

	return ret;
}


void ExecuteExternalFlowScriptFlowNodeEditor::_flow_node_updated()
{
	String new_text;
	if (!VariantUtilityFunctions::is_instance_valid(get_eefs_node()->get_external_flow_script()))
	{
		new_text = "No FlowScript assigned.";
	}
	else
	{
		String script_str;
		if (get_eefs_node()->get_external_flow_script()->get_owner() != nullptr)
		{
			script_str = get_eefs_node()->get_external_flow_script()->get_owner()->get_path_to(get_eefs_node()->get_external_flow_script());
		}
		else
		{
			script_str = get_eefs_node()->get_external_flow_script()->get_name();
		}

		if (get_eefs_node()->get_external_flow_script()->has_flow_node_with_name(get_eefs_node()->get_external_call_name()))
		{
			String call_str = get_eefs_node()->get_external_call_name();

			if (get_eefs_node()->is_wait_external_flow_script_finished_enabled())
			{

				new_text = vformat("Wait for %s to finish at %s.", call_str, script_str);
			}
			else
			{
				new_text = vformat("Trigger %s at %s.", call_str, script_str);
			}
		}
		else
		{
			new_text = vformat("No valid procedure for %s.", script_str);
		}
	}

	desc_label->set_text(new_text);
}


ExecuteExternalFlowScriptFlowNode *ExecuteExternalFlowScriptFlowNodeEditor::get_eefs_node() const
{
	return Object::cast_to<ExecuteExternalFlowScriptFlowNode>(get_flow_node());
}


ExecuteExternalFlowScriptFlowNodeEditor::ExecuteExternalFlowScriptFlowNodeEditor()
{
	desc_label = memnew(Label);
	add_child(desc_label);

	set_slot_enabled_left(INPUT_SLOT, true);
	set_slot_enabled_right(OUTPUT_SLOT, true);
}
