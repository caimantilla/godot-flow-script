#include "flow_script_node_text_comment.hpp"


void FlowScriptNodeTextComment::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_comment", "text"), &FlowScriptNodeTextComment::set_comment);
	ClassDB::bind_method(D_METHOD("get_comment"), &FlowScriptNodeTextComment::get_comment);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "comment", PROPERTY_HINT_MULTILINE_TEXT), "set_comment", "get_comment");
}


void FlowScriptNodeTextComment::set_data_state(const Dictionary &p_data)
{
	set_comment(p_data.get("comment", String()));
}


Dictionary FlowScriptNodeTextComment::get_data_state() const
{
	Dictionary d;
	d["comment"] = comment;
	return d;
}


void FlowScriptNodeTextComment::set_comment(const String &p_text)
{
	if (p_text == comment)
	{
		return;
	}
	comment = p_text;
	emit_changed();
}


String FlowScriptNodeTextComment::get_comment() const
{
	return comment;
}
