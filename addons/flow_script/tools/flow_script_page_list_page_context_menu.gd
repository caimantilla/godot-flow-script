@tool
class_name FlowScriptPageListPageContextMenu
extends PopupMenu


signal rename_requested()
signal deletion_requested()


enum Item {
	RENAME = 0,
	DELETE = 1,
}


var selected_page_id: String: set = set_selected_page_id, get = get_selected_page_id


var _selected_page_id: String = ""


func _init() -> void:
	add_item("Rename...", Item.RENAME)
	set_item_tooltip(Item.RENAME, "Rename the selected page.")
	set_item_disabled(Item.RENAME, true) # Don't feel like implementing renaming yet
	
	add_item("Delete", Item.DELETE)
	set_item_tooltip(Item.DELETE, "Delete the selected page.")
	
	id_pressed.connect(_on_id_pressed)


func _notification(what: int) -> void:
	if what == NOTIFICATION_READY or what == NOTIFICATION_THEME_CHANGED:
		_update_style()


func _update_style() -> void:
	set_item_icon(Item.RENAME, get_theme_icon(&"Rename", &"EditorIcons"))
	set_item_icon(Item.DELETE, get_theme_icon(&"Remove", &"EditorIcons"))


func set_selected_page_id(p_id: String) -> void:
	_selected_page_id = p_id

func get_selected_page_id() -> String:
	return _selected_page_id


func _on_id_pressed(p_id: int) -> void:
	var id := p_id as Item
	match id:
		Item.RENAME:
			rename_requested.emit()
		Item.DELETE:
			deletion_requested.emit()
