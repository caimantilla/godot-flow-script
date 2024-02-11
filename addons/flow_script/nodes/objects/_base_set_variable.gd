@tool
extends FlowNode


var next_node_id: String: set = set_next_node_id, get = get_next_node_id
var variable_name: String: set = set_variable_name, get = get_variable_name
var value_expression: String: set = set_value_expression, get = get_value_expression


var _next_node_id: String = ""
var _variable_name: String = ""
var _value_expression: String = ""


func _get_property_list() -> Array[Dictionary]:
	var properties: Array[Dictionary] = [
		{
			"name": get_type_name(),
			"type": TYPE_NIL,
			"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_CATEGORY,
		},
		{
			"name": "next_node_id",
			"type": TYPE_STRING,
			"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY,
		},
		{
			"name": "variable_name",
			"type": TYPE_STRING,
			"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR,
		},
		{
			"name": "value_expression",
			"type": TYPE_STRING,
			"hint": PROPERTY_HINT_EXPRESSION,
			"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR,
		},
	]
	
	return properties


func _is_property_flow_node_reference(p_name: StringName) -> bool:
	return (p_name == &"next_node_id")


func _on_external_node_renamed(p_from: String, p_to: String) -> void:
	if p_from == _next_node_id:
		_next_node_id = p_to


func set_next_node_id(p_id: String) -> void:
	if p_id != _next_node_id:
		_next_node_id = p_id
		emit_changed()

func get_next_node_id() -> String:
	return _next_node_id


func set_variable_name(p_name: String) -> void:
	if p_name != _variable_name:
		_variable_name = p_name
		emit_changed()

func get_variable_name() -> String:
	return _variable_name


func set_value_expression(p_expression: String) -> void:
	if p_expression != _value_expression:
		_value_expression = p_expression
		emit_changed()

func get_value_expression() -> String:
	return _value_expression
