#ifndef FLOW_SCRIPT_EXECUTION_CONTROLLER_HPP
#define FLOW_SCRIPT_EXECUTION_CONTROLLER_HPP


#include "scene/main/node.h"
#include "typedefs.hpp"
#include "flow_script_node_context.hpp"


class FlowScript;
class FlowScriptBridge;


class FlowScriptExecutionController final : public Node
{
	GDCLASS(FlowScriptExecutionController, Node);

public:
	enum : FlowScriptExecutionFiberID
	{
		FIBER_ID_INVALID = -1,
		FIBERS_MAX = 28,
	};

private:
	static String ERR_STR_NO_AVAILABLE_FIBERS;

	Ref<FlowScript> flow_script;
	FlowScriptBridge *bridge = nullptr;
	FlowScriptNodeContext fiber_list[FIBERS_MAX];
	FlowScriptExecutionFiberID cache_next_free_fiber_id = 0;

	void update_cache_next_free_fiber_id(const FlowScriptExecutionFiberID p_min_new_id);

protected:
	static void _bind_methods();

public:
	void set_flow_script(const Ref<FlowScript> &p_flow_script);
	Ref<FlowScript> get_flow_script() const;
	FlowScript *get_flow_script_ptr() const;
	void set_bridge(FlowScriptBridge *p_bridge);
	FlowScriptBridge *get_bridge() const;
	void set_state(const Dictionary &p_state);
	Dictionary get_state() const;
	int get_active_fiber_count() const;
	bool is_active() const;
	bool can_create_fiber() const;
	void execute_branch_with_finish_callback(const FlowScriptNodeID p_initial_node_id, const Callable &p_finish_callback);
	bool has_fiber(const FlowScriptExecutionFiberID p_fiber_id) const;
	void internal_fiber_finish(const FlowScriptExecutionFiberID p_fiber_id);
	int32_t internal_execute_sub_branch_list(const List<FlowScriptNodeID> p_initial_node_ids);

	FlowScriptExecutionController();
	~FlowScriptExecutionController();
};


#endif // FLOW_SCRIPT_EXECUTION_CONTROLLER_HPP
