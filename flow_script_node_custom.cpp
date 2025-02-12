#include "flow_script_node_custom.hpp"
#include "core/object/script_language.h"


void FlowScriptNodeCustom::_bind_methods()
{
	GDVIRTUAL_BIND(_exec_startup, "context");
	GDVIRTUAL_BIND(_exec_cleanup, "context");
	GDVIRTUAL_BIND(_exec_step, "context");
	GDVIRTUAL_BIND(_set_runtime_state, "context", "state");
	GDVIRTUAL_BIND(_get_runtime_state, "context");
	GDVIRTUAL_BIND(_set_data_state, "data");
	GDVIRTUAL_BIND(_get_data_state);
	GDVIRTUAL_BIND(_get_output_connection_list_lengths);
#ifdef TOOLS_ENABLED
	GDVIRTUAL_BIND(_can_instantiate_type);
	GDVIRTUAL_BIND(_can_edit_name);
	GDVIRTUAL_BIND(_can_edit_size);
	GDVIRTUAL_BIND(_get_type_name);
	GDVIRTUAL_BIND(_get_type_category);
	GDVIRTUAL_BIND(_get_type_description);
	GDVIRTUAL_BIND(_get_type_editor);
#endif // TOOLS_ENABLED
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


void FlowScriptNodeCustom::set_runtime_state(FlowScriptNodeContext *p_context, const Dictionary &p_state)
{
	GDVIRTUAL_CALL(_set_runtime_state, p_context, p_state);
}


Dictionary FlowScriptNodeCustom::get_runtime_state(const FlowScriptNodeContext *p_context) const
{
	Dictionary d;
	GDVIRTUAL_CALL(_get_runtime_state, p_context, d);
	return d;
}


void FlowScriptNodeCustom::set_data_state(const Dictionary &p_state)
{
	GDVIRTUAL_CALL(_get_data_state, p_state);
}


Dictionary FlowScriptNodeCustom::get_data_state() const
{
	Dictionary d;
	GDVIRTUAL_CALL(_get_data_state, d);
	return d;
}


void FlowScriptNodeCustom::get_output_connection_list_lengths(List<FlowScriptNodeConnectionListLength> *p_lengths) const
{
	PackedInt32Array virtual_lengths;
	GDVIRTUAL_CALL(_get_output_connection_list_lengths, virtual_lengths);
	for (const int32_t curr_length : virtual_lengths)
	{
		ERR_FAIL_INDEX(curr_length, FlowScriptConstants::NODE_CONNECTION_LIST_LENGTH_MAX);
		p_lengths->push_back(curr_length);
	}
}


#ifdef TOOLS_ENABLED

bool FlowScriptNodeCustom::can_instantiate_type() const
{
	bool ret = false;
	GDVIRTUAL_CALL(_can_instantiate_type, ret);
	return ret;
}


bool FlowScriptNodeCustom::can_edit_name() const
{
	bool ret = false;
	GDVIRTUAL_CALL(_can_edit_name, ret);
	return ret;
}


bool FlowScriptNodeCustom::can_edit_size() const
{
	bool ret = false;
	GDVIRTUAL_CALL(_can_edit_size, ret);
	return ret;
}


String FlowScriptNodeCustom::get_type_name() const
{
	String ret;
	GDVIRTUAL_CALL(_get_type_name, ret);
	if (ret.is_empty())
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

#endif // TOOLS_ENABLED
