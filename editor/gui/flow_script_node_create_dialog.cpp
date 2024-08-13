#include "flow_script_node_create_dialog.hpp"
#include "editor/themes/editor_scale.h"


void FlowScriptNodeCreateDialog::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_POSTINITIALIZE:
			init_signals();
			break;
		case NOTIFICATION_VISIBILITY_CHANGED:
			if (is_visible())
			{
				type_tree->call_deferred(SNAME("grab_focus"));
			}
			break;
	}
}


void FlowScriptNodeCreateDialog::init_signals()
{
}


FlowScriptNodeCreateDialog::FlowScriptNodeCreateDialog()
{
	set_flag(FLAG_RESIZE_DISABLED, false);
	set_wrap_controls(true);
	set_min_size((Size2(512, 384) * EDSCALE).ceil());

	VBoxContainer *vbox = memnew(VBoxContainer);
	vbox->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	vbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	vbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	vbox->set_focus_mode(Control::FOCUS_NONE);
	add_child(vbox);

	type_tree = memnew(Tree);
	type_tree->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	type_tree->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	type_tree->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	type_tree->set_focus_mode(Control::FOCUS_CLICK);
	vbox->add_child(type_tree);

	help_bit = memnew(EditorHelpBit);
	vbox->add_margin_child(TTR("Description:"), help_bit, false);
}
