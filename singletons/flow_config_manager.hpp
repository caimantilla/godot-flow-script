#ifndef FLOW_CONFIG_MANAGER_HPP
#define FLOW_CONFIG_MANAGER_HPP

#include "core/object/object.h"
#include "core/object/class_db.h"
#include "core/variant/array.h"
#include "../core_types/flow_type.hpp"
#include "../editor/flow_editor_constants.hpp"
#include "scene/main/timer.h"

class FlowConfigManager final : public Object
{
	GDCLASS(FlowConfigManager, Object);

	friend class FlowScriptEditorPlugin;

private:
	static FlowConfigManager *singleton;

	Variant get_project_setting(const String &p_name) const;
	Variant get_editor_setting(const String &p_name) const;


public:
	static FlowConfigManager *get_singleton();

	void initialize_project_settings();
	void initialize_editor_settings();

	FlowEditorConstants::FlowGUIEditorLocation get_flow_editor_gui_location() const;
	FlowEditorConstants::FlowNodeCreationDialogAppearPosition get_flow_node_creation_dialog_appear_position() const;

	bool is_editing_flow_script_instances_allowed() const;
	PackedStringArray get_flow_type_directories() const;
	Timer::TimerProcessCallback get_wait_seconds_timer_process_step() const;

	FlowConfigManager();
	~FlowConfigManager();
};

#endif
