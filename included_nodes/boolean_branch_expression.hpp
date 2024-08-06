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
	void set_expression(const String &p_expression);
	String get_expression() const;

	virtual String get_type_id() const override { return "boolean_branch_expression"; }
	virtual String get_type_name() const override { return "Conditional Branch"; }
	virtual String get_type_description() const override { return "Branches based on conditions which are evaluated as expressions."; }
	virtual String get_type_editor() const override { return "FlowScriptNodeEditorBooleanBranchExpression"; }
	
	virtual void exec_step(FlowScriptNodeContext *p_context) override;
};


VARIANT_ENUM_CAST(FlowScriptNodeBooleanBranchExpression::ConnectionList);


#endif // FLOW_SCRIPT_NODE_BOOLEAN_BRANCH_EXPRESSION_HPP
