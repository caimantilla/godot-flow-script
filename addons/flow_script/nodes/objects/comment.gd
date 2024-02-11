@tool
extends FlowNode


var comment: String: set = set_comment_string, get = get_comment_string


var _comment_string: String = ""


func _get_property_list() -> Array[Dictionary]:
	const properties: Array[Dictionary] = [
		{
			"name": "comment",
			"type": TYPE_STRING,
			"hint": PROPERTY_HINT_MULTILINE_TEXT,
			"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR,
		},
	]
	
	return properties


func set_comment_string(p_text: String) -> void:
	if p_text != _comment_string:
		_comment_string = p_text
		emit_changed()

func get_comment_string() -> String:
	return _comment_string
