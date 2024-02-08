@tool
class_name FlowNodeEditor
extends GraphNode
## The graphical representation of a FlowNode.
##
## A custom FlowNodeEditor is defined for each FlowNode type.
## Parameter editing is primarily done in the inspector, so the FlowNodeEditor should only offer light editing capabilities, while its role as a preview should be prioritized.





signal rename_dialog_requested()





# Public variables

var flow_node: FlowNode: set = set_flow_node, get = get_flow_node






# Private variables

var __flow_node: FlowNode = null
var __edit_safe: bool = false
var __rename_button: Button = null
var __close_button: Button = null






# Overrides


func _notification(what: int) -> void:
	if what == NOTIFICATION_THEME_CHANGED:
		update_style()



func _init() -> void:
	pass


func _ready() -> void:
	pass


func _enter_tree() -> void:
	pass


func _exit_tree() -> void:
	pass




# Virtual methods


## Virtual.
## Create any persistent nodes or whatever.
func _initialize() -> void:
	pass


## Virtual.
## Clean up any leftover stuff if reassigning the editor (this shouldn't happen though...)
func _clean_up() -> void:
	pass


## Virtual.
## Synchronize the editor with the node state.
func _on_node_updated() -> void:
	pass


## Virtual.
## Update the editor styling.
## This includes icons, style boxes, etc.
func _update_style() -> void:
	pass


## Virtual.
## Return the input slot for other nodes to connect to.
func _get_input_slot() -> int:
	return -1


## Virtual.
## Set a connection to another node.
func _set_outgoing_connection(p_connection_parameters: FlowNodeEditorOutgoingConnectionParameters) -> void:
	pass


## Virtual.
## Return each of the connections to other nodes.
func _get_outgoing_connections() -> Array[FlowNodeEditorOutgoingConnectionParameters]:
	return []








# Sealed methods


## Returns whether or not it's safe to edit the scene, for something like updating the style.
## This is irrelevant if constructing the editor scene from a script.
func is_safe_to_edit() -> bool:
	return __edit_safe


## Sets the editor's flow node instance.
func set_flow_node(p_node: FlowNode) -> void:
	if __flow_node != null:
		printerr("Attempted to override a FlowNodeEditor's FlowNode. Why!?")
		return
	
	__flow_node = p_node
	
	if is_node_ready():
		initialize()
	else:
		ready.connect(initialize, CONNECT_ONE_SHOT)


func get_flow_node() -> FlowNode:
	return __flow_node





func input_get_slot_from_port(p_port: int) -> int:
	var slot: int = get_input_port_slot(p_port)
	#slot -= 1
	return slot


func output_get_slot_from_port(p_port: int) -> int:
	var slot: int = get_output_port_slot(p_port)
	#slot -= 1
	return slot


func input_get_port_from_slot(p_slot: int) -> int:
	var port: int = p_slot
	
	for i in p_slot:
		if not is_slot_enabled_left(i):
			port -= 1
	
	return port


func output_get_port_from_slot(p_slot: int) -> int:
	var port: int = p_slot
	
	for i in p_slot:
		if not is_slot_enabled_right(i):
			port -= 1
	
	return port


func get_input_slot() -> int:
	return _get_input_slot()


func get_input_port() -> int:
	return input_get_port_from_slot(get_input_slot())


func set_outgoing_connection(p_outgoing_connection: FlowNodeEditorOutgoingConnectionParameters) -> void:
	p_outgoing_connection.slot = output_get_slot_from_port(p_outgoing_connection.port)
	_set_outgoing_connection(p_outgoing_connection)


## Returns all of the node's outgoing connections.
func get_outgoing_connections() -> Array[FlowNodeEditorOutgoingConnectionParameters]:
	var connections: Array[FlowNodeEditorOutgoingConnectionParameters] = []
	
	if __flow_node != null:
		connections.append_array(_get_outgoing_connections())
	
	for outgoing_connection in connections:
		outgoing_connection.port = output_get_port_from_slot(outgoing_connection.get_slot())
	
	return connections





func initialize() -> void:
	if __flow_node == null:
		return
	
	__edit_safe = true
	
	
	#set_texture_filter(TEXTURE_FILTER_NEAREST)
	
	
	__rename_button = Button.new()
	__rename_button.set_name("Rename Button")
	__rename_button.set_visible(__flow_node.is_id_editable())
	__rename_button.set_h_size_flags(SIZE_SHRINK_END)
	__rename_button.set_v_size_flags(SIZE_EXPAND_FILL)
	__rename_button.pressed.connect(request_rename_dialog, CONNECT_DEFERRED)
	get_titlebar_hbox().add_child(__rename_button, true)
	
	
	__close_button = Button.new()
	__close_button.set_name("Close Button")
	__close_button.set_h_size_flags(SIZE_SHRINK_END)
	__close_button.set_v_size_flags(SIZE_EXPAND_FILL)
	__close_button.pressed.connect(request_deletion, CONNECT_DEFERRED)
	get_titlebar_hbox().add_child(__close_button, true)
	
	
	__flow_node.graph_position_changed.connect(__update_position)
	__flow_node.changed.connect(on_node_updated)
	
	
	_initialize()
	
	
	update_style()
	on_node_updated()


func on_node_updated() -> void:
	if __flow_node == null or not is_safe_to_edit():
		return
	
	__edit_safe = false
	
	set_name(__flow_node.get_id())
	
	# No need to show the ID if the node's ID isn't editable.
	# If it is editable, then priority is given to the ID, the type is less important.
	if __flow_node.is_id_editable():
		set_title("%s (%s)" % [ __flow_node.get_id(), __flow_node.get_type_name() ])
	else:
		set_title(__flow_node.get_type_name())
	
	# Still, show it all in the tooltip.
	set_tooltip_text("Type: %s\nID: %s" % [ __flow_node.get_type_name(), __flow_node.get_id() ])
	
	__update_position()
	
	_on_node_updated()
	
	__edit_safe = true


func update_style() -> void:
	if not is_safe_to_edit() or not Engine.is_editor_hint():
		return
	
	__rename_button.set_button_icon(get_theme_icon(&"EditInternal", &"EditorIcons"))
	__close_button.set_button_icon(get_theme_icon(&"RemoveInternal", &"EditorIcons"))
	
	_update_style()



func get_editor_scale() -> float:
	if Engine.is_editor_hint():
		return EditorInterface.get_editor_scale()
	else:
		return 1.0


func request_deletion() -> void:
	delete_request.emit()



func request_rename_dialog() -> void:
	rename_dialog_requested.emit()


func copy_graph_position_to_node() -> void:
	var node: FlowNode = get_flow_node()
	if node != null:
		var node_new_pos: Vector2 = get_position_offset()
		node_new_pos /= get_editor_scale()
		node_new_pos = node_new_pos.floor()
		
		node.set_graph_position_no_signal(Vector2i(node_new_pos))


# Private methods


func __update_position() -> void:
	return # Actually let the graph handle this
	#if __flow_node != null:
		#
		#var new_position := Vector2(__flow_node.get_graph_position())
		#if Engine.is_editor_hint():
			#new_position *= EditorInterface.get_editor_scale()
		#
		#set_position_offset(new_position)
