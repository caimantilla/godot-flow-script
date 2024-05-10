#ifndef MULTI_BRANCH_EXECUTION_FLOW_NODE_HPP
#define MULTI_BRANCH_EXECUTION_FLOW_NODE_HPP

#include "../core_types/flow_node.hpp"


class MultiBranchExecutionFlowNode : public FlowNode
{
	GDCLASS(MultiBranchExecutionFlowNode, FlowNode);

protected:
	static void _bind_methods();

	FlowNodeID next_flow_node_id = FLOW_NODE_ID_NIL;
	FlowNodeIDArray execution_stack = FlowNodeIDArray();

	virtual bool _is_property_flow_node_reference(const StringName &p_name) const override;
	virtual bool _is_property_flow_node_reference_list(const StringName &p_name) const override;
	virtual void _on_external_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to) override;

public:
	static const int MIN_STACK_SIZE = 0;
	static const int MAX_STACK_SIZE = 9;

	void set_next_flow_node_id(const FlowNodeID p_node_id);
	int get_next_flow_node_id() const;

	void set_execution_stack(const FlowNodeIDArray &p_execution_stack);
	FlowNodeIDArray get_execution_stack() const;

	void set_execution_stack_size(int p_size);
	int get_execution_stack_size() const;

	void set_execution_stack_entry_at(const int p_idx, const FlowNodeID p_flow_node_id);
	FlowNodeID get_execution_stack_entry_at(const int p_idx) const;

	void add_execution_stack_entry(const FlowNodeID p_flow_node_id);
	void remove_execution_stack_entry_at(const int p_idx);
};


#endif
