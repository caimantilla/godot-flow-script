#ifndef FLOW_TYPE_DB_HPP
#define FLOW_TYPE_DB_HPP

#include "core/object/object.h"
#include "../core_types/flow_type.hpp"

class FlowTypeDB final : public Object
{
	GDCLASS(FlowTypeDB, Object);

private:
	static FlowTypeDB *singleton;

	List<Ref<FlowType>> native_type_list;
	List<Ref<FlowType>> non_native_type_list;

	HashMap<String, Ref<FlowType>> type_map;

	bool add_type_to_type_map(Ref<FlowType> p_type);

public:
	static FlowTypeDB *get_singleton();

	bool has_type(const String &p_type_id) const;
	Ref<FlowType> get_type(const String &p_type_id);
	PackedStringArray get_type_id_list() const;
	PackedStringArray get_non_native_type_id_list() const;
	TypedArray<FlowType> get_type_list() const;

	void refresh();

	bool register_native_type(const Ref<FlowType> p_type);
	bool unregister_native_type(const Ref<FlowType> p_type);
	bool unregister_native_type_using_id(const String &p_type_id);

	FlowTypeDB();
	~FlowTypeDB();
};

#endif
