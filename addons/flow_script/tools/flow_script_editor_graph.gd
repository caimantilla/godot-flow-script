@tool
class_name FlowScriptEditorGraph
extends GraphEdit




signal flow_node_selected(node_id: String)
signal flow_node_creation_dialog_requested()
signal flow_nodes_deletion_requested(node_ids: PackedStringArray)
signal flow_node_rename_dialog_requested(for_node_id: String)



var flow_page: FlowPage: set = set_page, get = get_page




var _flow_page: FlowPage = null
var _node_editor_map: Dictionary = {}

var _graph_redraw_queued: bool = false
var _connections_redraw_queued: bool = false





func _init() -> void:
	#set_connection_lines_antialiased(true)
	set_show_grid(false)
	set_show_grid_buttons(false)
	set_snapping_enabled(false)
	set_right_disconnects(true)
	
	connection_request.connect(_on_graph_connection_request)
	disconnection_request.connect(_on_graph_disconnection_request)
	delete_nodes_request.connect(_on_graph_delete_nodes_request)
	node_selected.connect(_on_graph_node_selected)
	end_node_move.connect(_on_graph_node_move_ended, CONNECT_DEFERRED)


func _process(_delta: float) -> void:
	if _graph_redraw_queued:
		_graph_redraw_queued = false
		_redraw_graph_nodes()
	
	if _connections_redraw_queued:
		_connections_redraw_queued = false
		_redraw_graph_connections()


func queue_graph_redraw() -> void:
	_graph_redraw_queued = true


func queue_redraw_connections() -> void:
	_connections_redraw_queued = true



func set_page(p_page: FlowPage) -> void:
	if p_page == _flow_page:
		return
	
	if _flow_page != null:
		#_flow_page.node_created.disconnect(queue_graph_redraw)
		_flow_page.node_created.disconnect(_on_page_node_created)
		_flow_page.deleting_node.disconnect(_on_page_deleting_node)
		_flow_page.node_deleted.disconnect(_on_page_node_deleted)
		_flow_page.nodes_deleted.disconnect(_on_page_nodes_deleted)
		_flow_page.node_renamed.disconnect(_on_page_node_renamed)
	
	_flow_page = p_page
	
	if p_page != null:
		#p_page.node_created.connect(queue_graph_redraw.unbind(1))
		p_page.node_created.connect(_on_page_node_created)
		p_page.deleting_node.connect(_on_page_deleting_node)
		p_page.node_deleted.connect(_on_page_node_deleted)
		p_page.nodes_deleted.connect(_on_page_nodes_deleted)
		p_page.node_renamed.connect(_on_page_node_renamed)
	
	_redraw_graph_nodes()


func get_page() -> FlowPage:
	return _flow_page



func has_node_editor_with_id(p_node_id: String) -> bool:
	return _node_editor_map.has(p_node_id)


func get_node_editor_by_id(p_node_id: String) -> FlowNodeEditor:
	if has_node_editor_with_id(p_node_id):
		return _node_editor_map[p_node_id] as FlowNodeEditor
	
	return null


func get_node_editor_list() -> Array:
	return _node_editor_map.values()



func _on_graph_node_selected(p_node: Node) -> void:
	if p_node is FlowNodeEditor:
		var flow_node: FlowNode = p_node.get_flow_node()
		var node_id: String = flow_node.get_id()
		
		flow_node_selected.emit(node_id)


func _clear_drawn_graph() -> void:
	clear_connections()
	
	for node_id: String in _node_editor_map.keys():
		_delete_node_editor(node_id, false)
	
	_node_editor_map.clear()


func _delete_node_editor(p_node_id: String, p_erase_id_map_entry: bool = true) -> void:
	if not has_node_editor_with_id(p_node_id):
		return
	
	var node_editor: FlowNodeEditor = get_node_editor_by_id(p_node_id)
	
	if node_editor != null:
		var node: FlowNode = node_editor.get_flow_node()
		
		if node != null:
			node.graph_position_changed.disconnect(_update_node_editor_position)
		
		node_editor.queue_free()
	
	if p_erase_id_map_entry:
		_node_editor_map.erase(p_node_id)


# Erases all the editors without a corresponding object.
func _delete_invalid_node_editors() -> void:
	if _flow_page == null:
		_clear_drawn_graph()
		return
	
	var erased_ids := PackedStringArray()
	
	for id: String in _node_editor_map.keys():
		if not _flow_page.has_node_with_id(id):
			_delete_node_editor(id, false)
	
	for id: String in erased_ids:
		_node_editor_map.erase(id)


func _clear_graph_connections() -> void:
	clear_connections()


func _redraw_graph_connections() -> void:
	_clear_graph_connections()
	
	for node_editor: FlowNodeEditor in get_node_editor_list():
		var outgoing_connections: Array[FlowNodeEditorOutgoingConnectionParameters] = node_editor.get_outgoing_connections()
		
		for outgoing_connection in outgoing_connections:
			var target_node_editor: FlowNodeEditor = get_node_editor_by_id(outgoing_connection.get_node_id())
			
			if target_node_editor != null:
				var input_port: int = target_node_editor.get_input_port()
				
				if input_port > -1 and input_port < target_node_editor.get_input_port_count():
					connect_node(node_editor.get_name(), outgoing_connection.get_port(), target_node_editor.get_name(), input_port)


func _redraw_graph_nodes() -> void:
	_clear_drawn_graph()
	
	if _flow_page == null:
		return
	
	for node_id: String in _flow_page.get_node_ids():
		_instantiate_editor_for_node(node_id)
	
	_redraw_graph_connections()


