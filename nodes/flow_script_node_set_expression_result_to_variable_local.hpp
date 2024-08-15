#ifndef FLOW_SCRIPT_NODE_SET_EXPRESSION_RESULT_TO_VARIABLE_LOCAL_HPP
#define FLOW_SCRIPT_NODE_SET_EXPRESSION_RESULT_TO_VARIABLE_LOCAL_HPP


#include "flow_script_node_set_expression_result_to_variable.hpp"


class FlowScriptNodeSetExpressionResultToVariableLocal : public FlowScriptNodeSetExpressionResultToVariable
{
	GDCLASS(FlowScriptNodeSetExpressionResultToVariableLocal, FlowScriptNodeSetExpressionResultToVariable);

protected:
	virtual bool exec_variable_assignment(FlowScriptNodeContext *p_context, const String &p_key, const Variant &p_value) override;
};


#endif // FLOW_SCRIPT_NODE_SET_EXPRESSION_RESULT_TO_VARIABLE_LOCAL_HPP
