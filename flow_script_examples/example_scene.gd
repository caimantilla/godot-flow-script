
extends Node


@export var flow_controller: FlowController
@export var test_procedure_id = ""


func _ready():
	flow_controller.flow_object.scene = self
	
	flow_controller.execute(test_procedure_id)
