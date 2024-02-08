@tool
extends FlowNodeEditor



const ExpressionBranchNode := preload("../objects/expression_branch.gd")



@onready var expression_box = $"Expression Box" as CodeEdit



func _initialize() -> void:
	if Engine.is_editor_hint():
		var expression_box_min_size: Vector2 = expression_box.get_custom_minimum_size()
		expression_box_min_size *= EditorInterface.get_editor_scale()
		expression_box_min_size = expression_box_min_size.floor()
		
		expression_box.set_custom_minimum_size(expression_box_min_size)


func _on_node_updated() -> void:
	var node = get_flow_node() as ExpressionBranchNode
	if node != null:
		
		if node.get_expressions() != expression_box.get_text():
			expression_box.set_text(node.get_expressions())


func _get_input_slot() -> int:
	return 1



func _set_outgoing_connection(p_outgoing_connection: FlowNodeEditorOutgoingConnectionParameters) -> void:
	
	if p_outgoing_connection.get_slot() == 0:
		set_next_id_true(p_outgoing_connection.get_node_id())
	
	elif p_outgoing_connection.get_slot() == 2:
		set_next_id_false(p_outgoing_connection.get_node_id())


func _get_outgoing_connections() -> Array[FlowNodeEditorOutgoingConnectionParameters]:
	var connections: Array[FlowNodeEditorOutgoingConnectionParameters] = []
	
	var true_id: String = get_next_id_true()
	var false_id: String = get_next_id_false()
	
	if not true_id.is_empty():
		var conn := FlowNodeEditorOutgoingConnectionParameters.new()
		conn.set_slot(0)
		conn.set_node_id(get_next_id_true())
		connections.append(conn)
	
	if not false_id.is_empty():
		var conn := FlowNodeEditorOutgoingConnectionParameters.new()
		conn.set_slot(2)
		conn.set_node_id(get_next_id_false())
		connections.append(conn)
	
	return connections



func set_next_id_true(p_id: String) -> void:
	(get_flow_node() as ExpressionBranchNode).set_true_id(p_id)

func get_next_id_true() -> String:
	return (get_flow_node() as ExpressionBranchNode).get_true_id()


func set_next_id_false(p_id: String) -> void:
	(get_flow_node() as ExpressionBranchNode).set_false_id(p_id)


func get_next_id_false() -> String:
	return (get_flow_node() as ExpressionBranchNode).get_false_id()


func _on_expression_box_text_changed() -> void:
	var node = get_flow_node() as ExpressionBranchNode
	if node != null:
		
		node.set_expressions(expression_box.get_text())
