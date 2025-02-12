#ifndef FLOW_SCRIPT_NODE_LOOP_WHILE_EXPRESSION_RESULT_TRUE_HPP
#define FLOW_SCRIPT_NODE_LOOP_WHILE_EXPRESSION_RESULT_TRUE_HPP


#include "common.hpp"


// THIS CLASS IS NOT FUNCTIONAL
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
	virtual void set_runtime_state(FlowScriptNodeContext *p_context, const Dictionary &p_state) override;
	virtual Dictionary get_runtime_state(const FlowScriptNodeContext *p_context) const override;
	virtual void set_data_state(const Dictionary &p_data) override;
	virtual Dictionary get_data_state() const override;
	virtual void get_output_connection_list_lengths(List<FlowScriptNodeConnectionListLength> *p_lengths) const override;

	void set_expression(const String &p_text);
	String get_expression() const;
};


VARIANT_ENUM_CAST(FlowScriptNodeLoopWhileExpressionResultTrue::ConnectionList);
VARIANT_ENUM_CAST(FlowScriptNodeLoopWhileExpressionResultTrue::TemporaryVariable);


#endif // FLOW_SCRIPT_NODE_LOOP_WHILE_EXPRESSION_RESULT_TRUE_HPP
