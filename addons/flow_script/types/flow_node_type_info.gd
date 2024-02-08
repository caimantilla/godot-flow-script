@tool
class_name FlowNodeTypeInfo
extends RefCounted
## Contains info about a type of FlowNode.





## The ID of the node type.
var id: String: set = set_id, get = get_id

## The name of the node type.
var name: String: set = set_name, get = get_name

## The node type's category.
## Sub-categories are split using a forward slash ("/").
var category: String: set = set_category, get = get_category

## The description of the type shown in the node creation dialog.
var description: String: set = set_description, get = get_description

## The type's icon.
## If it is a path to a texture, that texture will be used.
## Otherwise, the texture will be sourced from EditorIcons if found.
var icon: String: set = set_icon, get = get_icon

## The path to the type's object script.
var object_path: String: set = set_object_path, get = get_object_path

## The path to the type's editor script or scene.
var editor_path: String: set = set_editor_path, get = get_editor_path

## Whether or not the type is built-in.
## Built-in types are not saved to the type configuration.
var built_in: bool: set = set_as_built_in, get = is_built_in

## If disabled, the user will be prompted to enter the node's ID when creating the node.
var automatically_assign_id: bool: set = set_id_as_automatically_assigned, get = is_id_automatically_assigned





var _id: String = ""
var _name: String = ""
var _category: String = ""
var _description: String = ""
var _icon: String = ""
var _object_path: String = ""
var _editor_path: String = ""
var _built_in: bool = false
var _automatically_assign_id: bool = true







func set_id(p_id: String) -> void:
	_id = p_id

func get_id() -> String:
	return _id


func set_name(p_type_name: String) -> void:
	_name = p_type_name

func get_name() -> String:
	return _name


func set_category(p_category: String) -> void:
	_category = p_category

func get_category() -> String:
	return _category


func set_description(p_desc: String) -> void:
	_description = p_desc

func get_description() -> String:
	return _description


func set_icon(p_icon_name: String) -> void:
	_icon = p_icon_name

func get_icon() -> String:
	return _icon


func set_object_path(p_path: String) -> void:
	_object_path = p_path

func get_object_path() -> String:
	return _object_path


func set_editor_path(p_path: String) -> void:
	_editor_path = p_path

func get_editor_path() -> String:
	return _editor_path


func set_as_built_in(p_enabled: bool) -> void:
	_built_in = p_enabled

func is_built_in() -> bool:
	return _built_in


func set_id_as_automatically_assigned(p_enabled: bool) -> void:
	_automatically_assign_id = p_enabled

func is_id_automatically_assigned() -> bool:
	return _automatically_assign_id







func get_icon_texture() -> Texture2D:
	var tex: Texture2D = null
	
	if ResourceLoader.exists(_icon, "Texture2D"):
		tex = ResourceLoader.load(_icon, "Texture2D") as Texture2D
	
	if tex == null:
		var base_control: Control = EditorInterface.get_base_control()
		if base_control.has_theme_icon(_icon, &"EditorIcons"):
			tex = base_control.get_theme_icon(_icon, &"EditorIcons")
	
	return tex
