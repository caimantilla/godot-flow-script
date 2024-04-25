#ifndef PRINT_TO_CONSOLE_FLOW_NODE_EDITOR_HPP
#define PRINT_TO_CONSOLE_FLOW_NODE_EDITOR_HPP

#include "../print_to_console_flow_node.hpp"
#include "../../core_types/flow_node_editor.hpp"


class PrintToConsoleFlowNodeEditor final : public FlowNodeEditor
{
	GDCLASS(PrintToConsoleFlowNodeEditor, FlowNodeEditor);

private:
	Label *console_message_label;
	Label *format_expressions_label;

	PrintToConsoleFlowNode *get_ptc_flow_node() const;

protected:
	virtual void _clean_up() override;
	virtual void _flow_node_updated() override;
	virtual int _get_input_slot() const override;
	virtual void _set_out_going_connection(const Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection) override;
	virtual TypedArray<FlowNodeEditorOutGoingConnectionParameters> _get_out_going_connections() const override;

public:
	PrintToConsoleFlowNodeEditor();
};


#endif
