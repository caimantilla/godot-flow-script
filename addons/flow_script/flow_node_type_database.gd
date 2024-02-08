@tool
class_name FlowNodeTypeDB
extends RefCounted





const ProjectSettingsInterface := preload("project_settings_interface.gd")
const BuiltInTypes := preload("built_in_types.gd")
const BASE_DIR: String = "res://addons/flow_script/"





static var _type_map: Dictionary = {}
static var _type_list: Array[FlowNodeTypeInfo] = []




static func _static_init() -> void:
	reload_types()



static func has_type(p_type_id: String) -> bool:
	return _type_map.has(p_type_id)


static func get_type(p_type_id: String) -> FlowNodeTypeInfo:
	if _type_map.has(p_type_id):
		return _type_list[_type_map[p_type_id]]
	
	return null


static func get_type_list() -> Array[FlowNodeTypeInfo]:
	return _type_list


static func clear_types() -> void:
	_type_map.clear()
	_type_list.clear()


static func reload_types() -> void:
	clear_types()
	
	var nodes_config_path: String = ProjectSettingsInterface.get_flow_nodes_config_path()
	
	for bi_type_info: Dictionary in BuiltInTypes.data:
		var type := FlowNodeTypeInfo.new()
		
		type.set_as_built_in(true)
		
		type.set_id(bi_type_info.id)
		type.set_name(bi_type_info.name)
		type.set_category(bi_type_info.category)
		type.set_description(bi_type_info.description)
		type.set_id_as_automatically_assigned(bi_type_info.automatically_assign_id)
		type.set_object_path( BASE_DIR.path_join(bi_type_info.object_path) )
		type.set_editor_path( BASE_DIR.path_join(bi_type_info.editor_path) )
		
		_add_type(type)
	
	if FileAccess.file_exists(nodes_config_path):
		var cfg := ConfigFile.new()
		if cfg.load(nodes_config_path) == OK:
			
			for id: String in cfg.get_sections():
				var type := FlowNodeTypeInfo.new()
				type.set_id(id)
				
				for property_name: String in cfg.get_section_keys(id):
					if property_name in type:
						var value: Variant = cfg.get_value(id, property_name, null)
						if typeof(value) == typeof(type.get(property_name)):
							type.set(property_name, value)
				
				_add_type(type)


static func _add_type(p_type: FlowNodeTypeInfo) -> void:
	var type_id: String = p_type.get_id()
	
	if type_id.is_empty():
		printerr("Cannot add FlowNode type with no ID.")
	elif _type_map.has(type_id):
		printerr('FlowNode type with ID "%s" already exists.' % type_id)
	else:
		_type_map[p_type.get_id()] = _type_list.size()
		_type_list.append(p_type)
