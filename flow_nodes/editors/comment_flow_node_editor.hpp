#ifndef COMMENT_FLOW_NODE_EDITOR_HPP
#define COMMENT_FLOW_NODE_EDITOR_HPP

#include "../comment_flow_node.hpp"
#include "../../core_types/flow_node_editor.hpp"
#include "scene/gui/text_edit.h"


class CommentFlowNodeEditor final : public FlowNodeEditor
{
	GDCLASS(CommentFlowNodeEditor, FlowNodeEditor);

private:
	Label *comment_box;

	CommentFlowNode *get_comment_flow_node() const;

protected:
	virtual void _clean_up() override;
	virtual void _flow_node_updated() override;

public:
	CommentFlowNodeEditor();
};


#endif
