#ifndef FLOW_SCRIPT_NODE_SET_EXPRESSION_RESULT_TO_VARIABLE_HPP
#define FLOW_SCRIPT_NODE_SET_EXPRESSION_RESULT_TO_VARIABLE_HPP


#include "common.hpp"


class FlowScriptNodeSetExpressionResultToVariable : public FlowScriptNode
{
	GDCLASS(FlowScriptNodeSetExpressionResultToVariable, FlowScriptNode);

public:
	enum ConnectionList
	{
		CONNECTION_LIST_ADVANCE = 0,
	};

private:
	String variable;
	String expression;

protected:
	static void _bind_methods();

	virtual bool exec_variable_assignment(FlowScriptNodeContext *p_context, const String &p_key, const Variant &p_value);

public:
	virtual void exec_step(FlowScriptNodeContext *p_context) override;
	
	virtual void set_json_data(const Dictionary &p_data) override;
	virtual void get_json_data(Dictionary &r_data) const override;

	virtual void get_output_connection_list_lengths(List<int64_t> &r_lengths) const override;

	void set_variable(const String &p_key);
	String get_variable() const;
	void set_expression(const String &p_text);
	String get_expression() const;
};


VARIANT_ENUM_CAST(FlowScriptNodeSetExpressionResultToVariable::ConnectionList);


#endif // FLOW_SCRIPT_NODE_SET_EXPRESSION_RESULT_TO_VARIABLE_HPP
