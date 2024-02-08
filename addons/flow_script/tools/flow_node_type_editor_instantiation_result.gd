@tool
class_name FlowNodeTypeEditorInstantiationResult
extends RefCounted


var success: bool = false: set = set_successful, get = is_successful
var object: FlowNode = null: set = set_object, get = get_object
var editor: FlowNodeEditor = null: set = set_editor, get = get_editor


func set_successful(p_yes: bool) -> void:
	success = p_yes

func is_successful() -> bool:
	return success


func set_object(p_obj: FlowNode) -> void:
	object = p_obj

func get_object() -> FlowNode:
	return object


func set_editor(p_editor: FlowNodeEditor) -> void:
	editor = p_editor

func get_editor() -> FlowNodeEditor:
	return editor
