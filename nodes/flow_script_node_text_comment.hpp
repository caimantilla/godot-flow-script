#ifndef FLOW_SCRIPT_NODE_TEXT_COMMENT_HPP
#define FLOW_SCRIPT_NODE_TEXT_COMMENT_HPP


#include "common.hpp"


class FlowScriptNodeTextComment : public FlowScriptNode
{
	GDCLASS(FlowScriptNodeTextComment, FlowScriptNode);

private:
	String comment;

protected:
	static void _bind_methods();

public:
	virtual String get_type_id() const override { return "text_comment"; }
	virtual String get_type_name() const override { return "Comment"; }
	virtual String get_type_category() const override { return ""; }
	virtual String get_type_description() const override { return "A field to take notes in."; }
	virtual String get_type_editor() const override { return "FlowScriptNodeEditorTextComment"; }

	virtual void set_json_data(const Dictionary &p_data) override;
	virtual void get_json_data(Dictionary &r_data) const override;

	void set_comment(const String &p_text);
	String get_comment() const;
};


#endif // FLOW_SCRIPT_NODE_TEXT_COMMENT_HPP
