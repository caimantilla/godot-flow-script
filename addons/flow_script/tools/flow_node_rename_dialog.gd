@tool
class_name FlowNodeRenameDialog
extends ConfirmationDialog


var target_node_id: String: set = set_target_node_id, get = get_target_node_id


var _fail_alert: AcceptDialog
var _id_entry: LineEdit
var _target_node_id: String = ""


func _init() -> void:
	set_hide_on_ok(false)
	
	_fail_alert = AcceptDialog.new()
	_fail_alert.set_name("Fail Alert")
	_fail_alert.set_text("Failed to rename the node.")
	add_child(_fail_alert, true)
	
	_id_entry = LineEdit.new()
	_id_entry.set_name("ID Entry")
	add_child(_id_entry, true)


func _on_visibility_changed() -> void:
	if not is_visible():
		_id_entry.clear()



func set_target_node_id(p_id: String) -> void:
	_target_node_id = p_id
	_id_entry.set_text(p_id)
	_update_title()

func get_target_node_id() -> String:
	return _target_node_id


func get_new_node_id() -> String:
	return _id_entry.get_text()


func alert_failed() -> void:
	_fail_alert.popup_centered()


func _update_title() -> void:
	if _target_node_id.is_empty():
		set_title("Rename Node ???")
	else:
		set_title("Rename Node %s" % _target_node_id)
