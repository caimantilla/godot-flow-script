
extends FlowObject


var scene: Node


func get_sprite(node_name):
	return scene.get_node(node_name) as Sprite2D


func move_sprite_to_point(node_name, destination, speed):
	var sprite = get_sprite(node_name)
	var current_position = sprite.position
	
	if current_position.is_equal_approx(destination):
		return null
	
	var distance = current_position.distance_to(destination)
	var movement_duration = distance / speed
	
	var tween = scene.create_tween()
	tween.tween_property(sprite, "position", destination, movement_duration)
	
	return tween
