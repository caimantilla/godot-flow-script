@tool
extends FlowNodeEditor


const WhileNode := preload("../objects/while_expression_loop.gd")


@onready var expression_box := %"Expression Box" as CodeEdit


func _initialize() -> void:
	if Engine.is_editor_hint():
		expression_box.set_custom_minimum_size( (expression_box.get_custom_minimum_size() * EditorInterface.get_editor_scale()).floor() )


func _on_node_updated() -> void:
	var while_node: WhileNode = get_while_node()
	if while_node == null:
		return
	
	if expression_box.get_text() != while_node.get_expression_string():
		expression_box.set_text(while_node.get_expression_string())


func _get_input_slot() -> int:
	return 1


func _set_outgoing_connection(p_param: FlowNodeEditorOutgoingConnectionParameters) -> void:
	var while_node: WhileNode = get_while_node()
	if while_node == null:
		return
	
	match p_param.get_slot():
		0:
			while_node.set_next_node_id_true(p_param.get_node_id())
		2:
			while_node.set_next_node_id_false(p_param.get_node_id())


func _get_outgoing_connections() -> Array[FlowNodeEditorOutgoingConnectionParameters]:
	var connections: Array[FlowNodeEditorOutgoingConnectionParameters] = []
	
	var while_node: WhileNode = get_while_node()
	
	if while_node != null:
		
		if not while_node.get_next_node_id_true().is_empty():
			var conn := FlowNodeEditorOutgoingConnectionParameters.new()
			conn.set_slot(0)
			conn.set_node_id(while_node.get_next_node_id_true())
			connections.append(conn)
		
		if not while_node.get_next_node_id_false().is_empty():
			var conn := FlowNodeEditorOutgoingConnectionParameters.new()
			conn.set_slot(2)
			conn.set_node_id(while_node.get_next_node_id_false())
			connections.append(conn)
	
	return connections


func get_while_node() -> WhileNode:
	return get_flow_node() as WhileNode


func _on_expression_box_text_changed() -> void:
	var while_node: WhileNode = get_while_node()
	if while_node != null:
		while_node.set_expression_string(expression_box.get_text())
