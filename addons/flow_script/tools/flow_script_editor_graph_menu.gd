@tool
class_name FlowScriptEditorGraphMenu
extends MarginContainer




signal node_creation_dialog_requested()



const INNER_CONTAINER_MARGIN: int = 4
const MARGIN_CONSTANT_NAMES: Array[StringName] = [ &"margin_left", &"margin_top", &"margin_right", &"margin_bottom" ]



var _node_creation_button: Button



func _init() -> void:
	set_mouse_filter(MOUSE_FILTER_IGNORE)
	set_focus_mode(FOCUS_NONE)
	
	
	var inner_container_margin: int = floori(EditorInterface.get_editor_scale() * float(INNER_CONTAINER_MARGIN)) if Engine.is_editor_hint() else INNER_CONTAINER_MARGIN
	
	var inner_container := MarginContainer.new()
	inner_container.set_name("Inner Container")
	inner_container.set_mouse_filter(MOUSE_FILTER_IGNORE)
	inner_container.set_focus_mode(FOCUS_NONE)
	inner_container.set_h_size_flags(SIZE_EXPAND_FILL)
	inner_container.set_v_size_flags(SIZE_EXPAND_FILL)
	
	for margin_name in MARGIN_CONSTANT_NAMES:
		inner_container.add_theme_constant_override(margin_name, inner_container_margin)
	
	add_child(inner_container, true)
	
	
	var bottom_spread := HBoxContainer.new()
	bottom_spread.set_name("Bottom Spread")
	bottom_spread.set_mouse_filter(MOUSE_FILTER_IGNORE)
	bottom_spread.set_focus_mode(FOCUS_NONE)
	bottom_spread.set_h_size_flags(SIZE_EXPAND_FILL)
	bottom_spread.set_v_size_flags(SIZE_SHRINK_END)
	inner_container.add_child(bottom_spread, true)
	
	_node_creation_button = Button.new()
	_node_creation_button.set_name("Create Node")
	_node_creation_button.set_text("Create Node")
	_node_creation_button.pressed.connect(request_node_creation_dialog)
	_node_creation_button.hide() # Shouldn't actually be needed for now.
	bottom_spread.add_child(_node_creation_button, true)


func _notification(what: int) -> void:
	if what == NOTIFICATION_THEME_CHANGED:
		_update_style()


func _ready() -> void:
	_update_style()



func _update_style() -> void:
	if Engine.is_editor_hint() and is_node_ready():
		_node_creation_button.set_button_icon(get_theme_icon(&"Add", &"EditorIcons"))



func request_node_creation_dialog() -> void:
	node_creation_dialog_requested.emit()
