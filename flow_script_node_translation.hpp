#ifndef FLOW_SCRIPT_NODE_TRANSLATION_HPP
#define FLOW_SCRIPT_NODE_TRANSLATION_HPP


#include "core/io/resource.h"
#include "scene/property_list_helper.h"


class FlowScriptNodeTranslation final : public Resource
{
	GDCLASS(FlowScriptNodeTranslation, Resource);

private:
	struct Entry
	{
		String id;
		String text_original;
		String text_translated;
	};

	Vector<Entry> entries;

	static inline PropertyListHelper base_property_helper;
	PropertyListHelper property_helper;

protected:
	static void _bind_methods();
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	bool _property_can_revert(const StringName &p_name) const;
	bool _property_get_revert(const StringName &p_name, Variant &r_property) const;

public:
	void set_entry_count(const int64_t p_count);
	int64_t get_entry_count() const;
	void set_entry_id(const int64_t p_idx, const String &p_id);
	String get_entry_id(const int64_t p_idx) const;
	void set_entry_text_original(const int64_t p_idx, const String &p_text);
	String get_entry_text_original(const int64_t p_idx) const;
	void set_entry_text_translated(const int64_t p_idx, const String &p_text);
	String get_entry_text_translated(const int64_t p_idx) const;

	FlowScriptNodeTranslation();
};


#endif // FLOW_SCRIPT_NODE_TRANSLATION_HPP
