@tool
class_name FlowScriptEditorPanel
extends MarginContainer



signal save_requested()

## Notifies listeners that a node was selected in the node graph.
signal flow_node_selected(p_node_id: String)

## Notifies listeners that the page has been changed.
signal flow_page_changed()



enum NodeCreationDialogAppearPosition {
	CENTER = 0,
	MOUSE = 1,
}



const MARGIN: int = 0
const NODE_CREATION_DIALOG_SIZE := Vector2i(512, 384)






var flow_script: FlowScript: set = set_flow_script, get = get_flow_script





var _flow_script: FlowScript = null

var _main_vbox: VBoxContainer
var _no_resource_container: MarginContainer
var _editor_container: MarginContainer
var _editor_half_split: HSplitContainer

var _node_creation_dialog: FlowNodeCreationDialog
var _page_creation_dialog: FlowPageCreationDialog
var _node_creation_failed_alert: AcceptDialog
var _node_id_entry_dialog: FlowNodeIdEntryDialog
var _node_rename_dialog: FlowNodeRenameDialog
var _editor_page_list: FlowScriptEditorPageList
var _graph_menu: FlowScriptEditorGraphMenu
var _graph: FlowScriptEditorGraph
var _flow_script_path_label: Label
var _save_button: Button

var _next_node_position := Vector2.ZERO







