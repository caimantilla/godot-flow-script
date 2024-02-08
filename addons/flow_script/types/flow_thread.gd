@tool
class_name FlowThread
extends Node



## Emitted when the thread is finished, ie. there are no longer any nodes executing.
signal finished(return_value: Variant)

## Emitted when a new thread should be made.
## The ID of the node to begin that thread with is passed as a parameter.
signal new_threads_requested(initial_node_ids: PackedStringArray, wait_for_completion: bool)


## The thread's own ID.
var thread_id: String: get = get_thread_id

## Reference to the flow object which flow nodes should have access to.
var flow_object: FlowObject: get = get_flow_object

## Reference to the flow script containing the flow nodes.
var flow_script: FlowScript: get = get_flow_script

## Reference to the current node's state.
## Signals to progress, create threads, etc. go here.
var current_flow_node_state: FlowNodeState: get = get_current_flow_node_state

## The list of threads which must be finished for the current node to resume execution.
var resume_dependent_threads_ids: PackedStringArray: get = get_resume_dependent_threads_ids


var _thread_id: String = ""
var _flow_object: FlowObject = null
var _flow_script: FlowScript = null
var _current_flow_node: FlowNode = null
var _current_flow_node_state: FlowNodeState = null
var _resume_dependent_threads_ids := PackedStringArray()


func _init(p_thread_id: String, p_flow_object: FlowObject, p_flow_script: FlowScript) -> void:
	assert(not p_thread_id.is_empty())
	assert(is_instance_valid(p_flow_object), "Invalid FlowObject passed to FlowThread constructor.")
	assert(p_flow_script != null, "No FlowScript passed to FlowThread constructor.")
	
	_thread_id = p_thread_id
	_flow_object = p_flow_object
	_flow_script = p_flow_script


func get_thread_id() -> String:
	return _thread_id


func get_flow_object() -> FlowObject:
	return _flow_object


func get_flow_script() -> FlowScript:
	return _flow_script


func get_current_flow_node_state() -> FlowNodeState:
	return _current_flow_node_state


func get_resume_dependent_threads_ids() -> PackedStringArray:
	return _resume_dependent_threads_ids


## Call to add a thread which must finish for this thread to resume execution.
func add_resume_dependent_thread(p_thread_id: String) -> void:
	_resume_dependent_threads_ids.push_back(p_thread_id)


func is_finished() -> bool:
	return (_current_flow_node_state == null)


func is_active() -> bool:
	return not is_finished()


func kill() -> void:
	pass


## Begins execution of a node.
func start(p_initial_node_id: String) -> void:
	_execute_node(p_initial_node_id)


## Notifies the thread that some other thread has finished execution.
## This lets the thread know if it's time to resume in the case that it's dependent on other threads to resume execution.
func on_external_thread_finished(p_thread_id: String) -> void:
	var thread_list_idx: int = _resume_dependent_threads_ids.find(p_thread_id)
	if thread_list_idx > -1:
		_resume_dependent_threads_ids.remove_at(thread_list_idx)
		
		# Resume in this case where the thread is no longer dependent on the completion of any others.
		if _resume_dependent_threads_ids.is_empty():
			if _current_flow_node_state == null:
				printerr("Lost flow node state ref...?!??!?!?!??!!?? WGHAT")
				_emit_finished(null)
			else:
				_current_flow_node_state.resume()


func _execute_node(p_node_id: String) -> void:
	if _current_flow_node_state != null:
		_current_flow_node_state.free()
	
	_current_flow_node_state = null
	_current_flow_node = null
	
	if not _flow_script.has_node_with_id(p_node_id):
		printerr('Unable to execute non-existent FlowNode "%s" in thread "%s".' % [ p_node_id, _thread_id ])
		_emit_finished(null)
		return
	
	_current_flow_node_state = FlowNodeState.new(_flow_object)
	_current_flow_node_state.finished.connect(_on_node_finished)
	_current_flow_node_state.new_threads_requested.connect(_on_new_threads_requested)
	
	_current_flow_node = _flow_script.get_node_by_id(p_node_id)
	_current_flow_node.execute(_current_flow_node_state)


func _on_node_finished(p_next_node_id: String, p_return_value: Variant) -> void:
	var should_thread_finish: bool = false
	
	if p_next_node_id.is_empty():
		should_thread_finish = true
	
	elif not _flow_script.has_node_with_id(p_next_node_id):
		should_thread_finish = true
		printerr('Invalid connection in FlowScript at "%s" from node "%s" to non-existent node "%s". Terminating execution of thread "%s".' % [ _flow_script.get_path(), _current_flow_node.get_id(), p_next_node_id, _thread_id ])
	
	
	if should_thread_finish:
		_emit_finished(p_return_value)
	else:
		_execute_node(p_next_node_id)


func _on_new_threads_requested(p_initial_node_ids: PackedStringArray, p_wait_completion: bool) -> void:
	new_threads_requested.emit(p_initial_node_ids, p_wait_completion)


func _emit_finished(p_return_value: Variant) -> void:
	finished.emit(p_return_value)
