@tool
extends FlowNodeEditor
## An editor for the multi-branch execution nodes.
## It contains a slot for the next node after the branches finish, then a slot for each branch.
## Additionally, it contains an extra slot at the end which doesn't actually exist yet.
## When this slot becomes active, the execution stack is resized to accommodate it, and another one like it is added beneath.


const MultiBranchExecuteNode := preload("../objects/_multi_branch_execute.gd")

const SLOT_NEXT_NODE: int = 0
const SLOT_STACK_BEGIN: int = 2


var _execution_stack_gui_entries: Array[Control] = []


func _on_node_updated() -> void:
	_reload_execution_stack_gui()


func _get_input_slot() -> int:
	return 0


func _set_outgoing_connection(p_connection: FlowNodeEditorOutgoingConnectionParameters) -> void:
	var mbe_node: MultiBranchExecuteNode = get_multi_branch_execute_node()
	if mbe_node == null:
		return
	
	if p_connection.get_slot() == SLOT_NEXT_NODE:
		mbe_node.set_next_node_id(p_connection.get_node_id())
	else:
		var current_stack_size: int = mbe_node.get_execution_stack_size()
		var entry_idx: int = p_connection.get_slot() - SLOT_STACK_BEGIN
		var connection_id: String = p_connection.get_node_id()
		
		if connection_id.is_empty():
			if entry_idx < current_stack_size:
				mbe_node.remove_execution_stack_entry_at(entry_idx)
		
		else:
			if entry_idx >= current_stack_size:
				mbe_node.resize_execution_stack(entry_idx + 1)
			
			mbe_node.set_execution_stack_entry_at(entry_idx, connection_id)


func _get_outgoing_connections() -> Array[FlowNodeEditorOutgoingConnectionParameters]:
	var connections: Array[FlowNodeEditorOutgoingConnectionParameters] = []
	
	var mbe_node: MultiBranchExecuteNode = get_multi_branch_execute_node()
	
	if mbe_node != null:
		if not mbe_node.get_next_node_id().is_empty():
			var conn := FlowNodeEditorOutgoingConnectionParameters.new()
			conn.set_node_id(mbe_node.get_next_node_id())
			conn.set_slot(SLOT_NEXT_NODE)
			connections.append(conn)
		
		for entry_idx: int in mbe_node.get_execution_stack_size():
			var entry: String = mbe_node.get_execution_stack_entry_at(entry_idx)
			if not entry.is_empty():
				var conn := FlowNodeEditorOutgoingConnectionParameters.new()
				conn.set_node_id(entry)
				conn.set_slot(SLOT_STACK_BEGIN + entry_idx)
				connections.append(conn)
	
	return connections


func get_multi_branch_execute_node() -> MultiBranchExecuteNode:
	return get_flow_node() as MultiBranchExecuteNode


func _clear_execution_stack_gui() -> void:
	for node in _execution_stack_gui_entries:
		remove_child(node)
		node.queue_free()
	_execution_stack_gui_entries.clear()


func _create_execution_stack_entry_gui_component(p_idx: int, p_last: bool) -> VBoxContainer:
	var visible_idx: int = p_idx + 1
	
	var entry := VBoxContainer.new()
	entry.set_name("Branch %d" % visible_idx)
	
	var label := Label.new()
	label.set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT)
	label.set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER)
	entry.add_child(label, true)
	
	if p_last:
		label.set_text("New Branch:")
	else:
		label.set_text("Branch %d:" % visible_idx)
		entry.add_child(HSeparator.new())
	
	_execution_stack_gui_entries.append(entry)
	add_child(entry, true)
	return entry


func _reload_execution_stack_gui() -> void:
	_clear_execution_stack_gui()
	
	var mbe_node: MultiBranchExecuteNode = get_multi_branch_execute_node()
	if mbe_node == null:
		return
	
	var stack_size: int = mbe_node.get_execution_stack_size()
	
	for entry_idx: int in stack_size + 1:
		var is_last: bool = entry_idx == stack_size
		var gui_node: Control = _create_execution_stack_entry_gui_component(entry_idx, is_last)
		var gui_node_idx: int = gui_node.get_index(false)
		set_slot_enabled_right(gui_node_idx, true)
