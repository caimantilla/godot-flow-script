#if 0

#include "flow_script_node_translation.hpp"


void FlowScriptNodeTranslation::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_entry_count", "count"), &FlowScriptNodeTranslation::set_entry_count);
	ClassDB::bind_method(D_METHOD("get_entry_count"), &FlowScriptNodeTranslation::get_entry_count);
	ClassDB::bind_method(D_METHOD("set_entry_id", "entry", "id"), &FlowScriptNodeTranslation::set_entry_id);
	ClassDB::bind_method(D_METHOD("get_entry_id", "entry"), &FlowScriptNodeTranslation::get_entry_id);
	ClassDB::bind_method(D_METHOD("set_entry_text_original", "entry", "text"), &FlowScriptNodeTranslation::set_entry_text_original);
	ClassDB::bind_method(D_METHOD("get_entry_text_original", "entry"), &FlowScriptNodeTranslation::get_entry_text_original);
	ClassDB::bind_method(D_METHOD("set_entry_text_translated", "entry", "text"), &FlowScriptNodeTranslation::set_entry_text_translated);
	ClassDB::bind_method(D_METHOD("get_entry_text_translated", "entry"), &FlowScriptNodeTranslation::get_entry_text_translated);

	Entry defaults;

	base_property_helper.set_prefix("entries/");
	base_property_helper.set_array_length_getter(&FlowScriptNodeTranslation::get_entry_count);
	base_property_helper.register_property(PropertyInfo(Variant::STRING, "id"), defaults.id, &FlowScriptNodeTranslation::set_entry_id, &FlowScriptNodeTranslation::get_entry_id);
	base_property_helper.register_property(PropertyInfo(Variant::STRING, "text_original"), defaults.text_original, &FlowScriptNodeTranslation::set_entry_text_original, &FlowScriptNodeTranslation::get_entry_text_original);
	base_property_helper.register_property(PropertyInfo(Variant::STRING, "text_translated"), defaults.text_translated, &FlowScriptNodeTranslation::set_entry_text_translated, &FlowScriptNodeTranslation::get_entry_text_translated);
	PropertyListHelper::register_base_helper(&base_property_helper);
}


void FlowScriptNodeTranslation::_get_property_list(List<PropertyInfo> *p_list) const
{
	property_helper.get_property_list(p_list);
}


bool FlowScriptNodeTranslation::_set(const StringName &p_name, const Variant &p_value)
{
	return property_helper.property_set_value(p_name, p_value);
}


bool FlowScriptNodeTranslation::_get(const StringName &p_name, Variant &r_ret) const
{
	return property_helper.property_get_value(p_name, r_ret);
}


bool FlowScriptNodeTranslation::_property_can_revert(const StringName &p_name) const
{
	return property_helper.property_can_revert(p_name);
}


bool FlowScriptNodeTranslation::_property_get_revert(const StringName &p_name, Variant &r_property) const
{
	return property_helper.property_get_revert(p_name, r_property);
}


void FlowScriptNodeTranslation::set_entry_count(const int64_t p_count)
{
	entries.resize(p_count);
	notify_property_list_changed();
	emit_changed();
}


int64_t FlowScriptNodeTranslation::get_entry_count() const
{
	return entries.size();
}


void FlowScriptNodeTranslation::set_entry_id(const int64_t p_idx, const String &p_id)
{
	ERR_FAIL_INDEX(p_idx, entries.size());
	if (p_id == entries[p_idx].id)
	{
		return;
	}
	entries[p_idx].id = p_id;
	emit_changed();
}


String FlowScriptNodeTranslation::get_entry_id(const int64_t p_idx) const
{
	ERR_FAIL_INDEX_V(p_idx, entries.size(), String());
	return entries[p_idx].id;
}


void FlowScriptNodeTranslation::set_entry_text_original(const int64_t p_idx, const String &p_text)
{
	ERR_FAIL_INDEX(p_idx, entries.size());
	if (p_text == entries[p_idx].text_original)
	{
		return;
	}
	entries[p_idx].text_original = p_text;
	emit_changed();
}


String FlowScriptNodeTranslation::get_entry_text_original(const int64_t p_idx) const
{
	ERR_FAIL_INDEX_V(p_idx, entries.size(), String());
	return entries[p_idx].text_original;
}


void FlowScriptNodeTranslation::set_entry_text_translated(const int64_t p_idx, const String &p_text)
{
	ERR_FAIL_INDEX(p_idx, entries.size());
	if (p_text == entries[p_idx].text_translated)
	{
		return;
	}
	entries[p_idx].text_translated = p_text;
	emit_changed();
}


String FlowScriptNodeTranslation::get_entry_text_translated(const int64_t p_idx) const
{
	ERR_FAIL_INDEX_V(p_idx, entries.size(), String());
	return entries[p_idx].text_translated;
}


FlowScriptNodeTranslation::FlowScriptNodeTranslation()
{
	property_helper.setup_for_instance(base_property_helper, this);
}

#endif // 0
