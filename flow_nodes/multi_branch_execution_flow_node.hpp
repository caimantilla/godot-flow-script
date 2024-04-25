#ifndef MULTI_BRANCH_EXECUTION_FLOW_NODE_HPP
#define MULTI_BRANCH_EXECUTION_FLOW_NODE_HPP

#include "../core_types/flow_node.hpp"


class MultiBranchExecutionFlowNode : public FlowNode
{
	GDCLASS(MultiBranchExecutionFlowNode, FlowNode);

private:
	FlowNodeID next_flow_node_id = FLOW_NODE_ID_NIL;
	FlowNodeIDArray execution_stack;

	void _set_execution_stack_entry_at(const int p_idx, const FlowNodeID p_new_flow_node_id, const bool p_emit);
	void _resize_execution_stack(const int p_size, const bool p_emit);
	void _remove_execution_stack_entry_at(const int p_idx, const bool p_emit);

protected:
	static void _bind_methods();
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _property_can_revert(const StringName &p_name) const;
	bool _property_get_revert(const StringName &p_name, Variant &r_property) const;

	virtual void _on_external_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to) override;
	virtual bool _is_property_flow_node_reference(const StringName &p_name) const override;

public:
	void set_next_flow_node_id(const FlowNodeID p_node_id);
	int get_next_flow_node_id() const;

	void set_execution_stack(const FlowNodeIDArray &p_execution_stack);
	FlowNodeIDArray get_execution_stack() const;
	int get_execution_stack_size() const;
	void set_execution_stack_entry_at(const int p_idx, const FlowNodeID p_new_flow_node_id);
	FlowNodeID get_execution_stack_entry_at(const int p_idx) const;
	void remove_execution_stack_entry_at(const int p_idx);
	void resize_execution_stack(const int p_size);
	int get_max_execution_stack_size() const;
};


#endif
