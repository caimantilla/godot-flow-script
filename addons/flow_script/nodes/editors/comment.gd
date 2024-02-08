@tool
extends FlowNodeEditor



const Comment := preload("../objects/comment.gd")



@onready var comment_box := $"Comment Box" as TextEdit



func _initialize() -> void:
	if Engine.is_editor_hint():
		var cb_min_size: Vector2 = comment_box.get_custom_minimum_size()
		cb_min_size *= EditorInterface.get_editor_scale()
		cb_min_size = cb_min_size.floor()
		comment_box.set_custom_minimum_size(cb_min_size)


func _on_node_updated() -> void:
	var node = get_flow_node() as Comment
	if node != null:
		
		if node.get_comment_string() != comment_box.get_text():
			comment_box.set_text(node.get_comment_string())


func _on_comment_box_text_changed() -> void:
	var node = get_flow_node() as Comment
	if node != null:
		
		node.set_comment_string(comment_box.get_text())
