#include "flow_script_node_set_expression_result_to_variable_global.hpp"


bool FlowScriptNodeSetExpressionResultToVariableGlobal::exec_variable_assignment(FlowScriptNodeContext *p_context, const StringName &p_key, const Variant &p_value)
{
	p_context->get_bridge_ptr()->get_built_in_node_interface()->set_global(p_key, p_value);
	return true;
}
