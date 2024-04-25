#ifndef COMMENT_FLOW_NODE_HPP
#define COMMENT_FLOW_NODE_HPP

#include "../core_types/flow_node.hpp"

class CommentFlowNode final : public FlowNode
{
	GDCLASS(CommentFlowNode, FlowNode);

private:
	String comment;

protected:
	static void _bind_methods();

public:
	void set_comment_no_signal(const String &p_comment);
	void set_comment(const String &p_comment);
	String get_comment() const;
};

#endif
