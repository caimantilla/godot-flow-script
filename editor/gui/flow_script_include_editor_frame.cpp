#include "flow_script_include_editor_frame.hpp"
#include "scene/gui/box_container.h"
#include "editor/editor_string_names.h"


void FlowScriptIncludeEditorFrame::_notification(int p_what)
{
	if (p_what == NOTIFICATION_THEME_CHANGED)
	{
		Ref<Texture2D> remove_icon = get_theme_icon(SNAME("Close"), EditorStringName(EditorIcons));
		remove_script_button->set_icon(remove_icon);
	}
}


void FlowScriptIncludeEditorFrame::request_removal()
{
	emit_signal(SNAME("delete_request"));
}


void FlowScriptIncludeEditorFrame::update_title()
{
	if (flow_script.is_valid())
	{
		set_title(vformat(TTR("#%d - %s"), include_id, flow_script->get_path()));
	}
	else
	{
		set_title(vformat(TTR("#%d - No Script"), include_id));
	}
}


FlowScriptIncludeEditorFrame::FlowScriptIncludeEditorFrame()
{
	set_autoshrink_enabled(true);
	set_resizable(false);

	remove_script_button = memnew(Button);
	remove_script_button->connect(SceneStringName(pressed), callable_mp(this, &FlowScriptIncludeEditorFrame::request_removal));
	remove_script_button->set_h_size_flags(SIZE_SHRINK_END);
	get_titlebar_hbox()->add_child(remove_script_button);
}
