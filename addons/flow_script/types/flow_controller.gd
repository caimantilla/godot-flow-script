
class_name FlowController
extends Node
## Controls execution of a FlowScript.


signal finished()
signal thread_finished(return_value: Variant, thread_id: String)


## The number of threads cannot exceed this.
const MAX_THREADS: int = 999


## The script of the object type to instantiate.
## It script must extend FlowObject in order to be valid.
@export var flow_object_type: GDScript: set = set_flow_object_type, get = get_flow_object_type

## The FlowScript assigned to the controller.
## A reference to it is passed to each thread.
@export var flow_script: FlowScript: set = set_flow_script, get = get_flow_script

## The FlowController's FlowObject where local state and stuff is stored.
var flow_object: FlowObject: get = get_flow_object



var _flow_object_type: GDScript = null
var _flow_object: FlowObject = null
var _flow_script: FlowScript = null
var _flow_thread_map: Dictionary = {}
var _last_return_value: Variant = null


## Initializes the FlowController.
## A FlowObject type is required.
#func _init(p_flow_object_type: GDScript, p_flow_script: FlowScript) -> void:
	#assert(p_flow_object_type != null, "FlowObject script cannot be null!")
	#
	## The first script checked shouldn't be the one passed, since FlowObject should be treated as an abstract class. There are methods which aren't implemented in it, like the ones to interact with global variables.
	#var obj_base_script: Script = p_flow_object_type.get_base_script()
	#while obj_base_script != null:
		#if obj_base_script == FlowObject:
			#break
		#obj_base_script = obj_base_script.get_base_script()
	#
	#assert(obj_base_script == FlowObject, "Script passed doesn't extend FlowObject!")
	#
	#
	#_flow_object = p_flow_object_type.new()
	#_flow_script = p_flow_script


func _notification(what: int) -> void:
	if what == NOTIFICATION_PREDELETE:
		kill()


func _ready() -> void:
	if Engine.is_editor_hint():
		return
	
	_initialize_flow_object()


func set_flow_object_type(p_type_script: GDScript) -> void:
	var obj_base_script := p_type_script.get_base_script()
	
	while obj_base_script != null:
		if obj_base_script == FlowObject:
			break
		obj_base_script = obj_base_script.get_base_script()
	
	if obj_base_script == FlowObject:
		_flow_object_type = p_type_script
	else:
		_flow_object_type = null


func get_flow_object_type() -> GDScript:
	return _flow_object_type


func set_flow_script(p_script: FlowScript) -> void:
	_flow_script = p_script


func get_flow_script() -> FlowScript:
	return _flow_script


func get_flow_object() -> FlowObject:
	if _flow_object == null:
		_initialize_flow_object()
	return _flow_object


## Returns the last value to be returned by a thread.
## Query this once the thread you're waiting on finishes.
func get_last_return_value() -> Variant:
	return _last_return_value


func get_active_thread_count() -> int:
	return _flow_thread_map.size()


func has_thread_with_id(p_thread_id: String) -> bool:
	return _flow_thread_map.has(p_thread_id)


func get_thread_by_id(p_thread_id: String) -> FlowThread:
	if has_thread_with_id(p_thread_id):
		return _flow_thread_map[p_thread_id] as FlowThread
	
	return null


func is_thread_active(p_thread_id: String) -> bool:
	var thread: FlowThread = get_thread_by_id(p_thread_id)
	
	if thread != null:
		return thread.is_active()
	
	return false


func get_thread_list() -> Array:
	return _flow_thread_map.values()


func get_thread_id_list() -> Array:
	return _flow_thread_map.keys()


func is_finished() -> bool:
	for thread: FlowThread in get_thread_list():
		if thread != null and not thread.is_finished():
			return false
	
	return true


func kill() -> void:
	for thread_id: String in get_thread_id_list().duplicate(false):
		_delete_thread(thread_id)
	
	if _flow_object != null:
		_flow_object.kill()
		Engine.get_main_loop().queue_delete(_flow_object)
		_flow_object = null


## Begins execution of a thread.
func execute(p_procedure_id: String) -> void:
	var _thread: FlowThread = _create_new_thread(p_procedure_id)


