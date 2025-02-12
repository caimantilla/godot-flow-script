#ifndef FLOW_SCRIPT_NODE_CUSTOM_HPP
#define FLOW_SCRIPT_NODE_CUSTOM_HPP


#include "flow_script_node.hpp"


class FlowScriptNodeCustom final : public FlowScriptNode
{
	GDCLASS(FlowScriptNodeCustom, FlowScriptNode);

protected:
	static void _bind_methods();
	
	GDVIRTUAL1(_exec_startup, FlowScriptNodeContext *);
	GDVIRTUAL1(_exec_cleanup, FlowScriptNodeContext *);
	GDVIRTUAL1(_exec_step, FlowScriptNodeContext *);
	GDVIRTUAL2(_set_runtime_state, FlowScriptNodeContext *, const Dictionary &);
	GDVIRTUAL1RC(Dictionary, _get_runtime_state, const FlowScriptNodeContext *);
	GDVIRTUAL1(_set_data_state, const Dictionary &);
	GDVIRTUAL0RC(Dictionary, _get_data_state);
	GDVIRTUAL0RC(PackedInt32Array, _get_output_connection_list_lengths);
#ifdef TOOLS_ENABLED
	GDVIRTUAL0RC(bool, _can_instantiate_type);
	GDVIRTUAL0RC(bool, _can_edit_name);
	GDVIRTUAL0RC(bool, _can_edit_size);
	GDVIRTUAL0RC(String, _get_type_name);
	GDVIRTUAL0RC(String, _get_type_category);
	GDVIRTUAL0RC(String, _get_type_description);
	GDVIRTUAL0RC(String, _get_type_editor);
#endif // TOOLS_ENABLED

public:
	virtual void exec_startup(FlowScriptNodeContext *p_context) override;
	virtual void exec_cleanup(FlowScriptNodeContext *p_context) override;
	virtual void exec_step(FlowScriptNodeContext *p_context) override;

	virtual void set_runtime_state(FlowScriptNodeContext *p_context, const Dictionary &p_state) override;
	virtual Dictionary get_runtime_state(const FlowScriptNodeContext *p_context) const override;
	virtual void set_data_state(const Dictionary &p_state) override;
	virtual Dictionary get_data_state() const override;
	virtual void get_output_connection_list_lengths(List<FlowScriptNodeConnectionListLength> *p_lengths) const override;

#ifdef TOOLS_ENABLED
	bool can_instantiate_type() const;
	bool can_edit_name() const;
	bool can_edit_size() const;
	String get_type_name() const;
	String get_type_category() const;
	String get_type_description() const;
	String get_type_editor() const;
#endif // TOOLS_ENABLED
};


#endif // FLOW_SCRIPT_NODE_CUSTOM_HPP
