@tool
extends FlowNodeEditor



const SetVariableNode := preload("../objects/_base_set_variable.gd")



@onready var variable_name_edit := $HBox/Variable as LineEdit
@onready var expression_edit := $HBox/Expression as LineEdit




func _update_style() -> void:
	if Engine.is_editor_hint():
		
		var code_font: Font = get_theme_font(&"source", &"EditorFonts")
		
		for line_edit: LineEdit in [ variable_name_edit, expression_edit ]:
			line_edit.add_theme_font_override(&"font", code_font)


func _get_input_slot() -> int:
	return 0


func _set_outgoing_connection(p_outgoing_connection: FlowNodeEditorOutgoingConnectionParameters) -> void:
	var node = get_variable_flow_node()
	if node != null:
		
		if p_outgoing_connection.get_slot() == 0:
			node.set_next_node_id(p_outgoing_connection.get_node_id())


func _on_node_updated() -> void:
	var node = get_variable_flow_node()
	if node != null:
		
		if node.get_variable_name() != variable_name_edit.get_text():
			variable_name_edit.set_text(node.get_variable_name())
		
		if node.get_value_expression() != expression_edit.get_text():
			expression_edit.set_text(node.get_value_expression())


func _get_outgoing_connections() -> Array[FlowNodeEditorOutgoingConnectionParameters]:
	var connections: Array[FlowNodeEditorOutgoingConnectionParameters] = []
	
	var node = get_variable_flow_node()
	
	if (node != null) and (not node.get_next_node_id().is_empty()):
		var conn := FlowNodeEditorOutgoingConnectionParameters.new()
		conn.set_node_id(node.get_next_node_id())
		conn.set_slot(0)
		
		connections.append(conn)
	
	return connections






func get_variable_flow_node() -> SetVariableNode:
	return get_flow_node() as SetVariableNode



func _on_variable_text_changed(p_text: String) -> void:
	var node = get_variable_flow_node()
	if node != null:
		node.set_variable_name(p_text)


func _on_expression_text_changed(p_text: String) -> void:
	var node = get_variable_flow_node()
	if node != null:
		node.set_value_expression(p_text)
