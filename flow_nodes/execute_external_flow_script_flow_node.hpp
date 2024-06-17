#ifndef EXECUTE_EXTERNAL_FLOW_SCRIPT_FLOW_NODE_HPP
#define EXECUTE_EXTERNAL_FLOW_SCRIPT_FLOW_NODE_HPP

#include "../core_types/flow_node.hpp"
#include "../core_types/flow_script.hpp"


class ExecuteExternalFlowScriptFlowNode final : public FlowNode {
	GDCLASS(ExecuteExternalFlowScriptFlowNode, FlowNode);

private:
	FlowNodeID next_flow_node_id = FLOW_NODE_ID_NIL;
	bool wait_external_flow_script_finished = false;
	Ref<FlowScript> external_flow_script;
	String external_call_name;

	PackedStringArray get_external_flow_script_procedure_names() const;
	void on_awaiting_sub_flow_controller_finished(const Variant &p_return_value, FlowNodeExecutionState *p_state, FlowController *p_sub_controller);
	void on_immediate_sub_flow_controller_finished(const Variant &p_return_value, FlowController *p_sub_controller);

protected:
	static void _bind_methods();
	void _validate_property(PropertyInfo &p_property) const;

	virtual void _execute(FlowNodeExecutionState *p_state) override;
	virtual bool _is_property_flow_node_reference(const StringName &p_name) const override;
	virtual void _on_external_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to) override;

public:
	void set_next_flow_node_id(const FlowNodeID p_flow_node_id);
	FlowNodeID get_next_flow_node_id() const;

	void set_wait_external_flow_script_finished(const bool p_enabled);
	bool is_wait_external_flow_script_finished_enabled() const;

	void set_external_flow_script(const Ref<FlowScript> &p_external_flow_script);
	Ref<FlowScript> get_external_flow_script() const;

	void set_external_call_name(const String &p_external_call_name);
	String get_external_call_name() const;
};


#endif
