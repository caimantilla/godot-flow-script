#include "while_expression_true_loop_flow_node.hpp"


void WhileExpressionTrueLoopFlowNode::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_expression", "expression"), &WhileExpressionTrueLoopFlowNode::set_expression);
	ClassDB::bind_method(D_METHOD("get_expression"), &WhileExpressionTrueLoopFlowNode::get_expression);
	ClassDB::bind_method(D_METHOD("set_loop_flow_node_id", "node_id"), &WhileExpressionTrueLoopFlowNode::set_loop_flow_node_id);
	ClassDB::bind_method(D_METHOD("get_loop_flow_node_id"), &WhileExpressionTrueLoopFlowNode::get_loop_flow_node_id);
	ClassDB::bind_method(D_METHOD("set_finished_flow_node_id", "node_id"), &WhileExpressionTrueLoopFlowNode::set_finished_flow_node_id);
	ClassDB::bind_method(D_METHOD("get_finished_flow_node_id"), &WhileExpressionTrueLoopFlowNode::get_finished_flow_node_id);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "expression", PROPERTY_HINT_EXPRESSION), "set_expression", "get_expression");
	ADD_PROPERTY(PropertyInfo(TYPE_FLOW_NODE_ID, "loop_flow_node_id"), "set_loop_flow_node_id", "get_loop_flow_node_id");
	ADD_PROPERTY(PropertyInfo(TYPE_FLOW_NODE_ID, "finished_flow_node_id"), "set_finished_flow_node_id", "get_finished_flow_node_id");
}


void WhileExpressionTrueLoopFlowNode::do_loop_iteration(FlowNodeExecutionState *p_state)
{
	bool result = p_state->get_flow_bridge()->evaluate_multi_line_boolean_expression(expression, false);

	if (result)
	{
		p_state->request_new_fiber(loop_flow_node_id, true);
	}
	else
	{
		p_state->finish(finished_flow_node_id, Variant());
	}
}


void WhileExpressionTrueLoopFlowNode::set_expression(const String &p_expression)
{
	if (p_expression == expression)
	{
		return;
	}

	expression = p_expression;
	emit_changed();
}


String WhileExpressionTrueLoopFlowNode::get_expression() const
{
	return expression;
}


void WhileExpressionTrueLoopFlowNode::set_loop_flow_node_id(const FlowNodeID p_flow_node_id)
{
	if (p_flow_node_id == loop_flow_node_id)
	{
		return;
	}

	loop_flow_node_id = p_flow_node_id;
	emit_changed();
}


FlowNodeID WhileExpressionTrueLoopFlowNode::get_loop_flow_node_id() const
{
	return loop_flow_node_id;
}


void WhileExpressionTrueLoopFlowNode::set_finished_flow_node_id(const FlowNodeID p_flow_node_id)
{
	if (p_flow_node_id == finished_flow_node_id)
	{
		return;
	}

	finished_flow_node_id = p_flow_node_id;
	emit_changed();
}


FlowNodeID WhileExpressionTrueLoopFlowNode::get_finished_flow_node_id() const
{
	return finished_flow_node_id;
}


bool WhileExpressionTrueLoopFlowNode::_is_property_flow_node_reference(const StringName &p_name) const
{
	return p_name == SNAME("loop_flow_node_id") || p_name == SNAME("finished_flow_node_id");
}


void WhileExpressionTrueLoopFlowNode::_on_external_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to)
{
	if (loop_flow_node_id == p_from)
	{
		loop_flow_node_id = p_to;
	}

	if (finished_flow_node_id == p_from)
	{
		finished_flow_node_id = p_to;
	}
}


void WhileExpressionTrueLoopFlowNode::_execute(FlowNodeExecutionState *p_state)
{
	Callable resumed_callback = callable_mp(this, &WhileExpressionTrueLoopFlowNode::do_loop_iteration).bind(p_state);
	p_state->connect(SNAME("resumed"), resumed_callback);

	resumed_callback.call();
}
