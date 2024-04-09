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
@export_custom(PROPERTY_HINT_NONE, "", PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY)
var type: String: get = get_type

## The name of the node's type.
var type_name: String: get = get_type_name

## The node's ID in the script that owns it.
@export_custom(PROPERTY_HINT_NONE, "", PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY)
var id: String: set = set_id, get = get_id

## The node's position in the graph.
var graph_position: Vector2i: set = set_graph_position, get = get_graph_position

## The node's horizontal position in the graph.
@export_custom(PROPERTY_HINT_NONE, "", PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY)
var graph_position_x: int: set = set_graph_position_x, get = get_graph_position_x

@export_custom(PROPERTY_HINT_NONE, "", PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY)
## The node's vertical position in the graph.
var graph_position_y: int: set = set_graph_position_y, get = get_graph_position_y


var _type: String
var _type_name: String
var _id_editable: bool
var _id: String = ""
var _graph_position := Vector2i.ZERO


# Overrides


func _init(p_type_id: String, p_type_name: String, p_id_editable: bool) -> void:
	_type = p_type_id
	_type_name = p_type_name
	_id_editable = p_id_editable


# Virtuals

## Virtual.
## Executes whatever code the node should execute.
func _execute(p_state: FlowNodeState) -> void:
	p_state.finish("", null)


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


## Returns whether or not a property is a reference to another node.
## This is used by the inspector plugin to turn a String FlowNode reference into a node selection menu.
func is_property_flow_node_reference(p_property_name: StringName) -> bool:
	return _is_property_flow_node_reference(p_property_name)


## Called when some node in the script is renamed.
## This enables connections to be updated.
func on_external_node_renamed(p_from_id: String, p_to_id: String) -> void:
	_on_external_node_renamed(p_from_id, p_to_id)


func is_id_editable() -> bool:
	return _id_editable


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
