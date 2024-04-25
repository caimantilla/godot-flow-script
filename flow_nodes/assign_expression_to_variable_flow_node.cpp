#include "assign_expression_to_variable_flow_node.hpp"


void AssignExpressionToVariableFlowNode::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_variable_name", "variable_name"), &AssignExpressionToVariableFlowNode::set_variable_name);
	ClassDB::bind_method(D_METHOD("get_variable_name"), &AssignExpressionToVariableFlowNode::get_variable_name);
	ClassDB::bind_method(D_METHOD("set_next_flow_node_id", "node_id"), &AssignExpressionToVariableFlowNode::set_next_flow_node_id);
	ClassDB::bind_method(D_METHOD("get_next_flow_node_id"), &AssignExpressionToVariableFlowNode::get_next_flow_node_id);
	ClassDB::bind_method(D_METHOD("set_expression", "expression"), &AssignExpressionToVariableFlowNode::set_expression);
	ClassDB::bind_method(D_METHOD("get_expression"), &AssignExpressionToVariableFlowNode::get_expression);

	ADD_PROPERTY(PropertyInfo(TYPE_FLOW_NODE_ID, "next_flow_node_id"), "set_next_flow_node_id", "get_next_flow_node_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "variable_name"), "set_variable_name", "get_variable_name");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "expression", PROPERTY_HINT_EXPRESSION, "", PROPERTY_USAGE_DEFAULT), "set_expression", "get_expression");
}


void AssignExpressionToVariableFlowNode::_on_external_flow_node_id_changed(const int p_from, const int p_to)
{
	if (next_flow_node_id == p_from)
	{
		next_flow_node_id = p_to;
	}
}


bool AssignExpressionToVariableFlowNode::_is_property_flow_node_reference(const StringName &p_name) const
{
	return p_name == SNAME("next_flow_node_id");
}


Variant AssignExpressionToVariableFlowNode::evaluate_expression(FlowNodeExecutionState *p_state)
{
	Variant result = p_state->get_flow_bridge()->evaluate_expression(expression);
	return result;
}


void AssignExpressionToVariableFlowNode::set_variable_name(const String &p_variable_name)
{
	if (variable_name == p_variable_name)
	{
		return;
	}

	variable_name = p_variable_name;
	emit_changed();
}


StringName AssignExpressionToVariableFlowNode::get_variable_name() const
{
	return variable_name;
}


void AssignExpressionToVariableFlowNode::set_next_flow_node_id(const FlowNodeID p_flow_node_id)
{
	if (p_flow_node_id == next_flow_node_id)
	{
		return;
	}

	next_flow_node_id = p_flow_node_id;
	emit_changed();
}


FlowNodeID AssignExpressionToVariableFlowNode::get_next_flow_node_id() const
{
	return next_flow_node_id;
}


void AssignExpressionToVariableFlowNode::set_expression(const String &p_expression)
{
	if (expression == p_expression)
	{
		return;
	}

	expression = p_expression;
	emit_changed();
}


String AssignExpressionToVariableFlowNode::get_expression() const
{
	return expression;
}
