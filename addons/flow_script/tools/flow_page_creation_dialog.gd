@tool
class_name FlowPageCreationDialog
extends ConfirmationDialog




var _invalid_id_alert: AcceptDialog
var _id_entry_line_edit: LineEdit




func _init() -> void:
	set_title("Create New Page")
	set_hide_on_ok(false) # Auto-hide is not suitable as it prevents showing error
	
	
	_invalid_id_alert = AcceptDialog.new()
	_invalid_id_alert.set_name("Invalid ID Alert")
	add_child(_invalid_id_alert, true)
	
	_id_entry_line_edit = LineEdit.new()
	_id_entry_line_edit.set_name("ID Entry")
	add_child(_id_entry_line_edit, true)
	
	visibility_changed.connect(_on_visibility_changed)


func _on_visibility_changed() -> void:
	if is_visible():
		_id_entry_line_edit.grab_focus.call_deferred()
	else:
		_id_entry_line_edit.clear.call_deferred()



func get_new_id() -> String:
	return _id_entry_line_edit.get_text()


func show_invalid_id_alert() -> void:
	_invalid_id_alert.set_text('New page ID "%s" is invalid.' % get_new_id())
	_invalid_id_alert.popup_centered()
