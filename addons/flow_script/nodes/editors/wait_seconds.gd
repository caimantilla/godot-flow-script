@tool
extends FlowNodeEditor


const WaitSecondsNode := preload("../objects/wait_seconds.gd")


@onready var spinbox_seconds = $"Seconds Spinner"


func _ready() -> void:
	super()
	
	spinbox_seconds.set_min(WaitSecondsNode.MIN_SECONDS)
	spinbox_seconds.set_max(WaitSecondsNode.MAX_SECONDS)
	spinbox_seconds.set_step(WaitSecondsNode.SECONDS_STEP)


func _initialize() -> void:
	var seconds_spinner_size: Vector2 = spinbox_seconds.get_custom_minimum_size()
	seconds_spinner_size *= get_editor_scale()
	seconds_spinner_size = seconds_spinner_size.floor()
	spinbox_seconds.set_custom_minimum_size(seconds_spinner_size)


func _on_node_updated() -> void:
	_update_seconds_spinner()


func _get_input_slot() -> int:
	return 0


func _set_outgoing_connection(p_connection: FlowNodeEditorOutgoingConnectionParameters) -> void:
	var ws_node: WaitSecondsNode = get_ws_node()
	if ws_node == null:
		return
	
	if p_connection.get_slot() == 0:
		ws_node.set_next_node_id(p_connection.get_node_id())


func _get_outgoing_connections() -> Array[FlowNodeEditorOutgoingConnectionParameters]:
	var connections: Array[FlowNodeEditorOutgoingConnectionParameters] = []
	
	var ws_node: WaitSecondsNode = get_ws_node()
	if ws_node != null:
		if not ws_node.get_next_node_id().is_empty():
			var conn := FlowNodeEditorOutgoingConnectionParameters.new()
			conn.set_node_id(ws_node.get_next_node_id())
			conn.set_slot(0)
			connections.append(conn)
	
	return connections


func get_ws_node() -> WaitSecondsNode:
	return get_flow_node() as WaitSecondsNode


func _update_seconds_spinner() -> void:
	var ws_node: WaitSecondsNode = get_ws_node()
	if ws_node != null:
		var seconds: float = ws_node.get_seconds()
		
		if not is_equal_approx(spinbox_seconds.get_value(), seconds):
			spinbox_seconds.set_value_no_signal(seconds)
		
		if is_equal_approx(seconds, 1.0):
			spinbox_seconds.set_suffix("Second")
		else:
			spinbox_seconds.set_suffix("Seconds")


func _on_seconds_spinner_value_changed(p_value: float) -> void:
	var ws_node: WaitSecondsNode = get_ws_node()
	if ws_node != null:
		ws_node.set_seconds(p_value)
