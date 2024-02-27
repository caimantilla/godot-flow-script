@tool
extends RefCounted


const INTERFACE_LOCATION_SETTING: String = "flow_script/config/interface_location"
const INTERFACE_LOCATION_DEFAULT := FlowUtilities.InterfaceLocation.MAIN_SCREEN


const SETTINGS_LIST: Array[Dictionary] = [
	{
		name = INTERFACE_LOCATION_SETTING,
		type = TYPE_INT,
		hint = FlowUtilities.InterfaceLocation_HINT,
		hint_string = FlowUtilities.InterfaceLocation_HINT_STRING,
		usage = PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_RESTART_IF_CHANGED,
		default_value = INTERFACE_LOCATION_DEFAULT,
	},
]


static func get_editor_settings() -> EditorSettings:
	return EditorInterface.get_editor_settings()


static func create_settings() -> void:
	var editor_settings: EditorSettings = get_editor_settings()
	
	for setting: Dictionary in SETTINGS_LIST:
		if not editor_settings.has_setting(setting.name):
			editor_settings.set_setting(setting.name, setting.default_value)
		editor_settings.set_initial_value(setting.name, setting.default_value, false)
		editor_settings.add_property_info(setting)


static func get_interface_location() -> FlowUtilities.InterfaceLocation:
	return get_editor_settings().get_setting(INTERFACE_LOCATION_SETTING)
