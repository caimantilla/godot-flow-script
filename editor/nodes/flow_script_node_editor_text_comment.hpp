#ifndef FLOW_SCRIPT_NODE_EDITOR_TEXT_COMMENT_HPP
#define FLOW_SCRIPT_NODE_EDITOR_TEXT_COMMENT_HPP


#include "flow_script_node_editor.hpp"
#include "scene/gui/rich_text_label.h"


class FlowScriptNodeEditorTextComment final : public FlowScriptNodeEditor
{
	GDCLASS(FlowScriptNodeEditorTextComment, FlowScriptNodeEditor);

private:
	RichTextLabel *comment_box;

public:
	virtual void sync() override;

	FlowScriptNodeEditorTextComment();
};


#endif // FLOW_SCRIPT_NODE_EDITOR_TEXT_COMMENT_HPP
