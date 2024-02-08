@tool
class_name FlowScript
extends Resource
## Contains flow data...?




signal page_created(page_id: String)
signal deleting_page(page_id: String)
signal page_deleted(page_id: String)

signal node_created(node_id: String)
signal deleting_node(node_id: String)
signal node_deleted(node_id: String)
signal nodes_deleted(node_ids: PackedStringArray)
signal node_renamed(from_id: String, to_id: String)




const MAX_PAGES: int = 9




var _page_list: Array[FlowPage] = []
var _page_id_map: Dictionary = {}



func _init() -> void:
	#deleting_node.connect(_node_predelete)
	node_deleted.connect(_node_postdelete)
	nodes_deleted.connect(_nodes_postdelete)


func _notification(what: int) -> void:
	if what == NOTIFICATION_PREDELETE:
		for page in _page_list:
			page.free()
		_page_list.clear()
		_page_id_map.clear()



func _get_property_list() -> Array[Dictionary]:
	var properties: Array[Dictionary] = []
	
	properties.append({
		"name": "data",
		"type": TYPE_DICTIONARY,
		"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE,
	})
	
	return properties


func _set(property: StringName, value: Variant) -> bool:
	if property == &"data":
		_set_data_serializable(value)
		return true
	
	return false


func _get(property: StringName) -> Variant:
	if property == &"data":
		return _get_data_serializable()
	
	return null




func has_page_with_id(p_page_id: String) -> bool:
	return _page_id_map.has(p_page_id)


func get_page_by_id(p_page_id: String) -> FlowPage:
	if _page_id_map.has(p_page_id):
		return _page_list[_page_id_map[p_page_id]]
	
	return null


func get_page_ids() -> PackedStringArray:
	var ids := PackedStringArray()
	ids.resize(_page_list.size())
	
	for i in _page_list.size():
		ids.set(i, _page_list[i].get_id())
	
	return ids


func has_node_with_id(p_node_id: String) -> bool:
	for page in _page_list:
		if page.has_node_with_id(p_node_id):
			return true
	
	return false


func get_node_by_id(p_node_id: String) -> FlowNode:
	var node: FlowNode = null
	
	for page in _page_list:
		node = page.get_node_by_id(p_node_id)
		if node != null:
			break
	
	return node


func get_all_nodes() -> Array[FlowNode]:
	var all_nodes: Array[FlowNode] = []
	
	for page in _page_list:
		all_nodes.append_array(page.get_node_list())
	
	return all_nodes


func rename_node(p_old_id: String, p_new_id: String) -> bool:
	if p_old_id != p_new_id and is_new_node_id_valid(p_new_id):
		for page: FlowPage in get_page_list():
			if page.has_node_with_id(p_old_id):
				return page.rename_node(p_old_id, p_new_id)
	
	return false


func clear_references_to_node(p_node_id: String) -> void:
	for node in get_all_nodes():
		node.on_external_node_renamed(p_node_id, "")


func clear_references_to_nodes(p_nodes_ids: PackedStringArray) -> void:
	for node in get_all_nodes():
		for id in p_nodes_ids:
			node.on_external_node_renamed(id, "")


func is_new_node_id_valid(p_new_id: String) -> bool:
	return (not p_new_id.is_empty()) and (not has_node_with_id(p_new_id)) and (FlowUtilities.is_alphanumeric_id_valid(p_new_id))


func generate_new_node_id() -> String:
	var new_id: String = FlowUtilities.generate_random_alphanumeric_id()
	
	while not is_new_node_id_valid(new_id):
		new_id = FlowUtilities.generate_random_alphanumeric_id()
	
	return new_id


func create_node(p_page_id: String, p_type_id: String, p_node_id: String) -> FlowNode:
	if is_new_node_id_valid(p_node_id):
		var page: FlowPage = get_page_by_id(p_page_id)
		
		if page != null:
			return page.create_node(p_type_id, p_node_id)
	
	return null


func delete_node(p_node_id: String) -> bool:
	if has_node_with_id(p_node_id):
		for page in get_page_list():
			if page.has_node_with_id(p_node_id):
				return page.delete_node(p_node_id)
	
	return false


func get_page_count() -> int:
	return _page_list.size()


func get_page_list() -> Array[FlowPage]:
	return _page_list