func _init() -> void:
	var margin: int = floori(float(MARGIN) * EditorInterface.get_editor_scale()) if Engine.is_editor_hint() else MARGIN
	
	for margin_name: StringName in [ &"margin_left", &"margin_top", &"margin_right", &"margin_bottom" ]:
		add_theme_constant_override(margin_name, margin)
	
	
	
	_node_creation_dialog = FlowNodeCreationDialog.new()
	_node_creation_dialog.set_name("Node Creation Dialog")
	_node_creation_dialog.flow_node_type_chosen.connect(_on_node_creation_dialog_type_chosen)
	add_child(_node_creation_dialog, true)
	
	_page_creation_dialog = FlowPageCreationDialog.new()
	_page_creation_dialog.set_name("Page Creation Dialog")
	_page_creation_dialog.confirmed.connect(_on_page_creation_dialog_confirmed)
	add_child(_page_creation_dialog, true)
	
	_node_creation_failed_alert = AcceptDialog.new()
	_node_creation_failed_alert.set_name("Node Creation Failed")
	_node_creation_failed_alert.set_text("Failed to create node.")
	add_child(_node_creation_failed_alert, true)
	
	_node_id_entry_dialog = FlowNodeIdEntryDialog.new()
	_node_id_entry_dialog.set_name("Node ID Entry Dialog")
	_node_id_entry_dialog.confirmed.connect(_on_node_id_entry_dialog_confirmed)
	add_child(_node_id_entry_dialog, true)
	
	_node_rename_dialog = FlowNodeRenameDialog.new()
	_node_rename_dialog.set_name("Node Rename Dialog")
	_node_rename_dialog.confirmed.connect(_on_node_rename_dialog_confirmed)
	add_child(_node_rename_dialog, true)
	
	
	
	_main_vbox = VBoxContainer.new()
	_main_vbox.set_name("Main VBox")
	_main_vbox.set_anchors_and_offsets_preset(PRESET_FULL_RECT)
	_main_vbox.set_h_size_flags(SIZE_EXPAND_FILL)
	_main_vbox.set_v_size_flags(SIZE_EXPAND_FILL)
	add_child(_main_vbox, true)
	
	var top_menu := HBoxContainer.new()
	top_menu.set_name("Top Menu")
	top_menu.set_h_size_flags(SIZE_EXPAND_FILL)
	top_menu.set_v_size_flags(SIZE_SHRINK_BEGIN)
	_main_vbox.add_child(top_menu, true)
	
	_save_button = Button.new()
	_save_button.set_name("Save Script")
	_save_button.set_tooltip_text("Save the script to disk.")
	_save_button.pressed.connect(request_save, CONNECT_DEFERRED)
	top_menu.add_child(_save_button, true)
	
	_flow_script_path_label = Label.new()
	_flow_script_path_label.set_name("Script Path")
	top_menu.add_child(_flow_script_path_label, true)
	
	_no_resource_container = MarginContainer.new()
	_no_resource_container.set_name("No Resource Container")
	_no_resource_container.set_anchors_and_offsets_preset(PRESET_FULL_RECT)
	_no_resource_container.set_h_size_flags(SIZE_EXPAND_FILL)
	_no_resource_container.set_v_size_flags(SIZE_EXPAND_FILL)
	add_child(_no_resource_container, true)
	
	var no_resource_label := Label.new()
	no_resource_label.set_name("No Resource Label")
	no_resource_label.set_text("Select a FlowScript resource to create and edit flow scripts.")
	no_resource_label.set_h_size_flags(SIZE_EXPAND_FILL)
	no_resource_label.set_v_size_flags(SIZE_EXPAND_FILL)
	no_resource_label.set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER)
	no_resource_label.set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER)
	_no_resource_container.add_child(no_resource_label, true)
	
	_editor_container = MarginContainer.new()
	_editor_container.set_name("Editor Container")
	_editor_container.set_anchors_and_offsets_preset(PRESET_FULL_RECT)
	_editor_container.set_h_size_flags(SIZE_EXPAND_FILL)
	_editor_container.set_v_size_flags(SIZE_EXPAND_FILL)
	_main_vbox.add_child(_editor_container, true)
	
	_editor_half_split = HSplitContainer.new()
	_editor_half_split.set_name("Half Split")
	_editor_half_split.set_h_size_flags(SIZE_EXPAND_FILL)
	_editor_half_split.set_v_size_flags(SIZE_EXPAND_FILL)
	_editor_container.add_child(_editor_half_split, true)
	
	_editor_page_list = FlowScriptEditorPageList.new()
	_editor_page_list.set_name("Pages")
	_editor_page_list.set_h_size_flags(SIZE_FILL)
	_editor_page_list.set_v_size_flags(SIZE_EXPAND_FILL)
	_editor_page_list.page_selected.connect(_on_page_selected)
	_editor_page_list.page_creation_requested.connect(show_page_creation_dialog)
	_editor_page_list.page_deletion_requested.connect(delete_page)
	# TODO: Add a rename dialog that responds to _editor_page_list.page_rename_requested
	_editor_half_split.add_child(_editor_page_list, true)
	
	_graph_menu = FlowScriptEditorGraphMenu.new()
	_graph_menu.set_name("Menu")
	_graph_menu.set_anchors_and_offsets_preset(PRESET_FULL_RECT)
	_graph_menu.set_h_size_flags(SIZE_EXPAND_FILL)
	_graph_menu.set_v_size_flags(SIZE_EXPAND_FILL)
	_graph_menu.node_creation_dialog_requested.connect(show_node_creation_dialog.bind(NodeCreationDialogAppearPosition.CENTER))
	_editor_half_split.add_child(_graph_menu, true)
	
	
	_graph = FlowScriptEditorGraph.new()
	_graph.set_name("Graph")
	_graph.set_h_size_flags(SIZE_EXPAND_FILL)
	_graph.set_v_size_flags(SIZE_EXPAND_FILL)
	# TODO: Hook up more signals for node creation, including automatic connection when dragged from empty. That should be possible, right...?
	_graph.popup_request.connect(show_node_creation_dialog.bind(NodeCreationDialogAppearPosition.MOUSE).unbind(1))
	_graph.flow_node_selected.connect(_emit_flow_node_selected)
	_graph.flow_nodes_deletion_requested.connect(_delete_nodes)
	_graph.flow_node_rename_dialog_requested.connect(show_node_rename_dialog, CONNECT_DEFERRED)
	
	_graph_menu.add_child(_graph, true)
	_graph_menu.move_child(_graph, 0)



func _notification(what: int) -> void:
	if what == NOTIFICATION_THEME_CHANGED:
		_update_style()


func _ready() -> void:
	_editor_half_split.set_split_offset(0)
	_update_style()
	_on_flow_script_changed()


func _update_style() -> void:
	var save_icon: Texture2D = null
	
	if has_theme_icon(&"Save", &"EditorIcons"):
		save_icon = get_theme_icon(&"Save", &"EditorIcons")
	
	_save_button.set_button_icon(save_icon)




#func handles(p_obj: Object) -> bool:
	#if p_obj is FlowScript:
		#return true
	#
	#return false
#
#
#func edit(p_obj: Object) -> void:
	#if not p_obj is FlowScript:
		#return







