@tool
class_name FlowNodeCreationTree
extends Tree






var _category_map: Dictionary = {}
var _type_map: Dictionary = {}





func _ready() -> void:
	_reload_types()



func _notification(what: int) -> void:
	if what == NOTIFICATION_VISIBILITY_CHANGED:
		_reload_types()



func get_selected_flow_node_type() -> String:
	var selected_item: TreeItem = get_selected()
	var item_meta: Variant = selected_item.get_metadata(0)
	
	if typeof(item_meta) == TYPE_STRING:
		return item_meta
	else:
		return ""




func _clear_types() -> void:
	_category_map.clear()
	_type_map.clear()
	clear()



func _reload_types() -> void:
	if not is_node_ready():
		return
	
	_clear_types()
	
	if not is_visible_in_tree():
		return
	
	
	var root_item: TreeItem = create_item()
	root_item.set_text(0, "Nodes")
	root_item.set_selectable(0, false)
	# TODO: Find a better icon for the root
	#root_item.set_icon(0, get_theme_icon(&"GraphElement", &"EditorIcons"))
	
	
	var type_list: Array[FlowNodeTypeInfo] = FlowNodeTypeDB.get_type_list()
	
	for type in type_list:
		var id: String = type.get_id()
		var type_name: String = type.get_name()
		var category: String = type.get_category()
		var description: String = type.get_description()
		var icon_texture: Texture2D = type.get_icon_texture()
		
		
		_create_category_if_needed(category)
		
		
		var parent_item: TreeItem = null
		
		
		if _category_map.has(category):
			parent_item = _category_map[category]
		else:
			parent_item = get_root()
		
		
		var type_item: TreeItem = parent_item.create_child()
		type_item.set_text(0, type_name)
		type_item.set_tooltip_text(0, description)
		# NOTE: The type ID is stored as the type's metadata.
		type_item.set_metadata(0, id)
		type_item.set_icon(0, icon_texture)
		
		_type_map[id] = type_item



func _create_category_if_needed(p_category_path: String) -> void:
	if p_category_path.strip_edges().is_empty():
		_category_map[p_category_path] = get_root()
		return
	
	var categories: PackedStringArray = p_category_path.split("/")
	
	for current_category_depth: int in categories.size():
		var current_category_name_part: String = categories[current_category_depth]
		var current_category_name: String = "/".join(categories.slice(0, current_category_depth + 1))
		
		if not _category_map.has(current_category_name):
			
			var parent_item: TreeItem = null
			
			var previous_category_range: Array = range(current_category_depth - 1, -1, -1)
			
			for previous_category_depth: int in previous_category_range:
				var previous_category_name: String = "/".join(categories.slice(0, previous_category_depth + 1))
				
				if _category_map.has(previous_category_name):
					parent_item = _category_map[previous_category_name] as TreeItem
					break
			
			if parent_item == null:
				parent_item = get_root()
			
			var current_category_item: TreeItem = parent_item.create_child()
			current_category_item.set_text(0, current_category_name_part)
			current_category_item.set_selectable(0, false)
			
			_category_map[current_category_name] = current_category_item
