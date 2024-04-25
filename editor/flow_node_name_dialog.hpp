#ifndef FLOW_NODE_NAME_DIALOG_HPP
#define FLOW_NODE_NAME_DIALOG_HPP

#include "scene/gui/dialogs.h"
#include "scene/gui/line_edit.h"
#include "../flow_constants.hpp"


class FlowNodeNameDialog final : public ConfirmationDialog
{
	GDCLASS(FlowNodeNameDialog, ConfirmationDialog);

private:
	LineEdit *id_line_editor;
	AcceptDialog *invalid_id_alert;

	FlowNodeID target_flow_node_id; // Used for renaming
	String flow_type_id; // Used for creation

	void on_id_line_editor_visibility_changed();
	void on_id_line_editor_text_submitted(const String &p_new_text);

	void on_confirmed();

protected:
	static void _bind_methods();

public:
	void set_target_flow_node_id(const FlowNodeID p_id);
	FlowNodeID get_target_flow_node_id() const;
	void set_flow_type_id(const String &p_id);
	String get_flow_type_id() const;

	String get_entered_flow_node_name() const;

	void alert_invalid_name();

	FlowNodeNameDialog();
};


#endif
