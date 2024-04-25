#include "flow_config_manager.hpp"
#include "core/config/project_settings.h"
#include "editor/editor_settings.h"
#include "core/io/dir_access.h"


static const String ES_GUI_LOCATION = "flow_script/config/editor_gui_location";
static const String ES_FLOW_NODE_CREATION_DIALOG_APPEAR_POSITION = "flow_script/config/flow_node_creation_dialog_appear_position";

static const String PS_FLOW_TYPE_DIRECTORIES = "flow_script/config/flow_type_directories";
static const String PS_EDITING_FLOW_SCRIPT_INSTANCES_ALLOWED = "flow_script/config/flow_script_instance_editing_allowed";
static const String PS_WAIT_SECONDS_TIMER_PROCESS_STEP = "flow_script/flow_nodes/wait_seconds/timer_process_step";


FlowConfigManager *FlowConfigManager::singleton = nullptr;


FlowConfigManager *FlowConfigManager::get_singleton()
{
	return singleton;
}


Variant FlowConfigManager::get_project_setting(const String &p_name) const
{
	return GLOBAL_GET(p_name);
}


Variant FlowConfigManager::get_editor_setting(const String &p_name) const
{
	return EDITOR_GET(p_name);
}


void FlowConfigManager::initialize_project_settings()
{
	GLOBAL_DEF(PS_EDITING_FLOW_SCRIPT_INSTANCES_ALLOWED, false);

	GLOBAL_DEF(PS_FLOW_TYPE_DIRECTORIES, PackedStringArray());
	ProjectSettings::get_singleton()->set_custom_property_info(PropertyInfo(Variant::PACKED_STRING_ARRAY, PS_FLOW_TYPE_DIRECTORIES, PROPERTY_HINT_DIR));

	GLOBAL_DEF(PS_WAIT_SECONDS_TIMER_PROCESS_STEP, "idle");
	ProjectSettings::get_singleton()->set_custom_property_info(PropertyInfo(Variant::STRING, PS_WAIT_SECONDS_TIMER_PROCESS_STEP, PROPERTY_HINT_ENUM, "idle,physics", PROPERTY_USAGE_DEFAULT));
}


void FlowConfigManager::initialize_editor_settings()
{
	EDITOR_DEF_RST(ES_GUI_LOCATION, "main_screen");
	EditorSettings::get_singleton()->add_property_hint(PropertyInfo(Variant::STRING, ES_GUI_LOCATION, PROPERTY_HINT_ENUM, "main_screen,bottom_panel", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_RESTART_IF_CHANGED));

	EDITOR_DEF(ES_FLOW_NODE_CREATION_DIALOG_APPEAR_POSITION, "under_mouse");
	EditorSettings::get_singleton()->add_property_hint(PropertyInfo(Variant::STRING, ES_FLOW_NODE_CREATION_DIALOG_APPEAR_POSITION, PROPERTY_HINT_ENUM, "screen_center,under_mouse", PROPERTY_USAGE_DEFAULT));
}


FlowEditorConstants::FlowGUIEditorLocation FlowConfigManager::get_flow_editor_gui_location() const
{
	FlowEditorConstants::FlowGUIEditorLocation location = FlowEditorConstants::FLOW_EDITOR_GUI_LOCATION_MAIN_SCREEN;
	String location_str = get_editor_setting(ES_GUI_LOCATION);

	if (location_str == "main_screen")
	{
		location = FlowEditorConstants::FLOW_EDITOR_GUI_LOCATION_MAIN_SCREEN;
	}
	else if (location_str == "bottom_panel")
	{
		location = FlowEditorConstants::FLOW_EDITOR_GUI_LOCATION_BOTTOM_PANEL;
	}

	return location;
}


FlowEditorConstants::FlowNodeCreationDialogAppearPosition FlowConfigManager::get_flow_node_creation_dialog_appear_position() const
{
	FlowEditorConstants::FlowNodeCreationDialogAppearPosition position = FlowEditorConstants::FLOW_NODE_CREATION_DIALOG_APPEAR_POSITION_SCREEN_CENTER;
	String position_str = get_editor_setting(ES_FLOW_NODE_CREATION_DIALOG_APPEAR_POSITION);

	if (position_str == "screen_center")
	{
		position = FlowEditorConstants::FLOW_NODE_CREATION_DIALOG_APPEAR_POSITION_SCREEN_CENTER;
	}
	else if (position_str == "under_mouse")
	{
		position = FlowEditorConstants::FLOW_NODE_CREATION_DIALOG_APPEAR_POSITION_UNDER_MOUSE;
	}

	return position;
}


bool FlowConfigManager::is_editing_flow_script_instances_allowed() const
{
	if (get_project_setting(PS_EDITING_FLOW_SCRIPT_INSTANCES_ALLOWED))
	{
		return true;
	}
	else
	{
		return false;
	}
}


PackedStringArray FlowConfigManager::get_flow_type_directories() const
{
	PackedStringArray directories = get_project_setting(PS_FLOW_TYPE_DIRECTORIES);
	for (int i = directories.size() - 1; i > -1; i--)
	{
		String current_dir = directories.get(i);
		if (!DirAccess::dir_exists_absolute(current_dir))
		{
			directories.remove_at(i);
		}
	}
	return directories;
}


Timer::TimerProcessCallback FlowConfigManager::get_wait_seconds_timer_process_step() const
{
	String process_step_str = get_project_setting(PS_WAIT_SECONDS_TIMER_PROCESS_STEP);

	if (process_step_str == "idle")
	{
		return Timer::TIMER_PROCESS_IDLE;
	}
	else if (process_step_str == "physics")
	{
		return Timer::TIMER_PROCESS_PHYSICS;
	}
	// default to physics
	else
	{
		return Timer::TIMER_PROCESS_PHYSICS;
	}
}


FlowConfigManager::FlowConfigManager()
{
	CRASH_COND_MSG(singleton != nullptr, "FlowConfigManager is a singleton and may not be instantiated multiple times.");
	singleton = this;
}


FlowConfigManager::~FlowConfigManager()
{
	singleton = nullptr;
}
