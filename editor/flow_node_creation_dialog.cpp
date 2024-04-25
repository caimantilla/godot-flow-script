#include "flow_node_creation_dialog.hpp"
#include "../singletons/flow_type_db.hpp"
#include "editor/themes/editor_scale.h"


void FlowNodeCreationDialog::_bind_methods()
{
	ADD_SIGNAL(MethodInfo("flow_type_selected", PropertyInfo(Variant::STRING_NAME, "flow_type_id")));
	ADD_SIGNAL(MethodInfo("flow_type_chosen", PropertyInfo(Variant::STRING_NAME, "flow_type_id")));
}


void FlowNodeCreationDialog::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_POSTINITIALIZE:
			init_signals();
			break;
		case NOTIFICATION_VISIBILITY_CHANGED:
			if (is_visible())
			{
				flow_node_creation_tree->call_deferred(SNAME("grab_focus"));
			}
			break;
	}
}


FlowNodeCreationTree *FlowNodeCreationDialog::get_flow_node_creation_tree() const
{
	return flow_node_creation_tree;
}


String FlowNodeCreationDialog::get_selected_flow_type_id() const
{
	return flow_node_creation_tree->get_selected_flow_node_type_id();
}


void FlowNodeCreationDialog::update_flow_type_description()
{
	String new_description;
	String selected_type_id = get_selected_flow_type_id();

	if (FlowTypeDB::get_singleton()->has_type(selected_type_id))
	{
		Ref<FlowType> type_ref = FlowTypeDB::get_singleton()->get_type(selected_type_id);
		new_description = vformat("[b]%s[/b]: %s", type_ref->get_name(), type_ref->get_description()).replace("\n", " ").strip_edges();
	}
	else
	{
		new_description = "";
	}

	// flow_type_description_label->set_text(new_description);
	help_bit->set_text(new_description);
}


void FlowNodeCreationDialog::emit_selected_flow_type_chosen()
{
	String selected_type_id = get_selected_flow_type_id();
	emit_signal(SNAME("flow_type_chosen"), selected_type_id);
}


void FlowNodeCreationDialog::on_creation_tree_flow_type_selected(const String &p_flow_type_id)
{
	update_flow_type_description();
	emit_signal(SNAME("flow_type_selected"), p_flow_type_id);
}


void FlowNodeCreationDialog::init_signals()
{
	Callable type_selected_callback = callable_mp(this, &FlowNodeCreationDialog::on_creation_tree_flow_type_selected);
	flow_node_creation_tree->connect(SNAME("flow_type_selected"), type_selected_callback);

	Callable type_chosen_callback = callable_mp(this, &FlowNodeCreationDialog::emit_selected_flow_type_chosen);
	flow_node_creation_tree->connect(SNAME("item_activated"), type_chosen_callback);
	connect(SNAME("confirmed"), type_chosen_callback);
}


FlowNodeCreationDialog::FlowNodeCreationDialog()
{
	Size2i min_size = Size2i((EDSCALE * Size2(512, 384)).ceil());

	set_flag(FLAG_RESIZE_DISABLED, false);
	set_wrap_controls(true);
	set_min_size(min_size);

	VBoxContainer *vbox = memnew(VBoxContainer);
	vbox->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	vbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	vbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	vbox->set_focus_mode(Control::FOCUS_NONE);
	add_child(vbox);

	flow_node_creation_tree = memnew(FlowNodeCreationTree);
	flow_node_creation_tree->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	flow_node_creation_tree->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	flow_node_creation_tree->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	flow_node_creation_tree->set_focus_mode(Control::FOCUS_CLICK);
	vbox->add_child(flow_node_creation_tree);

	// vbox->add_spacer();

	// VBoxContainer *description_container = memnew(VBoxContainer);
	// description_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	// description_container->set_v_size_flags(Control::SIZE_SHRINK_END);
	// description_container->set_focus_mode(Control::FOCUS_NONE);
	// vbox->add_child(description_container);

	help_bit = memnew(EditorHelpBit);
	vbox->add_margin_child("Description:", help_bit, false);

	// RichTextLabel *description_header_label = memnew(RichTextLabel);
	// description_header_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	// description_header_label->set_v_size_flags(Control::SIZE_SHRINK_END);
	// description_header_label->set_focus_mode(Control::FOCUS_NONE);
	// description_header_label->set_text("[b]Description:[/b]");
	// description_header_label->set_autowrap_mode(TextServer::AUTOWRAP_OFF);
	// description_header_label->set_fit_content(true);
	// description_header_label->set_use_bbcode(true);
	// description_header_label->set_scroll_active(false);
	// description_container->add_child(description_header_label);
	// description_container->add_margin_child("Description:", )

	// description_container->add_spacer();

	// flow_type_description_label = memnew(RichTextLabel);
	// flow_type_description_label->set_custom_minimum_size(Size2(0, ceilf(EDSCALE * 80.0f)));
	// flow_type_description_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	// flow_type_description_label->set_v_size_flags(Control::SIZE_SHRINK_END);
	// flow_type_description_label->set_focus_mode(Control::FOCUS_NONE);
	// flow_type_description_label->set_use_bbcode(true);
	// flow_type_description_label->set_scroll_active(false);
	// flow_type_description_label->set_fit_content(true);
	// flow_type_description_label->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
	// description_container->add_child(flow_type_description_label);
}
