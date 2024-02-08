@tool
extends FlowNode
## Returns the result of some expression. This is optional.
##
## If multiline, a boolean result is returned instead.





var expression: String: set = set_expression_string, get = get_expression_string


var _expression_string: String = ""






func _get_property_list() -> Array[Dictionary]:
	const properties: Array[Dictionary] = [
		{
			"name": "expression",
			"type": TYPE_STRING,
			"hint": PROPERTY_HINT_EXPRESSION,
			"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR,
		},
	]
	
	return properties




func _execute(p_state: FlowNodeState) -> void:
	var results: Array = p_state.get_flow_object().evaluate_multiline_expression(get_expression_string())
	
	# If there are no results, this return was worthless. Notify the user.
	if results.is_empty():
		printerr("FlowScript return statement %s didn't have any valid expressions. What's going on?" % get_id())
		p_state.finish("", null)
	
	else:
		# Switch to boolean check.
		for value: Variant in results:
			if not value:
				p_state.finish("", false)
		
		p_state.finish("", true)




func set_expression_string(p_exp: String) -> void:
	if p_exp != _expression_string:
		_expression_string = p_exp
		emit_changed()

func get_expression_string() -> String:
	return _expression_string
