#ifndef MULTI_BRANCH_EXECUTION_FLOW_NODE_EDITOR_HPP
#define MULTI_BRANCH_EXECUTION_FLOW_NODE_EDITOR_HPP

#include "../multi_branch_execution_flow_node.hpp"
#include "../../core_types/flow_node_editor.hpp"


// NOTE: This editor is unfinished. Doesn't really work... fix it up later.
class MultiBranchExecutionFlowNodeEditor final : public FlowNodeEditor
{
	GDCLASS(MultiBranchExecutionFlowNodeEditor, FlowNodeEditor);

	class ExecutionStackEntryGUIComponent;

private:
	List<ExecutionStackEntryGUIComponent *> execution_stack_gui_components;

	MultiBranchExecutionFlowNode *get_mbe_flow_node() const;
	ExecutionStackEntryGUIComponent *create_execution_stack_entry_gui_component(const int p_idx, const bool p_last);
	void clear_execution_stack_gui_components();
	void reload_execution_stack_gui_components();

protected:
	virtual int _get_input_slot() const override;
	virtual void _set_out_going_connection(const Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection) override;
	virtual TypedArray<FlowNodeEditorOutGoingConnectionParameters> _get_out_going_connections() const override;
	virtual void _flow_node_updated() override;

public:
	MultiBranchExecutionFlowNodeEditor();
};


#endif
