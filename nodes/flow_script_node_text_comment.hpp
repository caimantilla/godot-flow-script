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
	virtual void set_json_data(const Dictionary &p_data) override;
	virtual void get_json_data(Dictionary &r_data) const override;

	void set_comment(const String &p_text);
	String get_comment() const;
};


#endif // FLOW_SCRIPT_NODE_TEXT_COMMENT_HPP
