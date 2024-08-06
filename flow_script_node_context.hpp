#ifndef FLOW_SCRIPT_NODE_CONTEXT_HPP
#define FLOW_SCRIPT_NODE_CONTEXT_HPP


#include "typedefs.hpp"
#include "core/object/object.h"
#include "core/object/class_db.h"


class FlowScript;
class FlowScriptNode;
class FlowScriptExecutionController;
class FlowScriptBridge;


class FlowScriptNodeContext final : public Object
{
	friend class FlowScriptExecutionController;

	GDCLASS(FlowScriptNodeContext, Object);

public:
	static const uint8_t VARIABLES_MAX = 31;

private:
	struct VariableSlot
	{
		bool used = false;
		Variant value;
	};

	FlowScriptExecutionFiberID self_id;
	FlowScriptExecutionController *execution_controller_ptr = nullptr;
	FlowScriptNodeID current_node_id;
	bool exec_blocked = false;
	VariableSlot variables[VARIABLES_MAX];
	VariableSlot return_variable;

	String create_variable_idx_out_of_range_error(const uint8_t p_idx) const;

	Callable cb_on_external_branch_finish;

	bool bind_await_branch_list(const PackedInt32Array &p_initial_node_ids);

protected:
	static void _bind_methods();

public:
	// Fibers that are being waited on are encoded as flags
	int32_t awaiting_fibers_bits = 0;
	Callable finished_callback;

	void reset();
	bool is_active() const;
	void set_awaiting_fiber(const FlowScriptExecutionFiberID p_fiber_id, const bool p_enabled);
	bool is_awaiting_fiber(const FlowScriptExecutionFiberID p_fiber_id) const;
	void set_return_value(const Variant &p_value);
	Variant get_return_value() const;
	void advance_to_node(const FlowScriptNodeID p_next_node_id);
	void invoke_step();
	void set_variable(const uint8_t p_idx, const Variant &p_value);
	Variant get_variable(const uint8_t p_idx) const;
	bool has_variable(const uint8_t p_idx) const;
	void clear_variables();
	FlowScriptNodeID get_current_node_id() const;
	Ref<FlowScriptNode> get_current_node_ref() const;
	FlowScriptNode *get_current_node_ptr() const;
	Ref<FlowScript> get_flow_script_ref() const;
	FlowScript *get_flow_script_ptr() const;
	FlowScriptBridge *get_bridge_ptr() const;
	FlowScriptExecutionController *get_execution_controller_ptr() const;
	void set_state(const Dictionary &p_state);
	void get_state(Dictionary &r_state) const;
	bool await_branch_list(const List<FlowScriptNodeID> p_initial_node_ids);
	bool await_branch_solo(const FlowScriptNodeID p_initial_node_id);

	FlowScriptNodeContext();
};


#endif // FLOW_SCRIPT_NODE_CONTEXT_HPP
