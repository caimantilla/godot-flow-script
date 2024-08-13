#ifndef FLOW_SCRIPT_NODE_SET_EXPRESSION_RESULT_TO_VARIABLE_GLOBAL_HPP
#define FLOW_SCRIPT_NODE_SET_EXPRESSION_RESULT_TO_VARIABLE_GLOBAL_HPP


#include "flow_script_node_set_expression_result_to_variable.hpp"


class FlowScriptNodeSetExpressionResultToVariableGlobal : public FlowScriptNodeSetExpressionResultToVariable
{
	GDCLASS(FlowScriptNodeSetExpressionResultToVariableGlobal, FlowScriptNodeSetExpressionResultToVariable);

protected:
	virtual bool exec_variable_assignment(FlowScriptNodeContext *p_context, const String &p_key, const Variant &p_value) override;

public:
	virtual String get_type_id() const override { return "set_expression_result_to_variable_global"; }
	virtual String get_type_name() const override { return "Assign Global Variable"; }
	virtual String get_type_description() const override { return "Evaluates an expression and assigns the result to a global variable."; }
};


#endif // FLOW_SCRIPT_NODE_SET_EXPRESSION_RESULT_TO_VARIABLE_GLOBAL_HPP
