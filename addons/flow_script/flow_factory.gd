@tool
class_name FlowFactory
extends RefCounted


#static func instantiate_flow_node_type(p_type_id: String) -> FlowNodeTypeEditorInstantiationResult:
	#var result := FlowNodeTypeEditorInstantiationResult.new()
	#
	#if not FlowNodeTypeDB.has_type(p_type_id):
		#return result
	#
	#var object: FlowNode = null
	#var editor: FlowNodeEditor = null
	#
	#var type_info: FlowNodeTypeInfo = FlowNodeTypeDB.get_type(p_type_id)
	#if ResourceLoader.exists(type_info.get_object_path()) and ResourceLoader.exists(type_info.get_editor_path()):
		#var res_object: Resource = ResourceLoader.load(type_info.get_object_path())
		#var res_editor: Resource = ResourceLoader.load(type_info.get_editor_path())
		#
		#if res_object is GDScript:
			#var obj_instance: Object = res_object.new()
			#if obj_instance is FlowNode:
				#
				#object = obj_instance
				#
				#if res_editor is PackedScene:
					#if res_editor.can_instantiate():
						#var edit_instance: Node = res_editor.instantiate(PackedScene.GEN_EDIT_STATE_DISABLED)
						#
						#if edit_instance is FlowNodeEditor:
							#editor = edit_instance
						#elif edit_instance != null:
							#edit_instance.queue_free()
				#
				#elif res_editor is GDScript:
					#var edit_instance: Object = res_editor.new()
					#
					#if edit_instance is FlowNodeEditor:
						#editor = edit_instance
					#elif (edit_instance != null) and (not edit_instance is RefCounted):
						#edit_instance.free()
			#
			#elif (obj_instance != null) and (not obj_instance is RefCounted):
				#obj_instance.free()
	#
	#
	#if object != null and editor != null:
		#result.set_object(object)
		#result.set_editor(editor)
		#result.set_successful(true)
		#
		#editor.set_flow_node(object)
	#
	#return result




static func instantiate_node_from_type(p_type_id: String) -> FlowNode:
	if not FlowNodeTypeDB.has_type(p_type_id):
		return null
	
	var type_info: FlowNodeTypeInfo = FlowNodeTypeDB.get_type(p_type_id)
	if ResourceLoader.exists(type_info.get_object_path()):
		var node_script := ResourceLoader.load(type_info.get_object_path(), "GDScript") as GDScript
		if node_script != null and node_script.is_tool():
			
			# Ensure that the script is FlowNode.
			var base_script: GDScript = node_script
			while base_script != null:
				if base_script == FlowNode:
					break
				
				base_script = base_script.get_base_script()
			
			if base_script == null:
				return null
			
			var instance: Object = node_script.new(type_info.get_id(), type_info.get_name(), not type_info.is_id_automatically_assigned())
			
			if instance is FlowNode:
				return instance as FlowNode
			
			elif (instance != null) and (not instance is RefCounted):
				instance.free()
	
	return null


static func instantiate_editor_from_node(p_node: FlowNode) -> FlowNodeEditor:
	if p_node == null:
		return null
	
	var type_id: String = p_node.get_type()
	
	var instance: FlowNodeEditor = instantiate_editor_from_type(type_id)
	instance.set_flow_node(p_node)
	
	return instance


static func instantiate_editor_from_type(p_type_id: String) -> FlowNodeEditor:
	if not FlowNodeTypeDB.has_type(p_type_id):
		return null
	
	var type_info: FlowNodeTypeInfo = FlowNodeTypeDB.get_type(p_type_id)
	
	if ResourceLoader.exists(type_info.get_editor_path()):
		var res_node_editor: Object = ResourceLoader.load(type_info.get_editor_path())
		
		if res_node_editor is PackedScene:
			if res_node_editor.can_instantiate():
				
				var editor_instance: Node = res_node_editor.instantiate(PackedScene.GEN_EDIT_STATE_DISABLED)
				
				if editor_instance is FlowNodeEditor:
					return editor_instance
				
				elif editor_instance != null:
					editor_instance.queue_free()
		
		elif res_node_editor is GDScript:
			
			var editor_instance: Object = res_node_editor.new()
			
			if editor_instance is FlowNodeEditor:
				return editor_instance
			
			elif (editor_instance != null) and (not editor_instance is RefCounted):
				editor_instance.free()
	
	return null
