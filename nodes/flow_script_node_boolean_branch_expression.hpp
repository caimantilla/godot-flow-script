#ifndef FLOW_SCRIPT_NODE_BOOLEAN_BRANCH_EXPRESSION_HPP
#define FLOW_SCRIPT_NODE_BOOLEAN_BRANCH_EXPRESSION_HPP


#include "common.hpp"
#include "scene/gui/label.h"
#include "scene/gui/rich_text_label.h"


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
	virtual String get_type_id() const override { return "boolean_branch_expression"; }
	virtual String get_type_name() const override { return "Expression Branch"; }
	virtual String get_type_category() const override { return "Condition"; }
	virtual String get_type_description() const override { return "Branches based on conditions which are evaluated as expressions."; }
	virtual String get_type_editor() const override { return "FlowScriptNodeEditorBooleanBranchExpression"; }

	virtual void exec_step(FlowScriptNodeContext *p_context) override;

	virtual void set_json_data(const Dictionary &p_data) override;
	virtual void get_json_data(Dictionary &r_data) const override;

	virtual void get_output_connection_list_lengths(List<int64_t> &r_lengths) const override;

	void set_expression(const String &p_expression);
	String get_expression() const;
};


VARIANT_ENUM_CAST(FlowScriptNodeBooleanBranchExpression::ConnectionList);


#endif // FLOW_SCRIPT_NODE_BOOLEAN_BRANCH_EXPRESSION_HPP
