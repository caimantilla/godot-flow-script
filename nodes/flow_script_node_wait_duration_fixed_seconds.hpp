#ifndef FLOW_SCRIPT_NODE_WAIT_DURATION_FIXED_SECONDS_HPP
#define FLOW_SCRIPT_NODE_WAIT_DURATION_FIXED_SECONDS_HPP


#include "flow_script_node_wait_duration.hpp"


class FlowScriptNodeWaitDurationFixedSeconds final : public FlowScriptNodeWaitDuration
{
	GDCLASS(FlowScriptNodeWaitDurationFixedSeconds, FlowScriptNodeWaitDuration);

private:
	double duration = 0.0;

protected:
	static void _bind_methods();
	virtual double get_initial_duration(FlowScriptNodeContext *p_context) const override;

public:
	void set_duration(const double p_duration);
	double get_duration() const;
};


#endif // FLOW_SCRIPT_NODE_WAIT_DURATION_FIXED_SECONDS_HPP
