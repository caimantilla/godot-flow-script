#ifndef FLOW_SCRIPT_NODE_LOOP_WHILE_EXPRESSION_RESULT_TRUE_HPP
#define FLOW_SCRIPT_NODE_LOOP_WHILE_EXPRESSION_RESULT_TRUE_HPP


#include "common.hpp"


class FlowScriptNodeLoopWhileExpressionResultTrue : public FlowScriptNode
{
	GDCLASS(FlowScriptNodeLoopWhileExpressionResultTrue, FlowScriptNode);

public:
	enum ConnectionList
	{
		CONNECTION_LIST_BREAK = 0,
		CONNECTION_LIST_LOOP = 1,
	};

	enum TemporaryVariable
	{
		VARIABLE_STEP_COUNT = 0,
		VARIABLE_RESTORE_SAVE = 1,
	};

private:
	String expression;

protected:
	static void _bind_methods();

public:
	virtual void exec_startup(FlowScriptNodeContext *p_context) override;
	virtual void exec_step(FlowScriptNodeContext *p_context) override;

	virtual void set_state(FlowScriptNodeContext *p_context, const Dictionary &p_state) override;
	virtual void get_state(const FlowScriptNodeContext *p_context, Dictionary &r_state) const override;

	virtual void set_json_data(const Dictionary &p_data) override;
	virtual void get_json_data(Dictionary &r_data) const override;

	virtual void get_output_connection_list_lengths(List<int64_t> &r_lengths) const override;

	void set_expression(const String &p_text);
	String get_expression() const;
};


VARIANT_ENUM_CAST(FlowScriptNodeLoopWhileExpressionResultTrue::ConnectionList);
VARIANT_ENUM_CAST(FlowScriptNodeLoopWhileExpressionResultTrue::TemporaryVariable);


#endif // FLOW_SCRIPT_NODE_LOOP_WHILE_EXPRESSION_RESULT_TRUE_HPP
