#include "comment_flow_node_editor.hpp"
#include "scene/gui/label.h"


CommentFlowNode *CommentFlowNodeEditor::get_comment_flow_node() const
{
	return Object::cast_to<CommentFlowNode>(get_flow_node());
}


void CommentFlowNodeEditor::_clean_up()
{
	comment_box->set_text("");
}


void CommentFlowNodeEditor::_flow_node_updated()
{
	comment_box->set_text(get_comment_flow_node()->get_comment());
}


CommentFlowNodeEditor::CommentFlowNodeEditor()
{
	comment_box = memnew(Label);
	comment_box->set_autowrap_mode(TextServer::AUTOWRAP_OFF);
	comment_box->set_h_size_flags(SIZE_EXPAND_FILL);
	comment_box->set_v_size_flags(SIZE_EXPAND_FILL);
	comment_box->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	comment_box->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	add_child(comment_box);
}
