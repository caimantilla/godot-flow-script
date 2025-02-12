#ifndef FLOW_SCRIPT_BUILT_IN_NODE_INTERFACE_HPP
#define FLOW_SCRIPT_BUILT_IN_NODE_INTERFACE_HPP


#include "core/object/object.h"
#include "core/object/class_db.h"
#include "core/object/gdvirtual.gen.inc"
#include "flow_script_built_in_timer_proxy.hpp"


class FlowScriptBuiltInNodeInterface : public Object
{
	GDCLASS(FlowScriptBuiltInNodeInterface, Object);

	LocalVector<FlowScriptBuiltInTimerProxy *> timer_instance_list;
	bool timer_instance_list_clearing_queued = false;

protected:
	static void _bind_methods();

	void on_timer_finished();
	void clear_timer_instances();

	virtual FlowScriptBuiltInTimerProxy *_create_timer_proxy();

	GDVIRTUAL2(_set_local, const StringName &, const Variant &);
	GDVIRTUAL1RC(Variant, _get_local, const StringName &);
	GDVIRTUAL1RC(bool, _has_local, const StringName &);
	GDVIRTUAL2(_set_global, const StringName &, const Variant &);
	GDVIRTUAL1RC(Variant, _get_global, const StringName &);
	GDVIRTUAL1RC(bool, _has_global, const StringName &);
	GDVIRTUAL1R(Variant, _evaluate_expression, const String &);
	GDVIRTUAL1R(Array, _evaluate_multiline_expression, const String &);
	GDVIRTUAL2R(bool, _evaluate_boolean_expression, const String &, const bool &);
	GDVIRTUAL2R(bool, _evaluate_multiline_boolean_expression, const String &, const bool &);
	GDVIRTUAL0RC(FlowScriptBuiltInTimerProxy *, _create_timer_proxy);

public:
	FlowScriptBuiltInTimerProxy *create_timer_proxy();

	virtual void set_local(const StringName &p_key, const Variant &p_value);
	virtual Variant get_local(const StringName &p_key) const;
	virtual bool has_local(const StringName &p_key) const;
	virtual void set_global(const StringName &p_key, const Variant &p_value);
	virtual Variant get_global(const StringName &p_key) const;
	virtual bool has_global(const StringName &p_key) const;
	virtual Variant evaluate_expression(const String &p_expression);
	virtual Array evaluate_multiline_expression(const String &p_expression);
	virtual bool evaluate_boolean_expression(const String &p_expression, const bool p_succeed_if_expression_empty);
	virtual bool evaluate_multiline_boolean_expression(const String &p_expression, const bool p_succeed_if_expression_empty);
};


#endif // FLOW_SCRIPT_BUILT_IN_NODE_INTERFACE_HPP
