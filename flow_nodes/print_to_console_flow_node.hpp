#ifndef PRINT_TO_CONSOLE_FLOW_NODE_HPP
#define PRINT_TO_CONSOLE_FLOW_NODE_HPP

#include "../core_types/flow_node.hpp"


class PrintToConsoleFlowNode final : public FlowNode
{
	GDCLASS(PrintToConsoleFlowNode, FlowNode);

private:
	FlowNodeID next_flow_node_id = FLOW_NODE_ID_NIL;
	String console_message;
	String format_expressions;

protected:
	static void _bind_methods();

	virtual void _execute(FlowNodeExecutionState *p_state) override;
	virtual bool _is_property_flow_node_reference(const StringName &p_name) const override;
	virtual void _on_external_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to) override;

public:
	void set_next_flow_node_id(const FlowNodeID p_node_id);
	FlowNodeID get_next_flow_node_id() const;
	void set_console_message(const String &p_console_message);
	String get_console_message() const;
	void set_format_expressions(const String &p_format_expressions);
	String get_format_expressions() const;
};


#endif
