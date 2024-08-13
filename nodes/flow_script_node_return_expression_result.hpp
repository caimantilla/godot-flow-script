#ifndef FLOW_SCRIPT_NODE_RETURN_EXPRESSION_RESULT_HPP
#define FLOW_SCRIPT_NODE_RETURN_EXPRESSION_RESULT_HPP


#include "common.hpp"


class FlowScriptNodeReturnExpressionResult : public FlowScriptNode
{
	GDCLASS(FlowScriptNodeReturnExpressionResult, FlowScriptNode);

private:
	String expression;

protected:
	static void _bind_methods();

public:
	virtual String get_type_id() const override { return "return_expression_result"; }
	virtual String get_type_name() const override { return "Evaluate and Return"; }
	virtual String get_type_category() const override { return "Expression"; }
	virtual String get_type_description() const override { return "Returns the result of an expression evaluation to the caller."; }
	virtual String get_type_editor() const override { return "FlowScriptNodeEditorReturnExpressionResult"; }

	virtual void exec_step(FlowScriptNodeContext *p_context) override;

	virtual void set_json_data(const Dictionary &p_data) override;
	virtual void get_json_data(Dictionary &r_data) const override;

	void set_expression(const String &p_text);
	String get_expression() const;
};


#endif // FLOW_SCRIPT_NODE_RETURN_EXPRESSION_RESULT_HPP
