#include "print_to_console_flow_node.hpp"


void PrintToConsoleFlowNode::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_next_flow_node_id", "flow_node_id"), &PrintToConsoleFlowNode::set_next_flow_node_id);
	ClassDB::bind_method(D_METHOD("get_next_flow_node_id"), &PrintToConsoleFlowNode::get_next_flow_node_id);
	ClassDB::bind_method(D_METHOD("set_console_message", "console_message"), &PrintToConsoleFlowNode::set_console_message);
	ClassDB::bind_method(D_METHOD("get_console_message"), &PrintToConsoleFlowNode::get_console_message);
	ClassDB::bind_method(D_METHOD("set_format_expressions", "format_expressions"), &PrintToConsoleFlowNode::set_format_expressions);
	ClassDB::bind_method(D_METHOD("get_format_expressions"), &PrintToConsoleFlowNode::get_format_expressions);

	ADD_PROPERTY(PropertyInfo(TYPE_FLOW_NODE_ID, "next_flow_node_id"), "set_next_flow_node_id", "get_next_flow_node_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "console_message", PROPERTY_HINT_MULTILINE_TEXT), "set_console_message", "get_console_message");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "format_expressions", PROPERTY_HINT_EXPRESSION), "set_format_expressions", "get_format_expressions");
}


void PrintToConsoleFlowNode::_execute(FlowNodeExecutionState *p_state)
{
	Array evaluation_results = p_state->get_flow_bridge()->evaluate_multi_line_expression(format_expressions);
	String formatted_message = console_message.format(evaluation_results);

	p_state->get_flow_bridge()->print_to_console(formatted_message);
	p_state->finish(next_flow_node_id, Variant());
}


bool PrintToConsoleFlowNode::_is_property_flow_node_reference(const StringName &p_name) const
{
	return p_name == SNAME("next_flow_node_id");
}


void PrintToConsoleFlowNode::_on_external_flow_node_id_changed(const int p_from, const int p_to)
{
	if (p_from == next_flow_node_id)
	{
		next_flow_node_id = p_to;
	}
}


void PrintToConsoleFlowNode::set_next_flow_node_id(const FlowNodeID p_flow_node_id)
{
	if (p_flow_node_id == next_flow_node_id)
	{
		return;
	}

	next_flow_node_id = p_flow_node_id;
	emit_changed();
}


FlowNodeID PrintToConsoleFlowNode::get_next_flow_node_id() const
{
	return next_flow_node_id;
}


void PrintToConsoleFlowNode::set_console_message(const String &p_console_message)
{
	if (p_console_message == console_message)
	{
		return;
	}

	console_message = p_console_message;
	emit_changed();
}


String PrintToConsoleFlowNode::get_console_message() const
{
	return console_message;
}


void PrintToConsoleFlowNode::set_format_expressions(const String &p_format_expressions)
{
	if (p_format_expressions == format_expressions)
	{
		return;
	}

	format_expressions = p_format_expressions;
	emit_changed();
}


String PrintToConsoleFlowNode::get_format_expressions() const
{
	return format_expressions;
}