func set_flow_script(p_script: FlowScript) -> void:
	if p_script != _flow_script:
		if _flow_script != null:
			_flow_script.changed.disconnect(_on_flow_script_changed)
			_flow_script.deleting_page.disconnect(_on_flow_script_page_predelete)
		
		_flow_script = p_script
		
		if p_script != null:
			p_script.changed.connect(_on_flow_script_changed)
			p_script.deleting_page.connect(_on_flow_script_page_predelete)
		
		_editor_page_list.set_flow_script(p_script)
		if not p_script.get_page_count() < 1:
			_editor_page_list.select_page(p_script.get_page_ids()[0])
		
		_on_flow_script_changed()


func get_flow_script() -> FlowScript:
	return _flow_script



func get_graph() -> GraphEdit:
	return _graph


func get_current_page_id() -> String:
	return _editor_page_list.get_page()


func get_current_page() -> FlowPage:
	if _flow_script != null:
		return _flow_script.get_page_by_id(get_current_page_id())
	
	return null


func delete_page(p_page_id: String) -> void:
	if _flow_script != null:
		_flow_script.delete_page(p_page_id)


func show_node_creation_dialog(appear_position_mode: NodeCreationDialogAppearPosition) -> void:
	prints("Page:", get_current_page())
	
	if _node_creation_dialog.is_visible() or get_current_page() == null:
		return
	
	
	update_next_node_position()
	
	
	var dialog_size_float := Vector2(NODE_CREATION_DIALOG_SIZE)
	if Engine.is_editor_hint():
		dialog_size_float *= EditorInterface.get_editor_scale()
	
	var dialog_size_integer := Vector2i(dialog_size_float.round())
	
	
	
	match appear_position_mode:
		
		NodeCreationDialogAppearPosition.CENTER:
			_node_creation_dialog.popup_centered(dialog_size_integer)
		
		NodeCreationDialogAppearPosition.MOUSE:
			var dialog_origin: Vector2 = get_global_mouse_position() - (dialog_size_float * 0.5)
			
			var dialog_rect := Rect2i(Vector2i(dialog_origin.round()), dialog_size_integer)
			_node_creation_dialog.popup(dialog_rect)



## Updates the point on the graph where the next node should be instantiated.
## Bear in mind that, when the time comes to place the node, this should be adjusted by half of the node's size as well.
## This is just the mouse position in the graph.
func update_next_node_position() -> void:
	var new_pos: Vector2 = _graph.get_local_mouse_position()
	new_pos += _graph.get_scroll_offset()
	
	if not is_zero_approx(_graph.get_zoom()):
		new_pos /= _graph.get_zoom()
	
	if Engine.is_editor_hint():
		new_pos /= EditorInterface.get_editor_scale()
	
	new_pos = new_pos.round()
	
	_next_node_position = new_pos


func show_page_creation_dialog() -> void:
	print("Page creation requested.")
	_page_creation_dialog.popup_centered()


func show_node_rename_dialog(p_for_node_id: String) -> void:
	_node_rename_dialog.set_target_node_id(p_for_node_id)
	_node_rename_dialog.popup_centered()


func request_save() -> void:
	save_requested.emit()


func _emit_flow_node_selected(p_node_id: String) -> void:
	flow_node_selected.emit(p_node_id)

func _emit_flow_page_changed() -> void:
	flow_page_changed.emit()


func _on_flow_script_changed() -> void:
	if _flow_script == null:
		_flow_script_path_label.set_text("")
		_main_vbox.hide()
		_no_resource_container.show()
		_graph.set_page(null)
	else:
		_flow_script_path_label.set_text("Editing: " + _flow_script.get_path())
		_no_resource_container.hide()
		_main_vbox.show()
		
		# The script could be saved as a subresource rather than its own resource.
		# Depending on the type of game, this might actually be the primary use case, eg. a script per NPC.
		# For these cases, certain things pertaining to the filesystem should be disabled.
		var path_valid: bool = ResourceLoader.exists(_flow_script.get_path()) or FileAccess.file_exists(_flow_script.get_path())
		
		# The saving would be done when saving the resource which owns the script, so the save button isn't needed.
		_save_button.disabled = not path_valid


func _on_page_selected(p_page_id: String) -> void:
	if _flow_script == null:
		_graph.set_page(null)
	else:
		var page: FlowPage = _flow_script.get_page_by_id(p_page_id)
		_graph.set_page(page)


