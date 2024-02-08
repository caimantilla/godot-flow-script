@tool
class_name FlowNodeCreationDialog
extends ConfirmationDialog


signal flow_node_type_chosen(p_type_id: String)


var _tree: FlowNodeCreationTree


func _init() -> void:
	set_title("Create New FlowNode")
	
	_tree = FlowNodeCreationTree.new()
	_tree.set_name("FlowNode Tree")
	_tree.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	_tree.set_h_size_flags(Control.SIZE_EXPAND_FILL)
	_tree.set_v_size_flags(Control.SIZE_EXPAND_FILL)
	_tree.item_activated.connect(_notify_selected_type_chosen)
	add_child(_tree, true)
	
	confirmed.connect(_notify_selected_type_chosen)
	
	visibility_changed.connect(_on_visibility_changed)


func get_selected_flow_node_type() -> String:
	return _tree.get_selected_flow_node_type()


func _notify_selected_type_chosen() -> void:
	var selected_type: String = get_selected_flow_node_type()
	if not selected_type.is_empty():
		flow_node_type_chosen.emit(selected_type)


func _on_visibility_changed() -> void:
	if is_visible():
		_tree.grab_focus.call_deferred()
