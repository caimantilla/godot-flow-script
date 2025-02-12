#ifndef FLOW_SCRIPT_NODE_BOOLEAN_BRANCH_EXPRESSION_HPP
#define FLOW_SCRIPT_NODE_BOOLEAN_BRANCH_EXPRESSION_HPP


#include "common.hpp"


class FlowScriptNodeBooleanBranchExpression final : public FlowScriptNode
{
	GDCLASS(FlowScriptNodeBooleanBranchExpression, FlowScriptNode);

public:
	enum ConnectionList
	{
		CONNECTION_LIST_TRUE = 0,
		CONNECTION_LIST_FALSE = 1,
	};

private:
	String expression;

protected:
	static void _bind_methods();

public:
	virtual void exec_step(FlowScriptNodeContext *p_context) override;
	virtual void set_data_state(const Dictionary &p_data) override;
	virtual Dictionary get_data_state() const override;
	virtual void get_output_connection_list_lengths(List<FlowScriptNodeConnectionListLength> *p_lengths) const override;

	void set_expression(const String &p_expression);
	String get_expression() const;
};


VARIANT_ENUM_CAST(FlowScriptNodeBooleanBranchExpression::ConnectionList);


#endif // FLOW_SCRIPT_NODE_BOOLEAN_BRANCH_EXPRESSION_HPP
