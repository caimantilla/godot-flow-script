@tool
extends "_multi_branch_execute.gd"


func _execute(p_state: FlowNodeState) -> void:
	p_state.set_temporary_variable("current_step", -1)
	_do_next_step(p_state)


func _do_next_step(p_state: FlowNodeState) -> void:
	var current_step: int = p_state.get_temporary_variable("current_step")
	current_step += 1
	p_state.set_temporary_variable("current_step", current_step)
	
	if current_step >= get_execution_stack_size():
		p_state.finish(get_next_node_id(), null)
	else:
		var next_step_node_id: String = get_execution_stack_entry_at(current_step)
		
		p_state.resumed.connect(_do_next_step.bind(p_state), CONNECT_ONE_SHOT)
		p_state.request_new_thread(next_step_node_id, true)
