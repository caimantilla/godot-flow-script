@tool
extends FlowNode
## Executes any number of expressions immediately.
## No branching is done, this is only to call methods, modify the game state...
## See expression_branch for branching on expressions.


## The ID of the next node to be executed.
var next_node_id: String: set = set_next_node_id, get = get_next_node_id

## The all of the expressions which should be executed, separated by line.
var expression_list: String: set = set_expression_string, get = get_expression_string


var _expression_string: String = ""
var _next_node_id: String = ""


func _get_property_list() -> Array[Dictionary]:
	const properties: Array[Dictionary] = [
		{
			"name": "next_node_id",
			"type": TYPE_STRING,
			"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY,
		},
		{
			"name": "expression_list",
			"type": TYPE_STRING,
			"hint": PROPERTY_HINT_EXPRESSION,
			"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR,
		},
	]
	
	return properties


func _is_property_flow_node_reference(p_property_name: StringName) -> bool:
	return (p_property_name == &"next_node_id")


func _on_external_node_renamed(p_from: String, p_to: String) -> void:
	if p_from == _next_node_id:
		_next_node_id = p_to


func _execute(p_state: FlowNodeState) -> void:
	# Just execute, whatever gets returned is irrelevant
	p_state.get_flow_object().evaluate_multiline_expression(_expression_string)
	p_state.finish(_next_node_id)


func set_next_node_id(p_id: String) -> void:
	_next_node_id = p_id

func get_next_node_id() -> String:
	return _next_node_id


func set_expression_string(p_string: String) -> void:
	_expression_string = p_string

func get_expression_string() -> String:
	return _expression_string
