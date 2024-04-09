@tool
extends FlowNode


@export_category("Expression Branch")

## The list of expressions to check.
@export_custom(PROPERTY_HINT_EXPRESSION, "", PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_DEFAULT)
var expression_string: String: set = set_expressions, get = get_expressions

## The next node to execute if the expression evaluates to true.
@export_custom(PROPERTY_HINT_NONE, "", PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY)
var next_node_id_true: String: set = set_true_id, get = get_true_id

## The next node to execute if the expression evaluates to false.
@export_custom(PROPERTY_HINT_NONE, "", PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY)
var next_node_id_false: String: set = set_false_id, get = get_false_id


var _expression_string: String = ""
var _next_node_id_true: String = ""
var _next_node_id_false: String = ""


func _get_property_list() -> Array[Dictionary]:
	const properties: Array[Dictionary] = [
		{
			"name": "expression_string",
			"type": TYPE_STRING,
			"hint": PROPERTY_HINT_EXPRESSION,
			"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR,
		},
		{
			"name": "next_node_id_true",
			"type": TYPE_STRING,
			"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY,
		},
		{
			"name": "next_node_id_false",
			"type": TYPE_STRING,
			"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY,
		},
	]
	
	return properties


func _execute(state: FlowNodeState) -> void:
	var result: bool = state.get_flow_object().evaluate_multiline_boolean_expression(get_expressions())
	
	var next_id: String
	if result:
		next_id = get_true_id()
	else:
		next_id = get_false_id()
	
	state.finish(next_id)


func _is_property_flow_node_reference(p_property: StringName) -> bool:
	return (p_property == &"next_node_id_true" or p_property == &"next_node_id_false")


func _on_external_node_renamed(p_from: String, p_to: String) -> void:
	if p_from == _next_node_id_true:
		_next_node_id_true = p_to
	
	if p_from == _next_node_id_false:
		_next_node_id_false = p_to


func set_expressions(p_string: String) -> void:
	if p_string != _expression_string:
		_expression_string = p_string
		emit_changed()

func get_expressions() -> String:
	return _expression_string


func set_true_id(p_node_id: String) -> void:
	if p_node_id != _next_node_id_true:
		_next_node_id_true = p_node_id
		emit_changed()

func get_true_id() -> String:
	return _next_node_id_true


func set_false_id(p_node_id: String) -> void:
	if p_node_id != _next_node_id_false:
		_next_node_id_false = p_node_id
		emit_changed()

func get_false_id() -> String:
	return _next_node_id_false
