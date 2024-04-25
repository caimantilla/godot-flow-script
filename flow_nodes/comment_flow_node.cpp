#include "comment_flow_node.hpp"


void CommentFlowNode::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_comment_no_signal", "comment"), &CommentFlowNode::set_comment_no_signal);
	ClassDB::bind_method(D_METHOD("set_comment", "comment"), &CommentFlowNode::set_comment);
	ClassDB::bind_method(D_METHOD("get_comment"), &CommentFlowNode::get_comment);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "comment", PROPERTY_HINT_MULTILINE_TEXT, "", PROPERTY_USAGE_DEFAULT), "set_comment", "get_comment");
}


void CommentFlowNode::set_comment_no_signal(const String &p_comment)
{
	comment = p_comment;
}


void CommentFlowNode::set_comment(const String &p_comment)
{
	if (p_comment == comment)
	{
		return;
	}

	comment = p_comment;
	emit_changed();
}


String CommentFlowNode::get_comment() const
{
	return comment;
}
