#ifndef FLOW_SCRIPT_NODE_WAIT_DURATION_HPP
#define FLOW_SCRIPT_NODE_WAIT_DURATION_HPP


#include "common.hpp"


class FlowScriptNodeWaitDuration : public FlowScriptNode
{
	GDCLASS(FlowScriptNodeWaitDuration, FlowScriptNode);

public:
	enum ConnectionList
	{
		CONNECTION_LIST_ADVANCE = 0,
	};

	enum TemporaryVariable
	{
		VARIABLE_TIMER = 0,
		VARIABLE_FINISH_NEXT_STEP = 1,
		VARIABLE_LOAD_SAVE_FLAG = 2,
		VARIABLE_LOAD_SAVE_DURATION = 3,
	};

private:
	FlowScriptBuiltInTimerProxy *get_timer(const FlowScriptNodeContext *p_context) const;

protected:
	static void _bind_methods();

	virtual double get_initial_duration(FlowScriptNodeContext *p_context) const = 0;

public:
	virtual void exec_startup(FlowScriptNodeContext *p_context) override;
	virtual void exec_step(FlowScriptNodeContext *p_context) override;
	virtual void exec_cleanup(FlowScriptNodeContext *p_context) override;
	virtual void get_output_connection_list_lengths(List<FlowScriptNodeConnectionListLength> *p_lengths) const override;
	virtual void set_runtime_state(FlowScriptNodeContext *p_context, const Dictionary &p_state) override;
	virtual Dictionary get_runtime_state(const FlowScriptNodeContext *p_context) const override;
};


VARIANT_ENUM_CAST(FlowScriptNodeWaitDuration::ConnectionList);
VARIANT_ENUM_CAST(FlowScriptNodeWaitDuration::TemporaryVariable);


#endif // FLOW_SCRIPT_NODE_WAIT_DURATION_HPP
