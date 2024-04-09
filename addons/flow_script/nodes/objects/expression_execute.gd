@tool
extends FlowNode
## Executes any number of expressions immediately.
## No branching is done, this is only to call methods, modify the game state...
## See expression_branch for branching on expressions.


## The ID of the next node to be executed.
@export_custom(PROPERTY_HINT_NONE, "", PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_READ_ONLY)
var next_node_id: String = "": set = set_next_node_id, get = get_next_node_id

## The all of the expressions which should be executed, separated by line.
@export_custom(PROPERTY_HINT_EXPRESSION, "", PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_DEFAULT)
var expression_list: String = "": set = set_expression_string, get = get_expression_string


var _expression_string: String
var _next_node_id: String


func _is_property_flow_node_reference(p_property_name: StringName) -> bool:
	return (p_property_name == &"next_node_id")


func _on_external_node_renamed(p_from: String, p_to: String) -> void:
	if p_from == _next_node_id:
		_next_node_id = p_to


func _execute(p_state: FlowNodeState) -> void:
	# Just execute, whatever gets returned is irrelevant
	p_state.get_flow_object().evaluate_multiline_expression(get_expression_string())
	p_state.finish(get_next_node_id())


func set_next_node_id(p_id: String) -> void:
	_next_node_id = p_id

func get_next_node_id() -> String:
	return _next_node_id


func set_expression_string(p_string: String) -> void:
	_expression_string = p_string

func get_expression_string() -> String:
	return _expression_string
