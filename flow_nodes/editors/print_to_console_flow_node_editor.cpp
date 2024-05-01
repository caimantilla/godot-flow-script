#include "print_to_console_flow_node_editor.hpp"
#include "scene/gui/label.h"
#include "scene/gui/box_container.h"
#include "scene/gui/separator.h"


static const int INPUT_SLOT = 0;
static const int OUTPUT_SLOT = 2;


PrintToConsoleFlowNode *PrintToConsoleFlowNodeEditor::get_ptc_flow_node() const
{
	return Object::cast_to<PrintToConsoleFlowNode>(get_flow_node());
}


int PrintToConsoleFlowNodeEditor::_get_input_slot() const
{
	return INPUT_SLOT;
}


void PrintToConsoleFlowNodeEditor::_set_out_going_connection(const Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection)
{
	PrintToConsoleFlowNode *ptc_node = get_ptc_flow_node();

	if (p_out_going_connection->get_origin_slot() == OUTPUT_SLOT)
	{
		ptc_node->set_next_flow_node_id(p_out_going_connection->get_target_flow_node_id());
	}
}


TypedArray<FlowNodeEditorOutGoingConnectionParameters> PrintToConsoleFlowNodeEditor::_get_out_going_connections() const
{
	TypedArray<FlowNodeEditorOutGoingConnectionParameters> ret;

	if (get_ptc_flow_node()->get_next_flow_node_id() != FLOW_NODE_ID_NIL)
	{
		ret.append(create_out_going_connection(get_ptc_flow_node()->get_next_flow_node_id(), OUTPUT_SLOT));
	}

	return ret;
}


void PrintToConsoleFlowNodeEditor::_flow_node_updated()
{
	PrintToConsoleFlowNode *ptc_node = get_ptc_flow_node();

	String format_expr_string = "No Format Expressions";
	if (!ptc_node->get_format_expressions().is_empty())
	{
		PackedStringArray format_expr_lines = ptc_node->get_format_expressions().split("\n");
		
		for (int i = 0; i < format_expr_lines.size(); i++)
		{
			String line = vformat("%d. ", i) + format_expr_lines.get(i);
			format_expr_lines.set(i, line);
		}
		
		format_expr_string = String("\n").join(format_expr_lines);
	}

	console_message_label->set_text(ptc_node->get_console_message());
	format_expressions_label->set_text(format_expr_string);
}


void PrintToConsoleFlowNodeEditor::_clean_up()
{
	console_message_label->set_text("");
	format_expressions_label->set_text("");
}


PrintToConsoleFlowNodeEditor::PrintToConsoleFlowNodeEditor()
{
	VBoxContainer *ml_vbox = memnew(VBoxContainer);
	ml_vbox->set_h_size_flags(SIZE_EXPAND_FILL);
	ml_vbox->set_v_size_flags(SIZE_FILL);
	add_child(ml_vbox);

	Label *ml_top_label = memnew(Label);
	ml_top_label->set_text("Console Message");
	ml_vbox->add_child(ml_top_label);

	HBoxContainer *ml_hbox = memnew(HBoxContainer);
	ml_hbox->set_h_size_flags(SIZE_EXPAND_FILL);
	ml_hbox->set_v_size_flags(SIZE_EXPAND_FILL);
	ml_hbox->add_spacer();
	ml_vbox->add_child(ml_hbox);

	console_message_label = memnew(Label);
	console_message_label->set_h_size_flags(SIZE_EXPAND_FILL);
	console_message_label->set_v_size_flags(SIZE_EXPAND_FILL);
	console_message_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	console_message_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	ml_hbox->add_child(console_message_label);

	add_child(memnew(HSeparator));

	VBoxContainer *fel_vbox = memnew(VBoxContainer);
	fel_vbox->set_h_size_flags(SIZE_EXPAND_FILL);
	fel_vbox->set_v_size_flags(SIZE_FILL);
	add_child(fel_vbox);

	Label *fel_top_label = memnew(Label);
	fel_top_label->set_text("Format Expressions");
	fel_vbox->add_child(fel_top_label);

	HBoxContainer *fel_hbox = memnew(HBoxContainer);
	fel_hbox->set_h_size_flags(SIZE_EXPAND_FILL);
	fel_hbox->set_v_size_flags(SIZE_EXPAND_FILL);
	fel_hbox->add_spacer();
	fel_vbox->add_child(fel_hbox);

	format_expressions_label = memnew(Label);
	format_expressions_label->set_h_size_flags(SIZE_EXPAND_FILL);
	format_expressions_label->set_v_size_flags(SIZE_EXPAND_FILL);
	format_expressions_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	format_expressions_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	fel_vbox->add_child(format_expressions_label);

	set_slot_enabled_left(INPUT_SLOT, true);
	set_slot_enabled_right(OUTPUT_SLOT, true);
}
