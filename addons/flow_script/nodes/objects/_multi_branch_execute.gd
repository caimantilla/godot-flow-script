@tool
extends FlowNode
## Executes a list of branches.
## This is the base, use either the simultaneous or sequential variants which extend this script!


const MAX_STACK_SIZE: int = 9


## The next node to execute once the branches have finished.
var next_node_id: String: set = set_next_node_id, get = get_next_node_id

## The list of nodes to start.
var execution_stack: PackedStringArray: set = set_execution_stack, get = get_execution_stack


var _next_node_id: String = ""
var _execution_stack := PackedStringArray()


func _get_property_list() -> Array[Dictionary]:
	var properties: Array[Dictionary] = []
	
	properties.append({
		"name": get_type_name(),
		"type": TYPE_NIL,
		"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_CATEGORY,
	})
	
	properties.append({
		"name": "next_node_id",
		"type": TYPE_STRING,
		"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY,
	})
	
	properties.append({
		"name": "execution_stack",
		"type": TYPE_PACKED_STRING_ARRAY,
		"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE,
	})
	
	properties.append({
		"name": "execution_stack/size",
		"type": TYPE_INT,
		"hint": PROPERTY_HINT_RANGE,
		"hint_string": "0,%d,1" % MAX_STACK_SIZE,
		"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_EDITOR,
	})
	
	for i: int in get_execution_stack_size():
		properties.append({
			"name": "execution_stack/node_%d" % (i + 1),
			"type": TYPE_STRING,
			"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY,
		})
	
	return properties


func _set(p_property: StringName, p_value: Variant) -> bool:
	if p_property.begins_with("execution_stack/"):
		var stack_property: String = p_property.get_slice("/", 1)
		
		if stack_property == "size":
			resize_execution_stack(p_value)
			return true
		
		elif stack_property.begins_with("node_"):
			var entry_idx: int = stack_property.get_slice("_", 1).to_int() - 1
			set_execution_stack_entry_at(entry_idx, p_value)
			return true
	
	return false


func _get(p_property: StringName) -> Variant:
	if p_property.begins_with("execution_stack/"):
		var stack_property: String = p_property.get_slice("/", 1)
		
		if stack_property == "size":
			return get_execution_stack_size()
		
		elif stack_property.begins_with("node_"):
			var entry_idx: int = stack_property.get_slice("_", 1).to_int() - 1
			return get_execution_stack_entry_at(entry_idx)
	
	return null


func _is_property_flow_node_reference(p_property: StringName) -> bool:
	return (p_property == &"next_node_id") or (p_property.begins_with("execution_stack/node_"))


func _on_external_node_renamed(p_from: String, p_to: String) -> void:
	if p_from == _next_node_id:
		_next_node_id = p_to
	
	for i: int in get_execution_stack_size():
		var current: String = get_execution_stack_entry_at(i)
		if current == p_from:
			_set_execution_stack_entry_at(i, p_to, false)


func set_next_node_id(p_id: String) -> void:
	if p_id != _next_node_id:
		_next_node_id = p_id
		emit_changed()

func get_next_node_id() -> String:
	return _next_node_id


func set_execution_stack(p_stack_arr: PackedStringArray) -> void:
	if p_stack_arr != _execution_stack:
		_execution_stack = p_stack_arr
		emit_changed()


func get_execution_stack() -> PackedStringArray:
	return _execution_stack


func get_execution_stack_size() -> int:
	return _execution_stack.size()


func set_execution_stack_entry_at(p_idx: int, p_value: String) -> void:
	_set_execution_stack_entry_at(p_idx, p_value, true)


func remove_execution_stack_entry_at(p_idx: int) -> void:
	_remove_execution_stack_entry_at(p_idx, true)


func get_execution_stack_entry_at(p_idx: int) -> String:
	if p_idx > -1 and p_idx < get_execution_stack_size():
		return _execution_stack[p_idx]
	
	return ""


func resize_execution_stack(p_new_size: int) -> void:
	_resize_execution_stack(p_new_size, true)


func _set_execution_stack_entry_at(p_idx: int, p_value: String, p_emit: bool) -> void:
	if p_idx > -1 and p_idx < get_execution_stack_size():
		var current: String = _execution_stack[p_idx]
		
		if current != p_value:
			_execution_stack.set(p_idx, p_value)
			
			if p_emit:
				emit_changed()


func _remove_execution_stack_entry_at(p_idx: int, p_emit: bool) -> void:
	if p_idx > -1 and p_idx < get_execution_stack_size():
		_execution_stack.remove_at(p_idx)
		
		if p_emit:
			emit_changed()


func _resize_execution_stack(p_new_size: int, p_emit: bool) -> void:
	if p_new_size > -1 and p_new_size < MAX_STACK_SIZE and p_new_size != _execution_stack.size():
		_execution_stack.resize(p_new_size)
		
		if p_emit:
			emit_changed()
		
		notify_property_list_changed()
