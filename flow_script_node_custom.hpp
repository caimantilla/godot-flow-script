#ifndef FLOW_SCRIPT_NODE_CUSTOM_HPP
#define FLOW_SCRIPT_NODE_CUSTOM_HPP


#include "flow_script_node.hpp"


class FlowScriptNodeCustom : public FlowScriptNode
{
	GDCLASS(FlowScriptNodeCustom, FlowScriptNode);

protected:
	static void _bind_methods();
	
	GDVIRTUAL1(_exec_startup, FlowScriptNodeContext *);
	GDVIRTUAL1(_exec_cleanup, FlowScriptNodeContext *);
	GDVIRTUAL1(_exec_step, FlowScriptNodeContext *);
	GDVIRTUAL0RC(bool, _can_instantiate_type);
	GDVIRTUAL0RC(String, _get_type_id);
	GDVIRTUAL0RC(String, _get_type_name);
	GDVIRTUAL0RC(String, _get_type_category);
	GDVIRTUAL0RC(String, _get_type_description);
	GDVIRTUAL0RC(String, _get_type_editor);
	GDVIRTUAL0RC(bool, _can_translate_text);
	GDVIRTUAL2(_init_text_translation, FlowScriptNodeID, Ref<FlowScriptNodeTranslation>);
	GDVIRTUAL2(_set_state, FlowScriptNodeContext *, const Dictionary &);
	GDVIRTUAL1RC(Dictionary, _get_state, const FlowScriptNodeContext *);
	GDVIRTUAL1(_set_json_data, const Dictionary &);
	GDVIRTUAL0RC(Dictionary, _get_json_data);
	GDVIRTUAL0RC(PackedInt64Array, _get_output_connection_list_lengths);

public:
	virtual void exec_startup(FlowScriptNodeContext *p_context) override;
	virtual void exec_cleanup(FlowScriptNodeContext *p_context) override;
	virtual void exec_step(FlowScriptNodeContext *p_context) override;
	virtual bool can_translate_text() const override;
	virtual void init_text_translation(const FlowScriptNodeID p_node_id, FlowScriptNodeTranslation *p_translation) override;
	virtual void set_state(FlowScriptNodeContext *p_context, const Dictionary &p_state) override;
	virtual void get_state(const FlowScriptNodeContext *p_context, Dictionary &r_state) const override;
	virtual void set_json_data(const Dictionary &p_data) override;
	virtual void get_json_data(Dictionary &r_data) const override;
	virtual void get_output_connection_list_lengths(List<int64_t> &r_lengths) const override;

	bool can_instantiate_type() const;
	String get_type_id() const;
	String get_type_name() const;
	String get_type_category() const;
	String get_type_description() const;
	String get_type_editor() const;
};


#endif // FLOW_SCRIPT_NODE_CUSTOM_HPP