func is_new_page_id_valid(p_new_id: String) -> bool:
	return (not has_page_with_id(p_new_id)) and (FlowUtilities.is_alphanumeric_id_valid(p_new_id))


func rename_page(p_old_name: String, p_new_name: String) -> bool:
	return false


func create_page(p_new_page_id: String) -> FlowScriptPageCreationResult:
	return _create_page(p_new_page_id, true)


func delete_page(p_page_id: String) -> bool:
	if not has_page_with_id(p_page_id):
		return false
	
	deleting_page.emit(p_page_id)
	
	var page_idx: int = _page_id_map[p_page_id]
	var page: FlowPage = _page_list[page_idx]
	
	_page_id_map.erase(p_page_id)
	_page_list.remove_at(page_idx)
	
	_update_page_id_map()
	
	page.free()
	
	page_deleted.emit(p_page_id)
	
	emit_changed()
	
	return true


func _update_page_id_map() -> void:
	for page_idx: int in _page_list.size():
		_page_id_map[_page_list[page_idx].get_id()] = page_idx



func _emit_node_created(p_node_id: String) -> void:
	node_created.emit(p_node_id)

func _emit_deleting_node(p_node_id: String) -> void:
	deleting_node.emit(p_node_id)

func _emit_node_deleted(p_node_id: String) -> void:
	node_deleted.emit(p_node_id)

func _emit_nodes_deleted(p_nodes_ids: PackedStringArray) -> void:
	nodes_deleted.emit(p_nodes_ids)

func _emit_node_renamed(p_from_id: String, p_to_id: String) -> void:
	node_renamed.emit(p_from_id, p_to_id)



func _create_page(p_new_page_id: String, p_emit: bool) -> FlowScriptPageCreationResult:
	var result := FlowScriptPageCreationResult.new()
	
	if not is_new_page_id_valid(p_new_page_id):
		result.set_error(FlowScriptPageCreationResult.PageCreationErrorCode.ID_INVALID)
		return result
	
	if get_page_count() >= MAX_PAGES:
		result.set_error(FlowScriptPageCreationResult.PageCreationErrorCode.MAX_PAGES_EXCEEDED)
		return result
	
	var page := FlowPage.new()
	result.set_page(page)
	page.set_id(p_new_page_id)
	
	_page_id_map[page.get_id()] = _page_list.size()
	_page_list.append(page)
	
	
	page.node_created.connect(_emit_node_created)
	page.deleting_node.connect(_emit_deleting_node)
	page.node_deleted.connect(_emit_node_deleted)
	page.nodes_deleted.connect(_emit_nodes_deleted)
	page.node_renamed.connect(_emit_node_renamed)
	
	
	if p_emit:
		page_created.emit(page.get_id())
	
	emit_changed()
	
	return result


#func _node_predelete(p_node_id: String) -> void:
	#clear_references_to_node(p_node_id)


func _node_postdelete(p_node_id: String) -> void:
	clear_references_to_node(p_node_id)


func _nodes_postdelete(p_nodes_ids: PackedStringArray) -> void:
	clear_references_to_nodes(p_nodes_ids)


func _set_data_serializable(p_data: Dictionary) -> void:
	for key in p_data:
		var value: Variant = p_data[key]
		
		match key:
			"pages":
				for seria_page in value:
					var page: FlowPage = _deserialize_page(seria_page)


func _get_data_serializable() -> Dictionary:
	var data: Dictionary = {
		"pages": [],
	}
	
	data.pages.resize(_page_list.size())
	
	for i in _page_list.size():
		data.pages[i] = _serialize_page(_page_list[i])
	
	return data


func _serialize_page(p_page: FlowPage) -> Dictionary:
	var data: Dictionary = {}
	
	for p_info: Dictionary in p_page.get_property_list():
		if p_info.usage & PROPERTY_USAGE_SCRIPT_VARIABLE \
		and p_info.usage & PROPERTY_USAGE_STORAGE:
			
			data[p_info.name] = p_page.get(p_info.name)
	
	return data


func _deserialize_page(p_page_data: Dictionary) -> FlowPage:
	var creation_result: FlowScriptPageCreationResult = _create_page(p_page_data.id, false)
	if creation_result.get_error() != FlowScriptPageCreationResult.PageCreationErrorCode.OK:
		return null
	
	var page: FlowPage = creation_result.get_page()
	
	for key in p_page_data:
		var value = p_page_data[key]
		page.set(key, value)
	
	return page