## Begins execution of a thread.
## A callback is also passed, which is later called when a value is returned from the thread created.
func execute_with_return_callback(p_procedure_id: String, p_callback: Callable) -> void:
	var thread: FlowThread = _create_new_thread(p_procedure_id)
	
	if thread == null:
		p_callback.call(get_last_return_value())
	else:
		# TODO: Add some callback with a bind for checking if the correct thread has finished.
		# qWhen that thread finishes, call the return callback passed to this method.
		pass


func _initialize_flow_object() -> void:
	if _flow_object == null and _flow_object_type != null:
		_flow_object = _flow_object_type.new() as FlowObject
		_flow_object.flow_thread_creation_requested.connect(_on_flow_object_new_thread_requested)


func _on_thread_new_threads_creation_request(p_initial_node_ids: PackedStringArray, p_wait_completion: bool, p_calling_thread_id: String) -> void:
	assert(has_thread_with_id(p_calling_thread_id), "wtf")
	
	var calling_thread: FlowThread = get_thread_by_id(p_calling_thread_id)
	assert(calling_thread != null)
	
	for initial_node_id: String in p_initial_node_ids:
		var new_thread: FlowThread = _create_new_thread(initial_node_id)
		var new_thread_id: String = new_thread.get_thread_id()
		
		calling_thread.add_resume_dependent_thread(new_thread_id)


func _on_thread_finished(p_return_value: Variant, p_thread_id: String) -> void:
	# Don't do anything unless thread deletion is successful... Not sure why it shouldn't be, but yeah !!
	_last_return_value = p_return_value
	
	if _delete_thread(p_thread_id):
		# Duplicate the thread list to make sure that the same array as any potential future calls is not being used.
		# It's just an array of pointers anyways, so I'd think it should be quick to duplicate.
		var thread_list = get_thread_list().duplicate(false)
		
		if thread_list.is_empty():
			finished.emit()
		
		# Notify every other thread of the thread's completion, in case they're waiting on its completion to resume execution.
		for other_thread: FlowThread in thread_list:
			if other_thread != null and other_thread.get_thread_id() != p_thread_id:
				# NOTE: I'm worried about threads suddenly finishing and fighting each other here.
				# Is deferring the call enough? If a deferred call is made at the end of a frame, wouldn't it be called immediately...?
				# I guess I'll figure it out as the edge cases show themselves... It probably wouldn't matter for a game I make, but it could for some game which involves many different threads.
				other_thread.on_external_thread_finished.call_deferred(p_thread_id)
		
		thread_finished.emit(p_return_value, p_thread_id)
		get_flow_object().notify_thread_finished(p_thread_id)


func _delete_thread(p_thread_id: String) -> bool:
	var thread: FlowThread = get_thread_by_id(p_thread_id)
	
	if thread != null:
		thread.kill()
		Engine.get_main_loop().queue_delete(thread)
		_flow_thread_map.erase(p_thread_id)
		return true
	
	return false


func _create_new_thread(p_from_node_id: String) -> FlowThread:
	assert(get_active_thread_count() < MAX_THREADS, "Attempting to make more than %d threads." % MAX_THREADS)
	
	var new_thread_id: String = _generate_new_thread_id()
	while has_thread_with_id(new_thread_id):
		new_thread_id = _generate_new_thread_id()
	
	var new_thread := FlowThread.new(new_thread_id, _flow_object, _flow_script)
	_flow_thread_map[new_thread_id] = new_thread
	
	new_thread.finished.connect(_on_thread_finished.bind(new_thread_id))
	new_thread.new_threads_requested.connect(_on_thread_new_threads_creation_request.bind(new_thread_id))
	
	new_thread.start(p_from_node_id)
	return new_thread


func _generate_new_thread_id() -> String:
	return "THREAD_" + str(randi())


func _on_flow_object_new_thread_requested(p_from_node_id: String) -> void:
	var thread: FlowThread = _create_new_thread(p_from_node_id)
	var thread_id: String = thread.get_thread_id() if thread != null else ""
	
	get_flow_object().set_last_created_thread_id(thread_id)
