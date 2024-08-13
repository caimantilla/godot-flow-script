#include "flow_script.hpp"
#include "flow_script_node_custom.hpp"


void FlowScriptNodeCustom::_bind_methods()
{
	GDVIRTUAL_BIND(_exec_startup, "context");
	GDVIRTUAL_BIND(_exec_cleanup, "context");
	GDVIRTUAL_BIND(_exec_step, "context");
	GDVIRTUAL_BIND(_get_type_id);
	GDVIRTUAL_BIND(_get_type_name);
	GDVIRTUAL_BIND(_get_type_category);
	GDVIRTUAL_BIND(_get_type_description);
	GDVIRTUAL_BIND(_get_type_editor);
	GDVIRTUAL_BIND(_can_translate_text);
	GDVIRTUAL_BIND(_init_text_translation, "node_id", "translation");
	GDVIRTUAL_BIND(_set_state, "context", "state");
	GDVIRTUAL_BIND(_get_state, "context");
	GDVIRTUAL_BIND(_set_json_data, "data");
	GDVIRTUAL_BIND(_get_json_data);
	GDVIRTUAL_BIND(_get_output_connection_list_lengths);
}


void FlowScriptNodeCustom::exec_startup(FlowScriptNodeContext *p_context)
{
	GDVIRTUAL_CALL(_exec_startup, p_context);
}


void FlowScriptNodeCustom::exec_cleanup(FlowScriptNodeContext *p_context)
{
	GDVIRTUAL_CALL(_exec_cleanup, p_context);
}


void FlowScriptNodeCustom::exec_step(FlowScriptNodeContext *p_context)
{
	GDVIRTUAL_CALL(_exec_step, p_context);
}


String FlowScriptNodeCustom::get_type_id() const
{
	String ret;
	if (!GDVIRTUAL_CALL(_get_type_id, ret))
	{
		ERR_PRINT("_get_type_id must be overriden.");
	}
	return ret;
}


String FlowScriptNodeCustom::get_type_name() const
{
	String ret;
	if (!GDVIRTUAL_CALL(_get_type_name, ret))
	{
		ret = get_type_id();
	}
	return ret;
}


String FlowScriptNodeCustom::get_type_category() const
{
	String ret;
	GDVIRTUAL_CALL(_get_type_category, ret);
	return ret;
}


String FlowScriptNodeCustom::get_type_description() const
{
	String ret;
	GDVIRTUAL_CALL(_get_type_description, ret);
	return ret;
}


String FlowScriptNodeCustom::get_type_editor() const
{
	String ret;
	if (!GDVIRTUAL_CALL(_get_type_editor, ret))
	{
		ERR_PRINT("_get_type_editor must be overriden.");
	}
	return ret;
}


bool FlowScriptNodeCustom::can_translate_text() const
{
	bool ret = false;
	GDVIRTUAL_CALL(_can_translate_text, ret);
	return ret;
}


void FlowScriptNodeCustom::init_text_translation(const FlowScriptNodeID p_node_id, FlowScriptNodeTranslation *p_translation)
{
	Ref<FlowScriptNodeTranslation> translation_ref = Ref<FlowScriptNodeTranslation>(p_translation);
	if (!GDVIRTUAL_CALL(_init_text_translation, p_node_id, translation_ref))
	{
		ERR_PRINT("_init_text_translation must be overriden.");
	}
}


void FlowScriptNodeCustom::set_state(FlowScriptNodeContext *p_context, const Dictionary &p_state)
{
	GDVIRTUAL_CALL(_set_state, p_context, p_state);
}


void FlowScriptNodeCustom::get_state(const FlowScriptNodeContext *p_context, Dictionary &r_state) const
{
	GDVIRTUAL_CALL(_get_state, p_context, r_state);
}


void FlowScriptNodeCustom::set_json_data(const Dictionary &p_data)
{
	GDVIRTUAL_CALL(_set_json_data, p_data);
}


void FlowScriptNodeCustom::get_json_data(Dictionary &r_data) const
{
	GDVIRTUAL_CALL(_get_json_data, r_data);
}


void FlowScriptNodeCustom::get_output_connection_list_lengths(List<int64_t> &r_lengths) const
{
	PackedInt64Array virtual_lengths;
	GDVIRTUAL_CALL(_get_output_connection_list_lengths, virtual_lengths);
	for (const int64_t curr_length : virtual_lengths)
	{
		r_lengths.push_back(curr_length);
	}
}
