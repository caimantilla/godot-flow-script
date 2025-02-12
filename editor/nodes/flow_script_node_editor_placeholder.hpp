#ifndef FLOW_SCRIPT_NODE_EDITOR_PLACEHOLDER_HPP
#define FLOW_SCRIPT_NODE_EDITOR_PLACEHOLDER_HPP


#include "flow_script_node_editor.hpp"


class Label;


class FlowScriptNodeEditorPlaceholder final : public FlowScriptNodeEditor
{
	GDCLASS(FlowScriptNodeEditorPlaceholder, FlowScriptNodeEditor);

public:
	enum
	{
		ANON_INPUT_SLOT = 0,
		ANON_INPUT_PORT = 0,
		ANON_OUTPUT_SLOT = 0,
		ANON_OUTPUT_PORT = 0,
	};

private:
	Label *placeholder_indicate_label;

public:
	virtual bool is_placeholder() const override;
	virtual void sync() override;
	virtual int get_input_slot() const override;
	virtual int output_connection_to_graph_slot(const FlowScriptNodeOutputConnection &p_connection) const override;
	virtual FlowScriptNodeOutputConnection output_graph_slot_to_connection(const int p_graph_slot) const override;

	FlowScriptNodeEditorPlaceholder();
};


#endif // FLOW_SCRIPT_NODE_EDITOR_PLACEHOLDER_HPP
