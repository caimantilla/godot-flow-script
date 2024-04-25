#ifndef FLOW_NODE_CREATION_DIALOG_HPP
#define FLOW_NODE_CREATION_DIALOG_HPP

#include "flow_node_creation_tree.hpp"
#include "scene/gui/dialogs.h"
#include "scene/gui/rich_text_label.h"
#include "editor/editor_help.h"


class FlowNodeCreationDialog final : public ConfirmationDialog
{
	GDCLASS(FlowNodeCreationDialog, ConfirmationDialog);

private:
	FlowNodeCreationTree *flow_node_creation_tree;
	EditorHelpBit *help_bit;

	void init_signals();

	void update_flow_type_description();

	void emit_selected_flow_type_chosen();

	void on_creation_tree_flow_type_selected(const String &p_flow_type_id);

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	FlowNodeCreationTree *get_flow_node_creation_tree() const;
	String get_selected_flow_type_id() const;

	FlowNodeCreationDialog();
};


#endif
