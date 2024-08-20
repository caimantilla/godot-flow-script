#include "flow_script_node_editor_text_comment.hpp"
#include "../../nodes/flow_script_node_text_comment.hpp"


void FlowScriptNodeEditorTextComment::sync()
{
	FlowScriptNodeTextComment *node = Object::cast_to<FlowScriptNodeTextComment>(get_edited_node_ptr());
	// no reason to keep right edge but maybe left edge should be stripped?
	// though that depends on the user's language...
	comment_box->set_text(node->get_comment().strip_edges(false, true));
}


FlowScriptNodeEditorTextComment::FlowScriptNodeEditorTextComment()
{
	comment_box = memnew(RichTextLabel);
	comment_box->set_fit_content(true);
	comment_box->set_scroll_active(false);
	comment_box->set_auto_translate(false);
	comment_box->set_use_bbcode(true);
	add_child(comment_box);
}
