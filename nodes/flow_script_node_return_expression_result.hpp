#ifndef FLOW_SCRIPT_NODE_RETURN_EXPRESSION_RESULT_HPP
#define FLOW_SCRIPT_NODE_RETURN_EXPRESSION_RESULT_HPP


#include "common.hpp"


class FlowScriptNodeReturnExpressionResult final : public FlowScriptNode
{
	GDCLASS(FlowScriptNodeReturnExpressionResult, FlowScriptNode);

private:
	String expression;

protected:
	static void _bind_methods();

public:
	virtual void exec_step(FlowScriptNodeContext *p_context) override;
	virtual void set_data_state(const Dictionary &p_data) override;
	virtual Dictionary get_data_state() const override;

	void set_expression(const String &p_text);
	String get_expression() const;
};


#endif // FLOW_SCRIPT_NODE_RETURN_EXPRESSION_RESULT_HPP
