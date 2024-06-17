#ifndef FLOW_FIBER_HPP
#define FLOW_FIBER_HPP

#include "core/object/object.h"
#include "flow_bridge.hpp"
#include "flow_script.hpp"
#include "flow_node.hpp"
#include "flow_node_execution_state.hpp"

class FlowController;

class FlowFiber final : public Object
{
	GDCLASS(FlowFiber, Object);

	friend class FlowController;

private:
	FlowFiberID flow_fiber_id;
	FlowBridge *flow_bridge = nullptr;
	FlowController *flow_controller = nullptr;
	Ref<FlowScript> flow_script;

	FlowNode *current_flow_node = nullptr;
	FlowNodeExecutionState *current_flow_node_execution_state = nullptr;

	HashSet<FlowFiberID> resume_dependent_fiber_id_set;
	Variant return_value;
	Callable finished_callback;

	void add_resume_dependent_fiber(const FlowFiberID p_other_fiber_id);
	void remove_resume_dependent_fiber(const FlowFiberID p_other_fiber_id);
	bool is_resume_dependent_on_fiber(const FlowFiberID p_other_fiber_id) const;
	bool has_resume_dependencies() const;

	void start_using_id(const FlowNodeID p_initial_flow_node_id);
	void start_using_name(const String &p_initial_flow_node_name);
	void execute_flow_node_using_id(const FlowNodeID p_flow_node_id);
	void execute_flow_node_using_name(const String &p_flow_node_name);
	void delete_flow_node_execution_state(FlowNodeExecutionState *p_flow_node_execution_state) const;
	void on_new_fibers_requested(const FlowNodeIDArray &p_initial_flow_node_ids, bool p_wait_fibers_completion);
	void on_flow_node_finished(const FlowNodeID p_next_flow_node_id, const Variant &p_return_value);
	void on_finished(const Variant &p_return_value);
	void on_external_fiber_finished(const FlowFiberID p_other_fiber_id);

protected:
	static void _bind_methods();

public:
	FlowFiberID get_flow_fiber_id() const;
	FlowBridge *get_flow_bridge() const;
	FlowController *get_flow_controller() const;
	Ref<FlowScript> get_flow_script() const;

	FlowNode *get_current_flow_node() const;
	FlowNodeID get_current_flow_node_id() const;
	FlowNodeExecutionState *get_current_flow_node_execution_state() const;

	Variant get_return_value() const;

	void set_finished_callback(const Callable &p_callback);
	Callable get_finished_callback() const;

	bool is_finished() const;
	bool is_active() const;
};

#endif
