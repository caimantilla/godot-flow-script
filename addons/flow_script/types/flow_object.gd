@tool
class_name FlowObject
extends Object
## Contains the state of a FlowScript and the Flow API.
## This should be extended for your game's needs.


## Emitted when the object would like to be deleted.
signal deletion_requested()

## Requested when a new thread should be created.
signal flow_thread_creation_requested(initial_flow_node_id: String)

## Emitted when the controller finishes execution of some thread.
signal flow_thread_execution_finished(thread_id: String)


## Emitted 
var last_created_thread_id: String: set = set_last_created_thread_id, get = get_last_created_thread_id


var _last_created_thread_id: String = ""
var _locals: Dictionary = {}
var _locals_names := PackedStringArray()


# Overrides


func _init() -> void:
	pass


func _notification(what: int) -> void:
	if what == NOTIFICATION_PREDELETE:
		kill()


# Virtuals


## Virtual.
## Use this method to clean up any unmanaged memory.
func _kill() -> void:
	pass


## Virtual.
## Restores the game-specific object state from a save file.
func _set_state(p_state: Dictionary) -> void:
	pass


## Virtual.
## Returns the game-specific object state for use in a save file.
func _get_state() -> Dictionary:
	return {}


## Virtual.
## Checks the existence of a global variable.
func _has_global(p_variable_name: String) -> bool:
	return false


## Virtual.
## Sets the value of a global variable.
## Since this is game-specific, it is implemented as a virtual function.
func _set_global(p_variable_name: String, p_value: Variant) -> void:
	pass


## Virtual.
## Returns a global variable.
## Since this is game-specific, it is implemented as a virtual function.
func _get_global(p_variable_name: String) -> Variant:
	return null


## Virtual.
## Returns a list with the names of every global variable.
## Please, try to handle this in an optimized way, or expression evaluation could get slow!
func _get_global_names() -> PackedStringArray:
	return PackedStringArray()


## Virtual.
## Override to change the message printing behavior.
## This isn't mandatory, though, as a default behavior is provided.
func _console_print(p_value: Variant) -> void:
	print(p_value)


## Virtual.
## A default implementation is provided, but override if needed (like if you need a custom pausing behavior).
func _create_seconds_timer(p_duration: float) -> Object:
	var main_loop := Engine.get_main_loop() as SceneTree
	if main_loop == null:
		return null
	
	# Process the timer during the physics step by default.
	var timer: SceneTreeTimer = main_loop.create_timer(p_duration, false, true, false)
	
	return timer


# Sealed


func notify_thread_finished(p_thread_id: String) -> void:
	emit_signal.call_deferred(&"flow_thread_execution_finished", p_thread_id)


func set_last_created_thread_id(p_thread_id: String) -> void:
	_last_created_thread_id = p_thread_id


func get_last_created_thread_id() -> String:
	return _last_created_thread_id


## Kills flow execution.
func kill() -> void:
	_kill()


func wait_last_thread_finished() -> void:
	var new_thread_id: String = _last_created_thread_id
	
	# If there was a mistake in creating the thread, don't wait. A non-empty thread ID should indicate success.
	if not new_thread_id.is_empty():
		# WARNING: This could probably cause an infinite loop? It won't lock the game up, but depending on the usage it could softlock, and I'm not sure if this causes a leak. Be careful.
		while 1:
			var finished_thread: String = await flow_thread_execution_finished
			if finished_thread == new_thread_id:
				return


## Creates a thread and begins execution of the FlowScript from the node ID passed.
func execute_flow(p_from_node_id: String) -> void:
	flow_thread_creation_requested.emit(p_from_node_id)


## Creates a thread and executes it, then awaits its completion.
## This is only recommended if calling internally. Await should be avoided in FlowNodes.
func execute_flow_async(p_from_node_id: String) -> void:
	execute_flow(p_from_node_id)
	await wait_last_thread_finished()


## Requests deletion of the object.
func request_deletion() -> void:
	deletion_requested.emit()


## Restores the object's state from a save file.
func set_state(p_state: Dictionary) -> void:
	for key in p_state:
		var value: Variant = p_state[key]
		match key:
			"locals":
				for lkey in value:
					var lvalue: Variant = value[lkey]
					set_local(lkey, lvalue)
	
	_set_state(p_state)


## Gets the object's state for use in a save file.
func get_state() -> Dictionary:
	var state: Dictionary = _get_state()
	
	for lkey in _locals:
		assert(typeof(lkey) == TYPE_STRING)
		
		var lvalue: Variant = _locals[lkey]
		assert(FlowUtilities.is_type_serializable(typeof(lvalue)))
		
		state[lkey] = lvalue
	
	return state


func has_local(p_variable_name: String) -> bool:
	return (p_variable_name in self) or (_locals.has(p_variable_name))


