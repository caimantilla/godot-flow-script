@tool
extends FlowNodeEditor
## An editor for the multi-branch execution nodes.
## It contains a slot for the next node after the branches finish, then a slot for each branch.
## Additionally, it contains an extra slot at the end which doesn't actually exist yet.
## When this slot becomes active, the execution stack is resized to accommodate it, and another one like it is added beneath.


const MultiBranchExecuteNode := preload("../objects/_multi_branch_execute.gd")


var _execution_stack_gui_entries: Array[Control] = []


func _set_outgoing_connection(p_connection: FlowNodeEditorOutgoingConnectionParameters) -> void:
	var mbe_node: MultiBranchExecuteNode = get_multi_branch_execute_node()
	if mbe_node == null:
		return
	
	var open_entries_count: int = get_output_port_count() - 1 # subtract 1 to account for the first port which is just for the next node after all the branches are done
	
	if p_connection.get_port() == (get_output_port_count()):
		mbe_node.resize_execution_stack(open_entries_count)


func get_multi_branch_execute_node() -> MultiBranchExecuteNode:
	return get_flow_node() as MultiBranchExecuteNode


func _clear_execution_stack_gui() -> void:
	for node in _execution_stack_gui_entries:
		remove_child(node)
		node.queue_free()
	_execution_stack_gui_entries.clear()


func _create_execution_stack_entry_gui_component(p_idx: int, p_last: bool) -> VBoxContainer:
	var entry := VBoxContainer.new()
	
	var label := Label.new()
	label.set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT)
	label.set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER)
	label.set_text("Branch %d:" % (p_idx + 1))
	entry.add_child(label, true)
	
	if not p_last:
		entry.add_child(HSeparator.new())
	
	_execution_stack_gui_entries.append(entry)
	return entry


func _reload_execution_stack_gui() -> void:
	_clear_execution_stack_gui()
	
	var mbe_node: MultiBranchExecuteNode = get_multi_branch_execute_node()
	if mbe_node == null:
		return
	
	var stack_size: int = mbe_node.get_execution_stack_size()
	
	for entry_idx: int in stack_size:
		_create_execution_stack_entry_gui_component(entry_idx, false)
	
	if stack_size < MultiBranchExecuteNode.MAX_STACK_SIZE:
		_create_execution_stack_entry_gui_component(stack_size, true)
