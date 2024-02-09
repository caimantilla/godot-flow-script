@tool
extends FlowNode
## Marks a flow procedure's starting point.
## Its name is defined by the user.


var next_node_id: String: set = set_next_node_id, get = get_next_node_id


var _next_node_id: String = ""


func _get_property_list() -> Array[Dictionary]:
	const properties: Array[Dictionary] = [
		{
			"name": "Procedure",
			"type": TYPE_NIL,
			"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_CATEGORY,
		},
		{
			"name": "next_node_id",
			"type": TYPE_STRING,
			"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY,
		},
	]
	
	return properties


func _is_property_flow_node_reference(p_name: StringName) -> bool:
	return (p_name == &"next_node_id")


func _on_external_node_renamed(p_from: String, p_to: String) -> void:
	if _next_node_id == p_from:
		_next_node_id = p_to


func _execute(p_state: FlowNodeState) -> void:
	p_state.finish(_next_node_id, null)


func set_next_node_id(p_id: String) -> void:
	if p_id != _next_node_id:
		_next_node_id = p_id
		emit_changed()

func get_next_node_id() -> String:
	return _next_node_id
