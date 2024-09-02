#ifndef FLOW_SCRIPT_NODE_CONTEXT_HPP
#define FLOW_SCRIPT_NODE_CONTEXT_HPP


#include "typedefs.hpp"
#include "core/object/object.h"
#include "core/object/class_db.h"
#include "flow_script_node_reference.hpp"
#include "flow_script_node_output_connection.hpp"


class FlowScript;
class FlowScriptNode;
class FlowScriptBridge;


class FlowScriptNodeContext final : public Object
{
	friend class FlowScriptBridge;

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
	FlowScriptBridge *bridge_ptr = nullptr;

	FlowScript *current_flow_script;
	FlowScriptNodeID current_node_id;
	bool exec_blocked = false;
	VariableSlot variables[VARIABLES_MAX];
	VariableSlot return_variable;

	Callable cb_on_external_branch_finish;

	String create_variable_idx_out_of_range_error(const uint8_t p_idx) const;
	void bind_advance(const uint8_t p_connection_list, const int64_t p_connection_slot);
	void bind_add_await_branch(const uint8_t p_connection_list, const int64_t p_connection_slot);
	void bind_execute_await_branches();

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
	bool prepare_for_execution(const Ref<FlowScript> &p_flow_script, const FlowScriptNodeID p_node_id);
	void execute_current_node();
	bool start(const Ref<FlowScript> &p_flow_script, const FlowScriptNodeID p_initial_node_id);
	void advance(const FlowScriptNodeOutputConnection &p_connection);
	void finish();
	bool add_await_branch(const FlowScriptNodeOutputConnection &p_connection);
	bool execute_await_branches();
	bool is_node_reference_valid(const FlowScriptNodeReference &p_node_reference) const;
	void invoke_step();
	void set_variable(const uint8_t p_idx, const Variant &p_value);
	Variant get_variable(const uint8_t p_idx) const;
	bool has_variable(const uint8_t p_idx) const;
	void clear_variables();
	FlowScriptNodeID get_current_node_id() const;
	Ref<FlowScriptNode> get_current_node_ref() const;
	FlowScriptNode *get_current_node_ptr() const;
	Ref<FlowScript> get_current_flow_script_ref() const;
	FlowScript *get_current_flow_script_ptr() const;
	Ref<FlowScript> get_flow_script_ref() const;
	FlowScript *get_flow_script_ptr() const;
	Ref<FlowScriptBridge> get_bridge_ref() const;
	FlowScriptBridge *get_bridge_ptr() const;
	void set_state(const Dictionary &p_state);
	void get_state(Dictionary &r_state) const;

	FlowScriptNodeContext();
};


#endif // FLOW_SCRIPT_NODE_CONTEXT_HPP
