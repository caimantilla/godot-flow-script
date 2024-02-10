@tool
extends FlowNode


const MIN_SECONDS: float = 0.01
const MAX_SECONDS: float = 999.0
const SECONDS_STEP: float = 0.01


@export_category("Wait Seconds")

@export_range(MIN_SECONDS, MAX_SECONDS, SECONDS_STEP) var seconds: float: set = set_seconds, get = get_seconds
@export var next_node_id: String: set = set_next_node_id, get = get_next_node_id


var _seconds: float = 1.0
var _next_node_id: String = ""


func _is_property_flow_node_reference(p_property: StringName) -> bool:
	return (p_property == &"next_node_id")


func _on_external_node_renamed(p_from: String, p_to: String) -> void:
	if p_from == _next_node_id:
		_next_node_id = p_to


func _execute(p_state: FlowNodeState) -> void:
	if _seconds > 0.0:
		var timer: Object = p_state.get_flow_object().create_seconds_timer(_seconds)
		if timer != null:
			timer.timeout.connect(_on_timeout.bind(p_state))
			return
	
	p_state.finish(_next_node_id, null)


func _on_timeout(p_state: FlowNodeState) -> void:
	p_state.finish(_next_node_id, null)


func set_seconds(p_secs: float) -> void:
	p_secs = clampf(p_secs, MIN_SECONDS, MAX_SECONDS)
	if not is_equal_approx(p_secs, _seconds):
		_seconds = p_secs
		emit_changed()

func get_seconds() -> float:
	return _seconds


func set_next_node_id(p_id: String) -> void:
	if p_id != _next_node_id:
		_next_node_id = p_id
		emit_changed()

func get_next_node_id() -> String:
	return _next_node_id