func _instantiate_editor_for_node(p_node_id: String) -> FlowNodeEditor:
	if _node_editor_map.has(p_node_id):
		return null
	
	if _flow_page == null:
		return
	
	var node: FlowNode = _flow_page.get_node_by_id(p_node_id)
	if node != null:
		var node_id: String = node.get_id()
		var editor_node_instance: FlowNodeEditor = FlowFactory.instantiate_editor_from_node(node)
		
		if editor_node_instance != null:
			_node_editor_map[node_id] = editor_node_instance
			
			editor_node_instance.rename_dialog_requested.connect(_request_flow_node_rename_dialog.bind(node_id))
			editor_node_instance.delete_request.connect(_on_node_editor_deletion_request.bind(editor_node_instance))
			node.graph_position_changed.connect(_update_node_editor_position.bind(editor_node_instance))
			
			_update_node_editor_position(editor_node_instance)
			add_child(editor_node_instance, false, INTERNAL_MODE_FRONT)
			
			return editor_node_instance
	
	return null


func _create_connection(p_from_editor: FlowNodeEditor, p_from_port: int, p_to_editor: FlowNodeEditor) -> void:
	
	var connection := FlowNodeEditorOutgoingConnectionParameters.new()
	
	connection.set_port(p_from_port)
	connection.set_node_id(p_to_editor.get_flow_node().get_id())
	
	p_from_editor.set_outgoing_connection(connection)
	_redraw_graph_connections()


func _erase_connection(p_from_editor: FlowNodeEditor, p_from_port: int) -> void:
	
	var connection := FlowNodeEditorOutgoingConnectionParameters.new()
	
	connection.set_port(p_from_port)
	connection.set_node_id("")
	
	p_from_editor.set_outgoing_connection(connection)
	
	_redraw_graph_connections()


func _on_graph_connection_request(p_from_node: StringName, p_from_port: int, p_to_node: StringName, p_to_port: int) -> void:
	var from_editor := get_node(String(p_from_node)) as FlowNodeEditor
	var to_editor := get_node(String(p_to_node)) as FlowNodeEditor
	
	if (from_editor == null) or (to_editor == null):
		return
	
	_create_connection(from_editor, p_from_port, to_editor)


func _on_graph_disconnection_request(p_from_node: StringName, p_from_port: int, p_to_node: StringName, p_to_port: int) -> void:
	var from_editor := get_node(String(p_from_node)) as FlowNodeEditor
	var to_editor := get_node(String(p_to_node)) as FlowNodeEditor
	
	if (from_editor == null) or (to_editor == null):
		return
	
	_erase_connection(from_editor, p_from_port)


func _on_graph_node_move_ended() -> void:
	for node_editor: FlowNodeEditor in _node_editor_map.values():
		node_editor.copy_graph_position_to_node()


func _on_graph_delete_nodes_request(p_node_names: Array[StringName]) -> void:
	var node_editors: Array[FlowNodeEditor] = []
	
	for node_name: StringName in p_node_names:
		var node_editor := get_node(String(node_name)) as FlowNodeEditor
		if node_editor != null:
			node_editors.append(node_editor)
	
	var nodes_ids := PackedStringArray()
	nodes_ids.resize(node_editors.size())
	
	for i in node_editors.size():
		var node: FlowNode = node_editors[i].get_flow_node()
		nodes_ids.set(i, node.get_id())
	
	flow_nodes_deletion_requested.emit(nodes_ids)


func _on_node_editor_deletion_request(p_node_editor: FlowNodeEditor) -> void:
	var flow_node: FlowNode = p_node_editor.get_flow_node()
	
	var ids := PackedStringArray()
	ids.push_back(flow_node.get_id())
	
	flow_nodes_deletion_requested.emit(ids)


func _update_node_editor_position(p_node_editor: FlowNodeEditor) -> void:
	if p_node_editor != null:
		var node: FlowNode = p_node_editor.get_flow_node()
		
		if node != null:
			var internal_pos: Vector2i = node.get_graph_position()
			
			var screen_pos := Vector2(internal_pos)
			
			if Engine.is_editor_hint():
				screen_pos *= EditorInterface.get_editor_scale()
			
			p_node_editor.set_position_offset(screen_pos.floor())


func _on_page_node_created(p_id: String) -> void:
	if _node_editor_map.has(p_id):
		return
	
	_instantiate_editor_for_node(p_id)


func _on_page_deleting_node(p_id: String) -> void:
	pass
	#_delete_node_editor(p_id, true)
	#queue_redraw_connections()
	#queue_graph_redraw()


func _on_page_node_deleted(p_id: String) -> void:
	if not p_id.is_empty():
		var arr := PackedStringArray()
		arr.push_back(p_id)
		_on_page_nodes_deleted(arr)


func _on_page_nodes_deleted(p_ids: PackedStringArray) -> void:
	clear_connections()
	
	for id in p_ids:
		var node_editor: FlowNodeEditor = get_node_editor_by_id(id)
		
		if node_editor != null:
			node_editor.queue_free()
	
	for id in p_ids:
		_node_editor_map.erase(id)
	
	_redraw_graph_connections()


func _request_flow_node_rename_dialog(p_node_id: String) -> void:
	flow_node_rename_dialog_requested.emit(p_node_id)


func _on_page_node_renamed(p_from_id: String, p_to_id: String) -> void:
	queue_graph_redraw()
