#ifndef FLOW_SCRIPT_NODE_WAIT_DURATION_EXPRESSION_RESULT_HPP
#define FLOW_SCRIPT_NODE_WAIT_DURATION_EXPRESSION_RESULT_HPP


#include "flow_script_node_wait_duration.hpp"


class FlowScriptNodeWaitDurationExpressionResult final : public FlowScriptNodeWaitDuration
{
	GDCLASS(FlowScriptNodeWaitDurationExpressionResult, FlowScriptNodeWaitDuration);

private:
	String expression;

protected:
	static void _bind_methods();

	virtual real_t get_initial_duration(FlowScriptNodeContext *p_context) const override;

public:
	void set_expression(const String &p_text);
	String get_expression() const;
};


#endif // FLOW_SCRIPT_NODE_WAIT_DURATION_EXPRESSION_RESULT_HPP
