@tool
extends FlowNode


## The expression to evaluate before each loop.
## If it is empty, the expression will evaluate to false to ensure that an infinite loop can only occur deliberately.
var expression: String: set = set_expression_string, get = get_expression_string

## The branch which is executed repeatedly for as long as the expression evaluates to true.
var next_node_id_true: String: set = set_next_node_id_true, get = get_next_node_id_true

## The branch which the loop breaks out to.
var next_node_id_false: String: set = set_next_node_id_false, get = get_next_node_id_false


var _expression_string: String = ""
var _next_node_id_true: String = ""
var _next_node_id_false: String = ""


func _get_property_list() -> Array[Dictionary]:
	var properties: Array[Dictionary] = [
		{
			"name": get_type_name(),
			"type": TYPE_NIL,
			"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_CATEGORY,
		},
		{
			"name": "expression",
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


func _execute(p_state: FlowNodeState) -> void:
	p_state.resumed.connect(_while_check.bind(p_state))
	_while_check(p_state)


func _is_property_flow_node_reference(p_property: StringName) -> bool:
	return (p_property == &"next_node_id_true") or (p_property == &"next_node_id_false")


func _on_external_node_renamed(p_from: String, p_to: String) -> void:
	if p_from == _next_node_id_true:
		_next_node_id_true = p_to
	
	if p_from == _next_node_id_false:
		_next_node_id_false = p_to


func _while_check(p_state: FlowNodeState) -> void:
	# Don't evaluate to true if there are no conditions in the first place.
	# If you want an infinite loop, like for the idle animation of a non-interactable entity, just write "true" as the expression.
	var result: bool = p_state.get_flow_object().evaluate_multiline_boolean_expression(_expression_string, false)
	
	if result:
		p_state.request_new_thread(_next_node_id_true, true)
	else:
		p_state.finish(_next_node_id_false, null)


func set_expression_string(p_exp: String) -> void:
	if p_exp != _expression_string:
		_expression_string = p_exp
		emit_changed()

func get_expression_string() -> String:
	return _expression_string


func set_next_node_id_true(p_id: String) -> void:
	if p_id != _next_node_id_true:
		_next_node_id_true = p_id
		emit_changed()

func get_next_node_id_true() -> String:
	return _next_node_id_true


func set_next_node_id_false(p_id: String) -> void:
	if p_id != _next_node_id_false:
		_next_node_id_false = p_id
		emit_changed()

func get_next_node_id_false() -> String:
	return _next_node_id_false
