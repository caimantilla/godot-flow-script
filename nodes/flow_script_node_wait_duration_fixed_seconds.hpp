#ifndef FLOW_SCRIPT_NODE_WAIT_DURATION_FIXED_SECONDS_HPP
#define FLOW_SCRIPT_NODE_WAIT_DURATION_FIXED_SECONDS_HPP


#include "flow_script_node_wait_duration.hpp"


class FlowScriptNodeWaitDurationFixedSeconds final : public FlowScriptNodeWaitDuration
{
	GDCLASS(FlowScriptNodeWaitDurationFixedSeconds, FlowScriptNodeWaitDuration);

private:
	real_t duration = real_t(0);

protected:
	static void _bind_methods();
	virtual real_t get_initial_duration(FlowScriptNodeContext *p_context) const override;

public:
	void set_duration(const real_t &p_duration);
	real_t get_duration() const;
};


#endif // FLOW_SCRIPT_NODE_WAIT_DURATION_FIXED_SECONDS_HPP
