@tool
extends FlowNodeEditor


const Procedure := preload("../objects/procedure.gd")


func _set_outgoing_connection(p_connection: FlowNodeEditorOutgoingConnectionParameters) -> void:
	if p_connection.get_slot() == 0:
		var node = get_proc_flow_node()
		if node != null:
			node.set_next_node_id(p_connection.get_node_id())


func _get_outgoing_connections() -> Array[FlowNodeEditorOutgoingConnectionParameters]:
	var connections: Array[FlowNodeEditorOutgoingConnectionParameters] = []
	
	var node = get_proc_flow_node()
	if node != null:
		if not node.get_next_node_id().is_empty():
			var connection := FlowNodeEditorOutgoingConnectionParameters.new()
			connection.set_slot(0)
			connection.set_node_id(node.get_next_node_id())
			
			connections.append(connection)
	
	return connections


func get_proc_flow_node() -> Procedure:
	return get_flow_node() as Procedure
