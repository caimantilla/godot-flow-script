@tool
extends RefCounted




const FLOW_NODES_CONFIG_PATH_SETTING: String = "flow_script/config/flow_nodes_config_path"
const FLOW_NODES_CONFIG_PATH_DEFAULT: String = "res://flow_nodes.cfg"



const SETTINGS_LIST: Array[Dictionary] = [
	{
		name = FLOW_NODES_CONFIG_PATH_SETTING,
		type = TYPE_STRING,
		hint = PROPERTY_HINT_FILE,
		hint_string = "*.cfg",
		default_value = FLOW_NODES_CONFIG_PATH_DEFAULT,
	},
]






static func create_settings() -> void:
	for setting in SETTINGS_LIST:
		if not ProjectSettings.has_setting(setting.name):
			ProjectSettings.set_setting(setting.name, setting.default_value)
		ProjectSettings.set_initial_value(setting.name, setting.default_value)
		ProjectSettings.add_property_info(setting)
	
	_initialize_files()




static func get_flow_nodes_config_path() -> String:
	return ProjectSettings.get_setting(FLOW_NODES_CONFIG_PATH_SETTING, FLOW_NODES_CONFIG_PATH_DEFAULT) as String




static func _initialize_files() -> void:
	var path: String = get_flow_nodes_config_path()
	if not FileAccess.file_exists(path):
		var cfg := ConfigFile.new()
		cfg.save(path)
