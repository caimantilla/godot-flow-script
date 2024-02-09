@tool
class_name FlowNodeState
extends Object
## A FlowNode's link to the rest of the program.
##
## FlowNodeState provides context for a FlowNode's execution, a way to store state, and a method to signal its completion.
## Any variable can be stored in it if the frame needs to maintain state over time. Don't store state on the node!



## Emitted when all of the threads which the thread this state lives in are waiting for have finished execution.
signal resumed()

## Emitted when the state should finish, and move to the next node.
signal finished(next_node_id: String, return_value: Variant)

## Emitted when the creation of new threads is desired.
signal new_threads_requested(p_beginning_node_ids: PackedStringArray, p_wait_threads_completion: bool)



## Reference to the FlowObject which controls execution of the script and provides the flow nodes with an API and game context.
var flow_object: FlowObject: get = get_flow_object


var _flow_object: FlowObject = null
var _node_state: Dictionary = {}




func _init(p_flow_object: FlowObject) -> void:
	assert(is_instance_valid(p_flow_object), "An invalid FlowObject instance cannot be passed to a node state!")
	
	_flow_object = p_flow_object


func get_flow_object() -> FlowObject:
	return _flow_object


func set_temporary_variable(p_name: String, p_value: Variant) -> void:
	_node_state[p_name] = p_value


func get_temporary_variable(p_name: String) -> Variant:
	if _node_state.has(p_name):
		return _node_state[p_name]
	
	return null


## Return the node state, in case the state of the FlowController needs to be serialized.
func get_local_state() -> Dictionary:
	return _node_state


func resume() -> void:
	resumed.emit()


## Notifies the thread that the state is finished.
## Optionally, a return value may be passed.
## WARNING: Be careful not to interact with the node state after calling finish, as it is immediately freed by its thread!
func finish(p_next_node_id: String, p_return_value: Variant = null) -> void:
	finished.emit(p_next_node_id, p_return_value)


## Requests the creation of new threads.
## If p_wait_threads_completion is true, this node will finish when the requested threads all finish execution.
func request_new_threads(p_initial_node_ids: PackedStringArray, p_wait_threads_completion: bool) -> void:
	new_threads_requested.emit(p_initial_node_ids, p_wait_threads_completion)


## Requests the creation of a new thread.
## If p_wait_thread_completion is true, this node will finish execution when the requested thread finishes execution.
func request_new_thread(p_from_node_id: String, p_wait_thread_completion: bool) -> void:
	var ids := PackedStringArray()
	ids.push_back(p_from_node_id)
	request_new_threads(ids, p_wait_thread_completion)
