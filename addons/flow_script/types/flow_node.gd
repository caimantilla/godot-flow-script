@tool
class_name FlowNode
extends Object
## A FlowNode object.




## Emitted when some property changes.
## Used to signal to update the graph state.
signal changed()

## Emitted when the node's ID is changed.
signal id_changed(from: String, to: String)

## Emitted when the node's graph position changes.
## Separate signal from changed() so that the whole editor doesn't have to update every frame when being dragged.
signal graph_position_changed()



## The ID of the node's type.
var type: String: get = get_type

## The name of the node's type.
var type_name: String: get = get_type_name

## The node's ID in the script that owns it.
var id: String: set = set_id, get = get_id

## The node's position in the graph.
var graph_position: Vector2i: set = set_graph_position, get = get_graph_position

## The node's horizontal position in the graph.
var graph_position_x: int: set = set_graph_position_x, get = get_graph_position_x

## The node's vertical position in the graph.
var graph_position_y: int: set = set_graph_position_y, get = get_graph_position_y


var _type: String
var _type_name: String
var _id: String = ""
var _graph_position := Vector2i.ZERO





# Overrides


func _init(p_type_id: String, p_type_name: String) -> void:
	_type = p_type_id
	_type_name = p_type_name




func _get_property_list() -> Array[Dictionary]:
	var properties: Array[Dictionary] = []
	
	properties.append({
		"name": "FlowNode",
		"type": TYPE_NIL,
		"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_CATEGORY,
	})
	
	properties.append({
		"name": "type",
		"type": TYPE_STRING,
		"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY,
	})
	
	properties.append({
		"name": "id",
		"type": TYPE_STRING,
		"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY,
	})
	
	for axis_name: String in [ 'x', 'y' ]:
		properties.append({
			"name": "graph_position_" + axis_name,
			"type": TYPE_INT,
			"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY,
		})
	
	return properties




# Virtuals

## Virtual.
## Executes whatever code the node should execute.
func _execute(p_state: FlowNodeState) -> void:
	p_state.finish("", null)


## Virtual.
## Returns whether or not the ID for this type of node is editable.
func _is_id_editable() -> bool:
	return false


## Virtual.
## Returns whether or not a string property is a reference to another node.
func _is_property_flow_node_reference(p_property_name: StringName) -> bool:
	return false


## Virtual.
## Handles node reference updates.
func _on_external_node_renamed(p_from_id: String, p_to_id: String) -> void:
	pass


## Virtual.
## Returns whatever string or resource translations there should be for the node.
## Note that this does not contain the remap itself, but only the ID and original data!
## It is used for translating a FlowScript.
func _get_translations() -> Array[FlowNodeTranslationEntry]:
	var placeholder_list: Array[FlowNodeTranslationEntry] = []
	return placeholder_list





# Sealed


## Begins execution of whatever code the node should execute.
func execute(p_state: FlowNodeState) -> void:
	_execute(p_state)


## If true, the node's ID is visible in the graph and is editable.
## When creating a new node, a dialog prompting its ID will be shown.
func is_id_editable() -> bool:
	return _is_id_editable()


## Returns whether or not a property is a reference to another node.
## This is used by the inspector plugin to turn a String FlowNode reference into a node selection menu.
func is_property_flow_node_reference(p_property_name: StringName) -> bool:
	return _is_property_flow_node_reference(p_property_name)


## Called when some node in the script is renamed.
## This enables connections to be updated.
func on_external_node_renamed(p_from_id: String, p_to_id: String) -> void:
	_on_external_node_renamed(p_from_id, p_to_id)


## Called when generating a translation template for a FlowScript.
func get_translations() -> Array[FlowNodeTranslationEntry]:
	return _get_translations()







## Emits the changed() signal.
func emit_changed() -> void:
	changed.emit()







## Returns the node's type's ID.
func get_type() -> String:
	return _type


## Returns the node's type's name.
func get_type_name() -> String:
	return _type_name


func set_id(p_id: String) -> void:
	if p_id != _id:
		var old_id: String = _id
		_id = p_id
		id_changed.emit(old_id, p_id)

func get_id() -> String:
	return _id


func set_graph_position(p_pos: Vector2i) -> void:
	if p_pos != _graph_position:
		_graph_position = p_pos
		graph_position_changed.emit()

func get_graph_position() -> Vector2i:
	return _graph_position


func set_graph_position_x(p_x: int) -> void:
	set_graph_position(Vector2i(p_x, _graph_position.y))

func get_graph_position_x() -> int:
	return _graph_position.x


func set_graph_position_y(p_y: int) -> void:
	set_graph_position(Vector2i(_graph_position.x, p_y))

func get_graph_position_y() -> int:
	return _graph_position.y


func set_graph_position_no_signal(p_pos: Vector2i) -> void:
	#if p_pos != _graph_position:
	# No point doing the check if the signal isn't even being emitted anyway lol
	_graph_position = p_pos






func set_data(p_data: Dictionary) -> void:
	var property_list: Array[Dictionary] = get_property_list()
	
	for property: Dictionary in property_list:
		var property_usage: int = property.usage
		
		if property_usage & PROPERTY_USAGE_SCRIPT_VARIABLE \
		and property_usage & PROPERTY_USAGE_STORAGE:
			
			var property_name: String = property.name
			
			if p_data.has(property_name):
				var property_type = property.type
				
				var seria_value: Variant = p_data[property_name]
				var seria_value_type: Variant.Type = typeof(seria_value)
				
				if (seria_value_type == property_type):
					set(property_name, seria_value)
				elif (seria_value_type == TYPE_NIL and property_type == TYPE_OBJECT):
					set(property_name, null)


func get_data() -> Dictionary:
	var data: Dictionary = {}
	
	for property_info: Dictionary in get_property_list():
		
		if property_info.usage & PROPERTY_USAGE_SCRIPT_VARIABLE \
		and property_info.usage & PROPERTY_USAGE_STORAGE:
			
			data[property_info.name] = get(property_info.name)
	
	data["type"] = get_type()
	
	return data
