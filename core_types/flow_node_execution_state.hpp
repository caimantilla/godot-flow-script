#ifndef FLOW_NODE_EXECUTION_STATE_HPP
#define FLOW_NODE_EXECUTION_STATE_HPP

#include "core/object/class_db.h"
#include "core/object/object.h"
#include "../flow_constants.hpp"
#include "flow_bridge.hpp"

class FlowController;
class FlowScript;

class FlowNodeExecutionState final : public Object
{
	GDCLASS(FlowNodeExecutionState, Object);

	friend class FlowFiber;

private:
	FlowBridge *flow_bridge = nullptr;
	FlowController *flow_controller = nullptr;
	FlowScript *flow_script = nullptr;

	void resume();

protected:
	static void _bind_methods();

public:
	FlowBridge *get_flow_bridge() const;
	FlowController *get_flow_controller() const;
	FlowScript *get_flow_script() const;

	void set_temporary_variable(const StringName &p_name, const Variant &p_value);
	Variant get_temporary_variable(const StringName &p_name) const;
	bool has_temporary_variable(const StringName &p_name) const;
	PackedStringArray get_temporary_variable_names() const;
	void restore_temporary_state(const Dictionary &p_temporary_state);
	Dictionary get_temporary_state() const;

	void finish(const FlowNodeID p_next_flow_node_id, const Variant &p_return_value);
	void request_multiple_new_fibers(const FlowNodeIDArray &p_initial_flow_node_ids, bool p_wait_fibers_completion);
	void request_new_fiber(const FlowNodeID p_flow_node_id, bool p_wait_fiber_completion);
};

#endif
