@tool
class_name FlowNodeEditorOutgoingConnectionParameters
extends RefCounted


var node_id: String: set = set_node_id, get = get_node_id
var slot: int: set = set_slot, get = get_slot
var port: int: set = set_port, get = get_port



var _node_id: String = ""
var _slot: int = -1
var _port: int = -1




func set_node_id(p_id: String) -> void:
	_node_id = p_id

func get_node_id() -> String:
	return _node_id


func set_slot(p_slot_idx: int)  -> void:
	_slot = p_slot_idx

func get_slot() -> int:
	return _slot


func set_port(p_port_idx: int) -> void:
	_port = p_port_idx

func get_port() -> int:
	return _port
