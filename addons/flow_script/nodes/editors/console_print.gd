@tool
extends FlowNodeEditor


const ConsolePrintNode := preload("../objects/console_print.gd")


@onready var message_box := %"Message Box" as TextEdit
@onready var format_box := %"Format Box" as CodeEdit


func _initialize() -> void:
	for box: Control in [ message_box, format_box ]:
		var box_min_size: Vector2 = box.get_custom_minimum_size()
		box_min_size *= get_editor_scale()
		box_min_size = box_min_size.floor()
		box.set_custom_minimum_size(box_min_size)


func _get_input_slot() -> int:
	return 0


func _set_outgoing_connection(p_connection: FlowNodeEditorOutgoingConnectionParameters) -> void:
	var cp_node: ConsolePrintNode = get_cp_node()
	if cp_node == null:
		return
	
	if p_connection.get_slot() == 0:
		cp_node.set_next_node_id(p_connection.get_node_id())


func _get_outgoing_connections() -> Array[FlowNodeEditorOutgoingConnectionParameters]:
	var connections: Array[FlowNodeEditorOutgoingConnectionParameters] = []
	
	var cp_node: ConsolePrintNode = get_cp_node()
	if cp_node != null:
		if not cp_node.get_next_node_id().is_empty():
			var conn := FlowNodeEditorOutgoingConnectionParameters.new()
			conn.set_node_id(cp_node.get_next_node_id())
			conn.set_slot(0)
			connections.append(conn)
	
	return connections


func _on_node_updated() -> void:
	var cp_node: ConsolePrintNode = get_cp_node()
	if cp_node == null:
		return
	
	if cp_node.get_message_string() != message_box.get_text():
		message_box.set_text(cp_node.get_message_string())
	
	if cp_node.get_format_expressions() != format_box.get_text():
		format_box.set_text(cp_node.get_format_expressions())


func get_cp_node() -> ConsolePrintNode:
	return get_flow_node() as ConsolePrintNode


func _on_message_box_text_changed() -> void:
	var cp_node: ConsolePrintNode = get_cp_node()
	if cp_node != null:
		cp_node.set_message_string(message_box.get_text())


func _on_format_box_text_changed() -> void:
	var cp_node: ConsolePrintNode = get_cp_node()
	if cp_node != null:
		cp_node.set_format_expressions(format_box.get_text())
