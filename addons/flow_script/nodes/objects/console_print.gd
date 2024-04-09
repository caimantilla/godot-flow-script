@tool
extends FlowNode


## The ID of the next node to execute.
@export_custom(PROPERTY_HINT_NONE, "", PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY)
var next_node_id: String: set = set_next_node_id, get = get_next_node_id

## The message to print.
@export_multiline() var message_string: String: set = set_message_string, get = get_message_string

## If the message should print anything relevant to the game state, expressions can be used.
## Expressions are separated by line.
@export_custom(PROPERTY_HINT_EXPRESSION, "", PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_DEFAULT)
var format_expressions: String: set = set_format_expressions, get = get_format_expressions


var _next_node_id: String = ""
var _message_string: String = ""
var _format_expressions: String = ""


func _is_property_flow_node_reference(p_property: StringName) -> bool:
	return (p_property == &"next_node_id")


func _on_external_node_renamed(p_from: String, p_to: String) -> void:
	if p_from == _next_node_id:
		_next_node_id = p_to


func _execute(p_state: FlowNodeState) -> void:
	var expression_results: Array = p_state.get_flow_object().evaluate_multiline_expression(_format_expressions)
	
	var string_to_print: String
	if expression_results.is_empty():
		string_to_print = _message_string
	else:
		string_to_print = _message_string % expression_results
	
	p_state.get_flow_object().console_print(string_to_print)
	p_state.finish(_next_node_id, null)


func set_next_node_id(p_id: String) -> void:
	if p_id != _next_node_id:
		_next_node_id = p_id
		emit_changed()

func get_next_node_id() -> String:
	return _next_node_id


func set_message_string(p_str: String) -> void:
	if p_str != _message_string:
		_message_string = p_str
		emit_changed()

func get_message_string() -> String:
	return _message_string


func set_format_expressions(p_exp_list: String) -> void:
	if p_exp_list != _format_expressions:
		_format_expressions = p_exp_list
		emit_changed()

func get_format_expressions() -> String:
	return _format_expressions
