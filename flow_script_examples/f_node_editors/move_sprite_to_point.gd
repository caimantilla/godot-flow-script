@tool
extends FlowNodeEditor


const MoveSpriteToPointNode = preload("../f_nodes/move_sprite_to_point.gd")


@export var sprite_name_edit: LineEdit
@export var spinbox_x: SpinBox
@export var spinbox_y: SpinBox


func _get_input_slot():
	return 0


func _set_outgoing_connection(connection):
	if flow_node == null:
		return
	
	if connection.slot == 0:
		flow_node.next_node_id = connection.node_id


func _get_outgoing_connections():
	var connections = []
	
	if flow_node != null and not flow_node.next_node_id.is_empty():
		var connection = FlowNodeEditorOutgoingConnectionParameters.new()
		connection.node_id = flow_node.next_node_id
		connection.slot = 0
		connections.append(connection)
	
	return connections


func _on_node_updated():
	spinbox_x.set_value_no_signal(flow_node.destination.x)
	spinbox_y.set_value_no_signal(flow_node.destination.y)
	
	if flow_node.sprite_name != sprite_name_edit.text:
		sprite_name_edit.text = flow_node.sprite_name


func _on_sprite_name_text_changed(new_text: String) -> void:
	if flow_node != null:
		flow_node.sprite_name = new_text


func _on_x_value_changed(value: float) -> void:
	if flow_node != null:
		flow_node.destination.x = value


func _on_y_value_changed(value: float) -> void:
	if flow_node != null:
		flow_node.destination.y = value
