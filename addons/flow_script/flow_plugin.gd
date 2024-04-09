@tool
class_name FlowPlugin
extends EditorPlugin




const ProjectSettingsInterface := preload("project_settings_interface.gd")
const EditorSettingsInterface := preload("editor_settings_interface.gd")


var _plugin_interface_location := FlowUtilities.InterfaceLocation.BOTTOM_PANEL
var _flow_node_inspector_plugin: FlowNodeInspectorPlugin
var _flow_script_bottom_panel_button: Button
var _flow_script_editor_panel: FlowScriptEditorPanel




func _notification(what: int) -> void:
	if what == NOTIFICATION_APPLICATION_FOCUS_IN:
		reload_type_db()


func _init() -> void:
	if Engine.is_editor_hint():
		Engine.set_meta(&"FlowPlugin", self)
	
	ProjectSettingsInterface.create_settings()
	ProjectSettings.settings_changed.connect(reload_type_db, CONNECT_DEFERRED)
	
	EditorSettingsInterface.create_settings()
	
	_plugin_interface_location = EditorSettingsInterface.get_interface_location()


func _enter_tree() -> void:
	_flow_node_inspector_plugin = FlowNodeInspectorPlugin.new()
	add_inspector_plugin(_flow_node_inspector_plugin)
	
	_flow_script_editor_panel = FlowScriptEditorPanel.new()
	_flow_script_editor_panel.set_name("FlowScript Editor")
	_flow_script_editor_panel.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	_flow_script_editor_panel.set_h_size_flags(Control.SIZE_EXPAND_FILL)
	_flow_script_editor_panel.set_v_size_flags(Control.SIZE_EXPAND_FILL)
	_flow_script_editor_panel.flow_node_selected.connect(_on_editor_flow_node_selected)
	_flow_script_editor_panel.save_requested.connect(save_current_edited_flow_script)
	
	match _plugin_interface_location:
		FlowUtilities.InterfaceLocation.BOTTOM_PANEL:
			_flow_script_bottom_panel_button = add_control_to_bottom_panel(_flow_script_editor_panel, "FlowScript")
		FlowUtilities.InterfaceLocation.MAIN_SCREEN, _:
			EditorInterface.get_editor_main_screen().add_child(_flow_script_editor_panel, true)
			_make_visible(false)
	
	EditorInterface.get_inspector().resource_selected.connect(_on_inspector_resource_selected)


func _exit_tree() -> void:
	ProjectSettings.settings_changed.disconnect(reload_type_db)
	EditorInterface.get_inspector().resource_selected.disconnect(_on_inspector_resource_selected)
	
	if _plugin_interface_location == FlowUtilities.InterfaceLocation.BOTTOM_PANEL:
		remove_control_from_bottom_panel(_flow_script_editor_panel)
	_flow_script_editor_panel.queue_free()
	
	remove_inspector_plugin(_flow_node_inspector_plugin)
	
	if Engine.is_editor_hint():
		Engine.remove_meta(&"FlowPlugin")


func _get_plugin_icon() -> Texture2D:
	var base: Texture2D = EditorInterface.get_base_control().get_theme_icon(&"GraphEdit", &"EditorIcons")
	if base != null:
		var img: Image = base.get_image()
		if img != null:
			img.adjust_bcs(1, 1, 0) # Desaturate the icon
			return ImageTexture.create_from_image(img)
	
	return null


func _has_main_screen() -> bool:
	return _plugin_interface_location == FlowUtilities.InterfaceLocation.MAIN_SCREEN


func _get_plugin_name() -> String:
	return "FlowScript"


func _handles(p_obj: Object) -> bool:
	return (p_obj is FlowScript)


func _edit(p_obj: Object) -> void:
	if p_obj is FlowScript:
		if p_obj != get_current_edited_flow_script():
			if get_current_edited_flow_script() != null:
				get_current_edited_flow_script().deleting_node.disconnect(_clear_inspector)
			
			_flow_script_editor_panel.set_flow_script(p_obj as FlowScript)
			
			# THIS NEEDS TO BE CONNECTED OR THE EDITOR WILL FUCKING CRASH WHEN AN INSPECTED NODE IS DELETED
			# I THINK IT'S TRYING TO ACCESS A FREED OBJECT!! NO is_instance_valid CHECKS HERE
			p_obj.deleting_node.connect(_clear_inspector.unbind(1))
		
		_make_visible(true)


func _make_visible(p_visible: bool) -> void:
	match _plugin_interface_location:
		FlowUtilities.InterfaceLocation.BOTTOM_PANEL:
			if p_visible:
				make_bottom_panel_item_visible(_flow_script_editor_panel)
		FlowUtilities.InterfaceLocation.MAIN_SCREEN, _:
			_flow_script_editor_panel.visible = p_visible


func reload_type_db() -> void:
	FlowNodeTypeDB.reload_types()



func get_current_edited_flow_script() -> FlowScript:
	if _flow_script_editor_panel != null:
		return _flow_script_editor_panel.get_flow_script()
	
	return null


func get_current_edited_flow_page() -> FlowPage:
	if _flow_script_editor_panel != null:
		return _flow_script_editor_panel.get_current_page()
	
	return null


func save_current_edited_flow_script() -> void:
	var flow_script: FlowScript = get_current_edited_flow_script()
	if flow_script != null:
		var path: String = flow_script.get_path()
		if ResourceLoader.exists(path):
			ResourceSaver.save(flow_script, path)


func _on_editor_flow_node_selected(p_node_id: String) -> void:
	var node: FlowNode = null
	
	var edited_script: FlowScript = get_current_edited_flow_script()
	if edited_script != null:
		node = edited_script.get_node_by_id(p_node_id)
	
	EditorInterface.inspect_object(node, "", false)


func _clear_inspector() -> void:
	EditorInterface.inspect_object(null, "", false)


func _on_inspector_resource_selected(resource: Resource, _path: String) -> void:
	print("Resource selected: ", resource, _path)
	
	#if _handles(resource):
		#_edit(resource)
