@tool
extends "_multi_branch_execute.gd"


func _execute(p_state: FlowNodeState) -> void:
	p_state.resumed.connect(_on_state_resumed, CONNECT_ONE_SHOT)
	p_state.request_new_threads(get_execution_stack().duplicate(), true)


func _on_state_resumed(p_state: FlowNodeState) -> void:
	p_state.finish(get_next_node_id(), null)
