#ifndef FLOW_SCRIPT_NODE_CREATE_DIALOG_HPP
#define FLOW_SCRIPT_NODE_CREATE_DIALOG_HPP


#include "core/templates/hash_map.h"
#include "core/templates/list.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/tree.h"
#include "editor/editor_help.h"


class FlowScriptNodeCreateDialog final : public ConfirmationDialog
{
	GDCLASS(FlowScriptNodeCreateDialog, ConfirmationDialog);

private:
	Tree *type_tree;
	EditorHelpBit *help_bit;

	HashMap<String, TreeItem *> item_map_type_category;
	HashMap<String, TreeItem *> item_map_type_id;

	void init_signals();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	FlowScriptNodeCreateDialog();
};


#endif // FLOW_SCRIPT_NODE_CREATE_DIALOG_HPP
