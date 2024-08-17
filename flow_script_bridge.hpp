#ifndef FLOW_SCRIPT_BRIDGE_HPP
#define FLOW_SCRIPT_BRIDGE_HPP


#include "core/object/ref_counted.h"
#include "core/object/gdvirtual.gen.inc"
#include "typedefs.hpp"
#include "flow_script_node_context.hpp"
#include "flow_script_timer_proxy.hpp"


class FlowScript;


class FlowScriptBridge : public RefCounted
{
	GDCLASS(FlowScriptBridge, RefCounted);

public:
	enum : FlowScriptExecutionFiberID
	{
		FIBER_ID_INVALID = -1,
		FIBERS_MAX = 31,
	};

private:
	static String ERR_STR_NO_AVAILABLE_FIBERS;

	Ref<FlowScript> flow_script;
	FlowScriptNodeContext fiber_list[FIBERS_MAX];
	FlowScriptExecutionFiberID cache_next_free_fiber_id = 0;

	void update_cache_next_free_fiber_id();

protected:
	static void _bind_methods();

	GDVIRTUAL2(_set_local, const String &, const Variant &);
	GDVIRTUAL1RC(Variant, _get_local, const String &);
	GDVIRTUAL1RC(bool, _has_local, const String &);
	GDVIRTUAL2(_set_global, const String &, const Variant &);
	GDVIRTUAL1RC(Variant, _get_global, const String &);
	GDVIRTUAL1RC(bool, _has_global, const String &);
	GDVIRTUAL1R(Variant, _evaluate_expression, const String &);
	GDVIRTUAL1R(Array, _evaluate_multiline_expression, const String &);
	GDVIRTUAL2R(bool, _evaluate_boolean_expression, const String &, const bool &);
	GDVIRTUAL2R(bool, _evaluate_multiline_boolean_expression, const String &, const bool &);
	GDVIRTUAL0RC(FlowScriptTimerProxy *, _create_timer_proxy);

public:
	virtual void set_local(const String &p_key, const Variant &p_value);
	virtual Variant get_local(const String &p_key) const;
	virtual bool has_local(const String &p_key) const;
	virtual void set_global(const String &p_key, const Variant &p_value);
	virtual Variant get_global(const String &p_key) const;
	virtual bool has_global(const String &p_key) const;
	virtual Variant evaluate_expression(const String &p_expression);
	virtual Array evaluate_multiline_expression(const String &p_expression);
	virtual bool evaluate_boolean_expression(const String &p_expression, const bool p_succeed_if_expression_empty);
	virtual bool evaluate_multiline_boolean_expression(const String &p_expression, const bool p_succeed_if_expression_empty);
	virtual FlowScriptTimerProxy *create_timer_proxy() const;

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

	FlowScriptBridge();
	~FlowScriptBridge();
};


#endif // FLOW_SCRIPT_BRIDGE_HPP
