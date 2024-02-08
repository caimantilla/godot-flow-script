@tool
class_name FlowNodeInspectorPlugin
extends EditorInspectorPlugin




func _can_handle(p_obj: Object) -> bool:
	return (p_obj is FlowNode)


func _parse_property(object: Object, type: Variant.Type, name: String, hint_type: PropertyHint, hint_string: String, usage_flags: int, wide: bool) -> bool:
	if object is FlowNode:
		# Hide the "script" property of the FlowNode.
		if hint_type == PropertyHint.PROPERTY_HINT_RESOURCE_TYPE \
		and hint_string == "Script" \
		and name == "script":
			return true
		
		# For now, don't show connections.
		# Add a button to set them from the inspector rather than by dragging later...
		elif object.is_property_flow_node_reference(name):
			return true
	
	return false
