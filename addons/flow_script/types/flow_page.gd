@tool
class_name FlowPage
extends Object
## A script page which contains nodes.
## Note that node names must be unique within the whole of a script, not just a page.
## Pages are purely for editor performance and organization concerns.



## Emitted when some property of the page changes.
signal changed()

signal node_created(node_id: String)
signal deleting_node(node_id: String)
signal node_deleted(node_id: String)
signal nodes_deleted(node_ids: PackedStringArray)
signal node_renamed(from_id: String, to_id: String)



## The maximum number of nodes that a page can hold.
const MAX_NODES: int = 999



## The page's ID.
var id: String: set = set_id, get = get_id



var _id: String = ""


# Contains each node in the page.
var _node_list: Array[FlowNode] = []

# Maps node IDs to list indices.
# Whenever the node list is updated, the ID map must be updated as well.
var _node_id_map: Dictionary = {}



func _notification(what: int) -> void:
	if what == NOTIFICATION_PREDELETE:
		for node in _node_list:
			node.free()
		_node_list.clear()
		_node_id_map.clear()



func _get_property_list() -> Array[Dictionary]:
	var properties: Array[Dictionary] = []
	
	properties.append({
		"name": "id",
		"type": TYPE_STRING,
		"usage": PROPERTY_USAGE_SCRIPT_VARIABLE | PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY,
	})
	
	
	for node in _node_list:
		var node_id: String = node.get_id()
		for node_p_info: Dictionary in node.get_property_list():
			if node_p_info.usage & PROPERTY_USAGE_SCRIPT_VARIABLE \
			and node_p_info.usage & PROPERTY_USAGE_EDITOR:
				
				node_p_info.name = ("nodes/%s/%s" % [ node_id, node_p_info.name ])
				node_p_info.usage = node_p_info.usage &~ PROPERTY_USAGE_STORAGE
				node_p_info.usage = node_p_info.usage &~ PROPERTY_USAGE_STORE_IF_NULL
				
				properties.append(node_p_info)
	
	
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




func set_id(p_id: String) -> void:
	if p_id != _id:
		_id = p_id
		emit_changed()

func get_id() -> String:
	return _id





func emit_changed() -> void:
	changed.emit()




func get_node_by_id(p_node_id: String) -> FlowNode:
	if _node_id_map.has(p_node_id):
		return _node_list[_node_id_map[p_node_id]]
	
	return null


func get_node_by_index(p_node_idx: int) -> FlowNode:
	if p_node_idx > -1 and p_node_idx < _node_list.size():
		return _node_list[p_node_idx]
	
	return null


func get_node_ids() -> PackedStringArray:
	var ids := PackedStringArray()
	ids.resize(_node_list.size())
	
	for i in _node_list.size():
		ids.set(i, _node_list[i].get_id())
	
	return ids


func get_node_list() -> Array[FlowNode]:
	return _node_list


func get_node_count() -> int:
	return _node_list.size()


func has_node_with_id(p_node_id: String) -> bool:
	return _node_id_map.has(p_node_id)


func is_new_node_id_valid(p_new_node_id: String) -> bool:
	return (not has_node_with_id(p_new_node_id)) and (FlowUtilities.is_alphanumeric_id_valid(p_new_node_id))


func create_node(p_type: String, p_id: String) -> FlowNode:
	return _create_node(p_type, p_id, true)


func delete_node(p_node_id: String) -> bool:
	return _delete_node(p_node_id, true)


func delete_nodes(p_node_ids: PackedStringArray) -> void:
	var successful_deletions := PackedStringArray()
	
	for id in p_node_ids:
		if _delete_node(id, false):
			successful_deletions.push_back(id)
	
	if not successful_deletions.is_empty():
		nodes_deleted.emit(successful_deletions)


## Renames a node.
## WARNING: This should only be called from the script which owns this page. This is because no safety checks are performed, as they require the other pages as context since node names must be unique to the script, not just to the page.
func rename_node(p_from_id: String, p_to_id: String) -> bool:
	var target_node: FlowNode = get_node_by_id(p_from_id)
	
	if target_node == null:
		delete_node(p_from_id)
		return false
	
	else:
		target_node.set_id(p_to_id)
		
		for other_node: FlowNode in get_node_list():
			other_node.on_external_node_renamed(p_from_id, p_to_id)
		
		_node_id_map[p_to_id] = _node_id_map[p_from_id]
		_node_id_map.erase(p_from_id)
		
		node_renamed.emit(p_from_id, p_to_id)
		
		return true





func _rebuild_node_id_map() -> void:
	for node_idx: int in _node_list.size():
		var node: FlowNode = _node_list[node_idx]
		var node_id: String = node.get_id()
		
		_node_id_map[node_id] = node_idx




func _create_node(p_type: String, p_id: String, p_emit: bool) -> FlowNode:
	if not is_new_node_id_valid(p_id):
		return null
	
	var node: FlowNode = FlowFactory.instantiate_node_from_type(p_type)
	if node == null:
		return null
	
	node.set_id(p_id)
	
	_node_id_map[p_id] = _node_list.size()
	_node_list.append(node)
	
	if p_emit:
		node_created.emit(node.get_id())
	
	return node



func _delete_node(p_node_id: String, p_emit: bool) -> bool:
	if not _node_id_map.has(p_node_id):
		return false
	
	var node_idx: int = _node_id_map[p_node_id]
	var node: FlowNode = _node_list[node_idx]
	
	# Do anything important before deletion!!
	#if p_emit:
	deleting_node.emit(p_node_id)
	
	node.free()
	_node_list.remove_at(node_idx)
	_node_id_map.erase(p_node_id)
	
	_rebuild_node_id_map()
	
	if p_emit:
		node_deleted.emit(p_node_id)
	
	return true




func _set_data_serializable(p_data: Dictionary) -> void:
	for key in p_data:
		var value: Variant = p_data[key]
		
		match key:
			"nodes":
				for node_data in value:
					if typeof(node_data) == TYPE_DICTIONARY:
						
						if node_data.has("type") and typeof(node_data["type"]) == TYPE_STRING \
						and node_data.has("id") and typeof(node_data["id"]) == TYPE_STRING:
							
							var node_type: String = node_data.type
							var node_id: String = node_data.id
							
							# The node has no need to deserialize these properties, and attempting to override them could cause problems.
							node_data.erase("type")
							node_data.erase("id")
							
							var new_node: FlowNode = _create_node(node_type, node_id, false)
							
							if new_node != null:
								new_node.set_data(node_data)


func _get_data_serializable() -> Dictionary:
	var data: Dictionary = {
		"nodes": [],
	}
	
	data.nodes.resize(get_node_count())
	
	for node_idx: int in _node_list.size():
		var node: FlowNode = _node_list[node_idx]
		var node_data: Dictionary = node.get_data()
		
		data.nodes[node_idx] = node_data
	
	return data
