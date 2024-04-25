#ifndef WAIT_SECONDS_FLOW_NODE_HPP
#define WAIT_SECONDS_FLOW_NODE_HPP

#include "../core_types/flow_node.hpp"

class WaitSecondsFlowNode final : public FlowNode
{
	GDCLASS(WaitSecondsFlowNode, FlowNode);

private:
	float timer_duration = 1.0f;
	FlowNodeID next_flow_node_id = FLOW_NODE_ID_NIL;

protected:
	static void _bind_methods();

	virtual void _execute(FlowNodeExecutionState *p_state) override;
	virtual bool _is_property_flow_node_reference(const StringName &p_name) const override;
	virtual void _on_external_flow_node_id_changed(const FlowNodeID p_from, const FlowNodeID p_to) override;

public:
	constexpr static const float MIN_DURATION = 0.1f;
	constexpr static const float MAX_DURATION = 500.0f;
	constexpr static const float STEP = 0.1f;

	void set_timer_duration_no_signal(const float p_seconds);
	void set_timer_duration(const float p_seconds);
	float get_timer_duration() const;
	void set_next_flow_node_id(const FlowNodeID p_node_id);
	FlowNodeID get_next_flow_node_id() const;
};

#endif
