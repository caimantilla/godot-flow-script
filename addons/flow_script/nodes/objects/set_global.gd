@tool
extends "_base_set_variable.gd"
## Assigns a new value to a global variable.


func _execute(p_state: FlowNodeState) -> void:
	
	var result: Variant = p_state.get_flow_object().evaluate_expression(get_value_expression())
	p_state.get_flow_object().set_global(get_variable_name(), result)
	
	p_state.finish(get_next_node_id())
