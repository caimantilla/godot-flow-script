@tool
extends FlowNode


@export_multiline var comment: String: set = set_comment_string, get = get_comment_string


var _comment_string: String = ""


func set_comment_string(p_text: String) -> void:
	if p_text != _comment_string:
		_comment_string = p_text
		emit_changed()

func get_comment_string() -> String:
	return _comment_string
