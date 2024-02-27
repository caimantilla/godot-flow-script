@tool
extends FlowNodeEditor


const ExpressionExecuteNode := preload("../objects/expression_execute.gd")


@onready var expression_box := %"Expression Box" as CodeEdit


func _get_input_slot() -> int:
	return 0


func _set_outgoing_connection(p_connection: FlowNodeEditorOutgoingConnectionParameters) -> void:
	var node := get_ee_node()
	if node == null:
		return
	
	if p_connection.get_slot() == 1:
		node.set_next_node_id(p_connection.get_node_id())


func _get_outgoing_connections() -> Array[FlowNodeEditorOutgoingConnectionParameters]:
	var connections: Array[FlowNodeEditorOutgoingConnectionParameters] = []
	
	var node := get_ee_node()
	if node != null:
		if not node.get_next_node_id().is_empty():
			var conn := FlowNodeEditorOutgoingConnectionParameters.new()
			conn.set_node_id(node.get_next_node_id())
			conn.set_slot(1)
			connections.append(conn)
	
	return connections


func _initialize() -> void:
	var exp_box_size: Vector2 = expression_box.get_custom_minimum_size()
	exp_box_size *= get_editor_scale()
	exp_box_size = exp_box_size.floor()
	expression_box.set_custom_minimum_size(exp_box_size)


func _on_node_updated() -> void:
	var node := get_ee_node()
	if node == null:
		return
	
	if expression_box.get_text() != node.get_expression_string():
		expression_box.set_text(node.get_expression_string())


func get_ee_node() -> ExpressionExecuteNode:
	return get_flow_node() as ExpressionExecuteNode


func _on_expression_box_text_changed() -> void:
	var node := get_ee_node()
	if node != null and expression_box.get_text() != node.get_expression_string():
		node.set_expression_string(expression_box.get_text())