## Sets the value of a local variable.
func set_local(p_variable_name: String, p_value: Variant) -> void:
	# If the variable name corresponds to a property, the input value will be assigned to that property instead of the locals dictionary.
	if p_variable_name in self:
		set(p_variable_name, p_value)
		return
	
	if not _locals.has(p_variable_name):
		_locals_names.push_back(p_variable_name)
	
	_locals[p_variable_name] = p_value


## Retrieves the value of a local variable, or null if it doesn't exist.
func get_local(p_variable_name: String) -> Variant:
	if p_variable_name in self:
		return get(p_variable_name)
	elif _locals.has(p_variable_name):
		return _locals[p_variable_name]
	else:
		return null


func get_local_names() -> PackedStringArray:
	return _locals_names


func get_local_values() -> Array:
	var local_names: PackedStringArray = get_local_names()
	var local_values: Array = []
	local_values.resize(local_names.size())
	
	for lidx: int in local_names.size():
		local_values[lidx] = get_local(local_names[lidx])
	
	return local_values


## Checks if a global variable with the name passed exists.
func has_global(p_variable_name: String) -> bool:
	return _has_global(p_variable_name)


## Sets the value of a global variable.
func set_global(p_variable_name: String, p_value: Variant) -> void:
	_set_global(p_variable_name, p_value)


## Retrieves the value of a global variable, or null if it doesn't exist.
func get_global(p_variable_name: String) -> Variant:
	return _get_global(p_variable_name)


## Returns the name of each global variable.
func get_global_names() -> PackedStringArray:
	return _get_global_names()


## Returns the value of each global variable.
func get_global_values() -> Array:
	var global_names: PackedStringArray = get_global_names()
	var global_values: Array = []
	global_values.resize(global_names.size())
	
	for gidx: int in global_names.size():
		global_values[gidx] = get_global(global_names[gidx])
	
	return global_values


## Checks if a variable exists with the name passed.
func has_variable(p_variable_name: String) -> bool:
	return has_local(p_variable_name) or has_global(p_variable_name)


## Returns a variable with the name passed.
## Local variables are prioritized.
func get_variable(p_variable_name: String) -> Variant:
	if has_local(p_variable_name):
		return get_local(p_variable_name)
	return null


## Returns the name of each variable; first local, then global.
func get_variable_names() -> PackedStringArray:
	var variable_names: PackedStringArray = get_local_names().duplicate()
	variable_names.append_array(get_global_names())
	return variable_names


## Returns the value of each variable; first local, then global.
func get_variable_values() -> Array:
	# NOTE: Assumes that the values array is constructed when called, not beforehand.
	var variable_values: Array = get_local_values()
	variable_values.append_array(get_global_values())
	return variable_values


## Evaluates some expression.
## If there are multiple expressions, only the result of the last one is returned.
func evaluate_expression(p_expression_string: String) -> Variant:
	var results: Array = evaluate_multiline_expression(p_expression_string)
	if results.is_empty():
		return null
	else:
		return results.back()


## Evaluates each line of a multi-line expression.
## If any return false, break and return false.
## If p_success_if_empty is true, and there are no expressions to evaluate, return true.
## Otherwise, return false in that case.
func evaluate_multiline_boolean_expression(p_multiline_expression_string: String, p_success_if_empty: bool = true) -> bool:
	var evaluation_results: Array = evaluate_multiline_expression(p_multiline_expression_string)
	
	if evaluation_results.is_empty():
		return p_success_if_empty
	
	for evaluation_result: Variant in evaluation_results:
		# This should also evaluate to false if null or something... the behavior isn't 100% consistent though
		if not evaluation_result:
			return false
	
	return true


## Executes any number of expressions, split by line.
## The result of each line is pushed into an array.
func evaluate_multiline_expression(p_multiline: String) -> Array:
	var variable_names: PackedStringArray = get_variable_names()
	var variable_values: Array = get_variable_values()
	
	var evaluation_results: Array = []
	var expression := Expression.new()
	
	var expression_strings: PackedStringArray = split_multiline_expression(p_multiline)
	
	for string: String in expression_strings:
		var parse_error: Error = expression.parse(string, variable_names)
		assert(parse_error == OK)
		
		evaluation_results.append(expression.execute(variable_values, self, true, false))
	
	return evaluation_results


func split_multiline_expression(p_multiline: String) -> PackedStringArray:
	var expressions: PackedStringArray = p_multiline.split("\n", false)
	
	for i in range(expressions.size() - 1, -1, -1):
		var expression: String = expressions[i]
		expression = expression.strip_edges()
		if expression.is_empty():
			expressions.remove_at(i)
		else:
			expressions.set(i, expression)
	
	return expressions


## Print some value to the console.
func console_print(p_value: Variant) -> void:
	_console_print(p_value)


## Creates a seconds timer.
## can return null, a Timer, or a SceneTreeTimer.
func create_seconds_timer(p_duration: float) -> Object:
	return _create_seconds_timer(p_duration)
