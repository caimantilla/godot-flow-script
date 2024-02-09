@tool
extends FlowNode


@export_category("Move Sprite")


@export var sprite_name = "":
	set(value):
		if value != sprite_name:
			sprite_name = value
			emit_changed()

@export var destination = Vector2(0, 0):
	set(value):
		if not value.is_equal_approx(destination):
			destination = value
			emit_changed()

@export_range(0.1, 1000) var speed = 100.0:
	set(value):
		if not is_equal_approx(value, speed):
			speed = value
			emit_changed()

@export var next_node_id = "":
	set(value):
		if value != next_node_id:
			next_node_id = value
			emit_changed()


func _is_property_flow_node_reference(property):
	return (property == "next_node_id")


func _on_external_node_renamed(from, to):
	if from == next_node_id:
		next_node_id = to


func _execute(state):
	var tween = state.flow_object.move_sprite_to_point(sprite_name, destination, speed)
	
	print_debug(tween)
	
	if tween == null:
		_on_movement_finished(state)
	else:
		tween.finished.connect(_on_movement_finished.bind(state), CONNECT_ONE_SHOT)


func _on_movement_finished(state):
	state.finish(next_node_id)
