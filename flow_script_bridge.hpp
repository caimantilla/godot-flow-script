#ifndef FLOW_SCRIPT_BRIDGE_HPP
#define FLOW_SCRIPT_BRIDGE_HPP


#include <array>

#include "core/object/gdvirtual.gen.inc"
#include "scene/main/node.h"
#include "flow_script.hpp"
#include "flow_script_typedefs.hpp"
#include "flow_script_constants.hpp"
#include "flow_script_node_context.hpp"
#include "flow_script_built_in_node_interface.hpp"


class FlowScriptBridge : public Node
{
	GDCLASS(FlowScriptBridge, Node);

private:
	static String ERR_STR_NO_AVAILABLE_FIBERS;

	Ref<FlowScript> flow_script;
	std::array<FlowScriptNodeContext, FlowScriptConstants::FIBERS_MAX> fiber_list;
	FlowScriptExecutionFiberID cache_next_free_fiber_id = 0;
	FlowScriptBuiltInNodeInterface *built_in_node_interface = nullptr;

	void update_cache_next_free_fiber_id();

protected:
	static void _bind_methods();

	virtual FlowScriptBuiltInNodeInterface *create_built_in_node_interface();

	GDVIRTUAL0R(FlowScriptBuiltInNodeInterface *, _create_built_in_node_interface);

public:
	void set_flow_script(const Ref<FlowScript> &p_flow_script);
	Ref<FlowScript> get_flow_script() const;
	FlowScript *get_flow_script_ptr() const;
	void set_state(const Dictionary &p_state);
	Dictionary get_state() const;
	int get_active_fiber_count() const;
	bool is_active() const;
	bool can_create_fiber() const;
	void execute_branch_with_finish_callback(const FlowScriptNodeID p_initial_node_id, const Callable &p_finish_callback);
	bool has_fiber(const FlowScriptExecutionFiberID p_fiber_id) const;
	void internal_fiber_finish(const FlowScriptExecutionFiberID p_fiber_id);
	// Initializes a branch for later execution
	FlowScriptExecutionFiberID internal_init_branch(const FlowScriptNodeReference &p_node_reference);
	bool internal_exec_branch(const FlowScriptExecutionFiberID p_fiber_id);

	FlowScriptBuiltInNodeInterface *get_built_in_node_interface();

	FlowScriptBridge();
	~FlowScriptBridge();
};


#endif // FLOW_SCRIPT_BRIDGE_HPP
