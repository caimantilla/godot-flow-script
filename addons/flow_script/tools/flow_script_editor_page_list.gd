@tool
class_name FlowScriptEditorPageList
extends ItemList




signal page_selected(page_id: String)
signal page_creation_requested()
signal page_rename_requested(page_id: String)
signal page_deletion_requested(page_id: String)




const MIN_SIZE := Vector2(120, 100)




var flow_script: FlowScript: set = set_flow_script, get = get_flow_script
var current_page: String: set = set_page, get = get_page

var _flow_script: FlowScript = null
var _current_page: String = ""
var _page_context_menu: FlowScriptPageListPageContextMenu

var _new_page_request_item_idx: int = -1




func _init() -> void:
	
	var min_size: Vector2 = (MIN_SIZE * EditorInterface.get_editor_scale()).floor() if Engine.is_editor_hint() else MIN_SIZE
	set_custom_minimum_size(min_size)
	set_allow_reselect(true)
	
	_page_context_menu = FlowScriptPageListPageContextMenu.new()
	_page_context_menu.set_name("Page Context Menu")
	_page_context_menu.rename_requested.connect(_on_context_menu_rename_requested)
	_page_context_menu.deletion_requested.connect(_on_context_menu_deletion_requested)
	add_child(_page_context_menu, true)
	
	item_selected.connect(_on_item_selected)
	item_clicked.connect(_on_item_clicked)





func set_flow_script(p_script: FlowScript) -> void:
	if _flow_script != null:
		_flow_script.page_created.disconnect(_reload_pages)
		_flow_script.page_deleted.disconnect(_reload_pages)
		
		_flow_script.page_created.disconnect(select_page)
	
	_flow_script = p_script
	
	if p_script != null:
		p_script.page_created.connect(_reload_pages.unbind(1))
		p_script.page_deleted.connect(_reload_pages.unbind(1))
		
		# The new page should automatically be selected upon creation.
		p_script.page_created.connect(select_page, CONNECT_DEFERRED)
	
	_reload_pages()


func get_flow_script() -> FlowScript:
	return _flow_script


func set_page(p_page_id: String) -> void:
	if p_page_id == _current_page:
		return
	
	select_page(p_page_id)

func get_page() -> String:
	return _current_page




func select_page(p_page_id: String) -> void:
	
	for item_idx: int in get_item_count():
		
		var item_meta: Variant = get_item_metadata(item_idx)
		
		if typeof(item_meta) == TYPE_STRING \
		and item_meta == p_page_id:
			
			select(item_idx, true)
			_on_item_selected(item_idx) # docs say that item_selected isn't emitted automatically by select()
			
			break


func get_selected_page_id() -> String:
	var selected_page_id: String = ""
	
	var sel = get_selected_items()
	
	if sel.size() > 0:
		var selection_index: int = sel[0]
		selected_page_id = get_page_id_at(selection_index)
	
	return selected_page_id


func get_page_id_at(p_idx: int) -> String:
	if p_idx > -1 and p_idx < get_item_count() \
	and p_idx != _new_page_request_item_idx:
		var meta: Variant = get_item_metadata(p_idx)
		if typeof(meta) == TYPE_STRING:
			return meta as String
	
	return ""


func _clear_pages() -> void:
	clear()



func _reload_pages() -> void:
	_clear_pages()
	
	if _flow_script == null:
		return
	
	for page: FlowPage in _flow_script.get_page_list():
		var page_id: String = page.get_id()
		var item_idx: int = add_item(page_id, null, true)
		set_item_metadata(item_idx, page_id)
	
	if _flow_script.get_page_count() < FlowScript.MAX_PAGES:
		var add_icon: Texture2D = null
		
		if has_theme_icon(&"Add", &"EditorIcons"):
			add_icon = get_theme_icon(&"Add", &"EditorIcons")
		
		var new_page_item_idx: int = add_item("New Page", add_icon, true)
		set_item_tooltip(new_page_item_idx, "Request creation of a new page.")
		
		_new_page_request_item_idx = new_page_item_idx
	else:
		_new_page_request_item_idx = -1


func _on_item_selected(p_item_idx: int) -> void:
	if p_item_idx == _new_page_request_item_idx:
		page_creation_requested.emit()
	else:
		var item_meta: Variant = get_item_metadata(p_item_idx)
		if typeof(item_meta) == TYPE_STRING:
			_current_page = item_meta
			page_selected.emit(item_meta as String)


func _on_item_clicked(p_item_idx: int, p_position: Vector2, p_button_idx: int) -> void:
	if p_button_idx == MOUSE_BUTTON_RIGHT:
		var clicked_page_id: String = get_page_id_at(p_item_idx)
		
		if not clicked_page_id.is_empty():
			grab_focus()
			
			_page_context_menu.set_selected_page_id(clicked_page_id)
			
			var click_screen_pos: Vector2 = p_position + get_screen_position()
			var click_screen_pos_i := Vector2i(click_screen_pos.round())
			
			_page_context_menu.popup( Rect2i(click_screen_pos_i, Vector2i.ZERO) )


func _on_context_menu_rename_requested() -> void:
	page_rename_requested.emit(_page_context_menu.get_selected_page_id())


func _on_context_menu_deletion_requested() -> void:
	page_deletion_requested.emit(_page_context_menu.get_selected_page_id())
