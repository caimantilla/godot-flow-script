@tool
class_name FlowNodeTranslationEntry
extends RefCounted
## Returned by a FlowNode for every translatable item.


enum Type {
	STRING = 1,
	RESOURCE = 2,
}


var type: set = set_type, get = get_type
var id: String: set = set_id, get = get_id
var original_string: String: set = set_original_string, get = get_original_string
var original_resource: Resource: set = set_original_resource, get = get_original_resource


var _type := Type.STRING
var _id: String = ""
var _original_string := ""
var _original_resource: Resource = null


func set_type(p_type: Type) -> void:
	_type = p_type

func get_type() -> Type:
	return _type


func set_id(p_id: String) -> void:
	_id = p_id

func get_id() -> String:
	return _id


func set_original_string(p_og_string: String) -> void:
	_original_string = p_og_string

func get_original_string() -> String:
	return _original_string


func set_original_resource(p_og_res: Resource) -> void:
	_original_resource = p_og_res

func get_original_resource() -> Resource:
	return _original_resource
