@tool
class_name FlowNodeIdEntryDialog
extends ConfirmationDialog


## The ID of the new type to be made.
## A bit hacky to store it here... Oh well.
var new_type_id: String: set = set_new_type_id, get = get_new_type_id


var _id_edit: LineEdit
var _invalid_id_alert: AcceptDialog


var _new_type_id: String = ""


func _init() -> void:
	set_title("Enter Node ID")
	set_hide_on_ok(false)
	
	_id_edit = LineEdit.new()
	_id_edit.set_name("ID Line Edit")
	add_child(_id_edit, true)
	
	_invalid_id_alert = AcceptDialog.new()
	_invalid_id_alert.set_name("Invalid ID Alert")
	add_child(_invalid_id_alert, true)
	
	visibility_changed.connect(_on_visibility_changed)


func get_new_id() -> String:
	return _id_edit.get_text()


func _on_visibility_changed() -> void:
	if not is_visible():
		_id_edit.clear.call_deferred()


func alert_invalid_id() -> void:
	_invalid_id_alert.set_text('New Node ID "%s" is invalid.' % get_new_id())
	_invalid_id_alert.popup_centered()



func set_new_type_id(p_id: String) -> void:
	_new_type_id = p_id

func get_new_type_id() -> String:
	return _new_type_id
