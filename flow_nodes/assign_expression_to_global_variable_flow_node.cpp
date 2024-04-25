#include "assign_expression_to_global_variable_flow_node.hpp"


void AssignExpressionToGlobalVariableFlowNode::_execute(FlowNodeExecutionState *p_state)
{
	Variant result = evaluate_expression(p_state);
	p_state->get_flow_bridge()->set_global(get_variable_name(), result);
	
	p_state->finish(get_next_flow_node_id(), Variant());
}
