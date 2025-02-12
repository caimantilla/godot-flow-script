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
	virtual void set_data_state(const Dictionary &p_data) override;
	virtual Dictionary get_data_state() const override;

	void set_comment(const String &p_text);
	String get_comment() const;
};


#endif // FLOW_SCRIPT_NODE_TEXT_COMMENT_HPP
