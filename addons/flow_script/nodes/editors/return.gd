@tool
extends FlowNodeEditor



const Return := preload("../objects/return.gd")




const EXPRESSION_BOX_WIDTH: float = 200.0




@onready var expression_box := $Expression as CodeEdit




func _on_node_updated() -> void:
	var node := get_flow_node() as Return
	if node != null:
		if node.get_expression_string() != expression_box.get_text():
			expression_box.set_text(node.get_expression_string())


func _get_input_slot() -> int:
	return 0




func _on_expression_box_text_changed() -> void:
	var node := get_flow_node() as Return
	if node != null:
		node.set_expression_string(expression_box.get_text())