func _on_page_creation_dialog_confirmed() -> void:
	if _flow_script == null:
		_page_creation_dialog.hide()
		return
	
	var new_page_id: String = _page_creation_dialog.get_new_id()
	
	if _flow_script.is_new_page_id_valid(new_page_id):
		_page_creation_dialog.hide()
		_flow_script.create_page(new_page_id)
	else:
		_page_creation_dialog.show_invalid_id_alert()


func _on_node_id_entry_dialog_confirmed() -> void:
	if _flow_script == null or get_current_page() == null:
		_node_id_entry_dialog.hide()
		return
	
	var page_id: String = get_current_page_id()
	
	var node_id: String = _node_id_entry_dialog.get_new_id()
	var type_id: String = _node_id_entry_dialog.get_new_type_id()
	
	if _flow_script.is_new_node_id_valid(node_id):
		_node_id_entry_dialog.hide()
		_create_new_node(type_id, node_id)
	else:
		_node_id_entry_dialog.alert_invalid_id()


func _on_node_creation_dialog_type_chosen(p_type_id: String) -> void:
	if _node_creation_dialog.is_visible():
		_node_creation_dialog.hide()
	
	if _flow_script != null and get_current_page() != null:
		var type_info: FlowNodeTypeInfo = FlowNodeTypeDB.get_type(p_type_id)
		
		if type_info != null:
			if type_info.is_id_automatically_assigned():
				
				var new_id: String = _flow_script.generate_new_node_id()
				_create_new_node(p_type_id, new_id)
			
			else:
				_node_id_entry_dialog.set_new_type_id(p_type_id)
				_node_id_entry_dialog.popup_centered()


func _create_new_node(p_type_id: String, p_node_id: String) -> void:
	var page: FlowPage = get_current_page()
	if page != null:
		var node: FlowNode = page.create_node(p_type_id, p_node_id)
		
		if node == null:
			_node_creation_failed_alert.popup_centered()
		else:
			var node_editor: FlowNodeEditor = _graph.get_node_editor_by_id(p_node_id)
			if node_editor != null:
				#get_tree().process_frame.connect(_node_editor_set_position_mouse_center.bind(p_node_id), CONNECT_ONE_SHOT | CONNECT_DEFERRED)
				node_editor.draw.connect(_node_editor_set_position_mouse_center.bind(p_node_id), CONNECT_ONE_SHOT | CONNECT_DEFERRED)


# Hook this up to the draw signal, so it's finished expanding and all.
func _node_editor_set_position_mouse_center(p_node_id: String) -> void:
	#await get_tree().process_frame
	
	var node_editor: FlowNodeEditor = _graph.get_node_editor_by_id(p_node_id)
	print("ID: %s\nEditor: %s" % [ p_node_id, str(node_editor) ])
	
	if node_editor != null:
		var node: FlowNode = node_editor.get_flow_node()
		
		var new_pos: Vector2 = _next_node_position
		
		var node_half_size: Vector2 = node_editor.get_size() * 0.5
		if Engine.is_editor_hint():
			node_half_size /= EditorInterface.get_editor_scale()
		
		new_pos -= node_half_size
		
		var new_pos_i := Vector2i(new_pos.floor())
		#print("Current position: %s\nNew Position: %s" % [ str(node.get_graph_position()), str(new_pos_i) ])
		node.set_graph_position(new_pos_i)


func _delete_nodes(p_node_ids: PackedStringArray) -> void:
	var page: FlowPage = get_current_page()
	if page != null:
		page.delete_nodes(p_node_ids)


func _on_node_rename_dialog_confirmed() -> void:
	var target_node_id: String = _node_rename_dialog.get_target_node_id()
	var new_id: String = _node_rename_dialog.get_new_node_id()
	
	if _flow_script != null and _flow_script.has_node_with_id(target_node_id) \
	and _flow_script.is_new_node_id_valid(new_id) \
	and _flow_script.rename_node(target_node_id, new_id):
		_node_rename_dialog.hide()
	else:
		_node_rename_dialog.alert_failed()


func _on_node_renamed(p_from_id: String, p_to_id: String) -> void:
	_graph.queue_graph_redraw()


func _on_flow_script_page_predelete(p_page_id: String) -> void:
	#if get_current_page_id() == p_page_id:
	_graph.set_page(null)
