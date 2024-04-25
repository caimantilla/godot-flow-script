#include "flow_node_name_dialog.hpp"
#include "core/core_string_names.h"
#include "scene/scene_string_names.h"


void FlowNodeNameDialog::_bind_methods()
{
	ADD_SIGNAL(MethodInfo("flow_node_id_confirmed", PropertyInfo(Variant::STRING_NAME, "new_flow_node_id")));
}


void FlowNodeNameDialog::set_target_flow_node_id(const FlowNodeID p_id)
{
	target_flow_node_id = p_id;
}


FlowNodeID FlowNodeNameDialog::get_target_flow_node_id() const
{
	return target_flow_node_id;
}


void FlowNodeNameDialog::set_flow_type_id(const String &p_id)
{
	flow_type_id = p_id;
}


String FlowNodeNameDialog::get_flow_type_id() const
{
	return flow_type_id;
}


String FlowNodeNameDialog::get_entered_flow_node_name() const
{
	return id_line_editor->get_text().strip_edges();
}


void FlowNodeNameDialog::alert_invalid_name()
{
	// String str_new_id = get_entered_flow_node_name();
	// String alert_msg;

	// if (str_new_id.is_empty())
	// {
	// 	alert_msg = "FlowNode ID is empty.";
	// }
	// else
	// {
	// 	alert_msg = vformat("Failed to use FlowNode ID \"%s\".", str_new_id);
	// }

	// invalid_id_alert->set_text(alert_msg);
	// invalid_id_alert->popup_centered();
}


void FlowNodeNameDialog::on_id_line_editor_visibility_changed()
{
	if (id_line_editor->is_visible_in_tree())
	{
		id_line_editor->grab_focus();
	}
}


void FlowNodeNameDialog::on_id_line_editor_text_submitted(const String &p_new_text)
{
	if (p_new_text.is_empty())
	{
		return;
	}

	on_confirmed();
}


void FlowNodeNameDialog::on_confirmed()
{
	// emit_signal("flow_node_id_confirmed", get_entered_flow_node_id());
}


FlowNodeNameDialog::FlowNodeNameDialog()
{
	set_hide_on_ok(false);

	id_line_editor = memnew(LineEdit);
	id_line_editor->set_placeholder("FlowNode ID...");
	id_line_editor->connect(SceneStringNames::get_singleton()->visibility_changed, callable_mp(this, &FlowNodeNameDialog::on_id_line_editor_visibility_changed));
	id_line_editor->connect(SNAME("text_submitted"), callable_mp(this, &FlowNodeNameDialog::on_id_line_editor_text_submitted));
	add_child(id_line_editor);

	invalid_id_alert = memnew(AcceptDialog);
	invalid_id_alert->set_title("Invalid FlowNode ID!");
	add_child(invalid_id_alert);
}
