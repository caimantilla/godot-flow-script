#ifndef FLOW_SCRIPT_NODE_EDITOR_WAIT_DURATION_FIXED_SECONDS_HPP
#define FLOW_SCRIPT_NODE_EDITOR_WAIT_DURATION_FIXED_SECONDS_HPP


#include "flow_script_node_editor.hpp"
#include "scene/gui/label.h"


class FlowScriptNodeEditorWaitDurationFixedSeconds final : public FlowScriptNodeEditor
{
	GDCLASS(FlowScriptNodeEditorWaitDurationFixedSeconds, FlowScriptNodeEditor);

private:
	Label *label;

public:
	virtual void sync() override;
	virtual FlowScriptNodeOutputConnection output_graph_slot_to_connection(const int p_graph_slot) const override;
	virtual int output_connection_to_graph_slot(const FlowScriptNodeOutputConnection &p_connection) const override;
	virtual int get_input_slot() const override;

	FlowScriptNodeEditorWaitDurationFixedSeconds();
};


#endif // FLOW_SCRIPT_NODE_EDITOR_WAIT_DURATION_FIXED_SECONDS_HPP
