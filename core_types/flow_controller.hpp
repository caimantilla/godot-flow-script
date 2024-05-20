#ifndef FLOW_CONTROLLER_HPP
#define FLOW_CONTROLLER_HPP

#include "scene/main/node.h"
#include "flow_script.hpp"
#include "flow_node.hpp"
#include "flow_fiber.hpp"
#include "flow_bridge.hpp"

class FlowController final : public Node
{
	GDCLASS(FlowController, Node);

private:
	FlowBridge *flow_bridge = nullptr;
	FlowScript *flow_script = nullptr;
	Variant last_return_value = Variant();
	HashMap<FlowFiberID, FlowFiber *> flow_fiber_map;
	FlowFiberID next_flow_fiber_id = FLOW_FIBER_ID_MIN;

	void emit_flow_fiber_finished(const FlowFiberID flow_fiber_id, const Variant &p_return_value);
	void emit_all_flow_fibers_finished(const Variant &p_return_value);

	void delete_flow_fiber_instance(FlowFiber *p_flow_fiber);
	void update_next_flow_fiber_id();
	bool is_new_flow_fiber_id_valid(const FlowFiberID p_new_flow_fiber_id) const;

	void on_flow_fiber_finished(const Variant &p_return_value, const FlowFiberID p_flow_fiber_id);
	void on_flow_fiber_new_fibers_creation_request(const FlowNodeIDArray &p_initial_flow_node_ids, const bool p_wait_fibers_completion, const FlowFiberID p_calling_fiber_id);

	FlowFiber *create_new_flow_fiber(const FlowNodeID p_initial_flow_node_id, const Callable &p_finished_callback);
	// FlowFiber *_create_new_flow_fiber(const FlowFiberID p_flow_fiber_id, const FlowNodeID p_initial_flow_node_id, const Callable &p_finished_callback);

protected:
	static void _bind_methods();

public:
	void set_flow_script(FlowScript *p_flow_script);
	FlowScript *get_flow_script() const;

	void set_flow_bridge(FlowBridge *p_flow_bridge);
	FlowBridge *get_flow_bridge() const;

	bool has_flow_fiber(const FlowFiberID p_flow_fiber_id) const;
	FlowFiber *get_flow_fiber(const FlowFiberID p_flow_fiber_id) const;
	int get_flow_fiber_count() const;

	void start_with_return_callback(const String &p_initial_flow_node_name, const Callable &p_finished_callback);
	void start(const String &p_initial_flow_node_name);

	FlowController();
	~FlowController();
};

#endif