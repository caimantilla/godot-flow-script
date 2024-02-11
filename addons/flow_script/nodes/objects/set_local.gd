@tool
extends "_base_set_variable.gd"
## Assigns a new value to a local variable.
## This can also be used to initialize a variable, useful in something like a loop.


func _execute(p_state: FlowNodeState) -> void:
	
	var expression_result: Variant = p_state.get_flow_object().evaluate_expression(get_value_expression())
	p_state.get_flow_object().set_local(get_variable_name(), expression_result)
	
	p_state.finish(get_next_node_id())
