#include "flow_type_definition_dialog.hpp"
#include "scene/gui/box_container.h"
#include "scene/gui/label.h"


void FlowTypeDefinitionDialog::reset_edited_flow_type()
{
	edited_flow_type;
}


FlowTypeDefinitionDialog::FlowTypeDefinitionDialog()
{
	VBoxContainer *main_vbox = memnew(VBoxContainer);
	main_vbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	main_vbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	add_child(main_vbox);

	HBoxContainer *type_id_hbox = memnew(HBoxContainer);
	type_id_hbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	main_vbox->add_child(type_id_hbox);

	Label *type_id_label = memnew(Label);
	type_id_label->set_text("Type ID:");
	type_id_hbox->add_child(type_id_label);

	type_id_line = memnew(LineEdit);
	type_id_line->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	type_id_hbox->add_child(type_id_line);
}
