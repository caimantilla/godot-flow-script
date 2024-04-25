#include "flow_type_db.hpp"
#include "flow_config_manager.hpp"
#include "core/io/dir_access.h"


FlowTypeDB *FlowTypeDB::singleton = nullptr;


FlowTypeDB *FlowTypeDB::get_singleton()
{
	return singleton;
}


bool FlowTypeDB::has_type(const String &p_type_id) const
{
	return type_map.has(p_type_id);
}


Ref<FlowType> FlowTypeDB::get_type(const String &p_type_id)
{
	Ref<FlowType> type = type_map[p_type_id];
	return type;
}


PackedStringArray FlowTypeDB::get_type_id_list() const
{
	PackedStringArray type_id_list;
	type_id_list.resize(type_map.size());

	int i = 0;
	for (const KeyValue<String, Ref<FlowType>> &map_entry : type_map)
	{
		type_id_list.set(i, map_entry.key);
		i += 1;
	}

	return type_id_list;
}


PackedStringArray FlowTypeDB::get_non_native_type_id_list() const
{
	PackedStringArray non_native_type_id_list;

	for (const KeyValue<String, Ref<FlowType>> &map_entry : type_map)
	{
		if (map_entry.value->is_native())
		{
			continue;
		}

		non_native_type_id_list.push_back(map_entry.key);
	}

	return non_native_type_id_list;
}


TypedArray<FlowType> FlowTypeDB::get_type_list() const
{
	TypedArray<FlowType> type_list;
	type_list.resize(type_map.size());

	int i = 0;
	for (KeyValue<String, Ref<FlowType>> map_entry : type_map)
	{
		type_list.set(i, &map_entry.value);
		i += 1;
	}

	return type_list;
}


void FlowTypeDB::refresh()
{
	non_native_type_list.clear();

	PackedStringArray non_native_type_ids = get_non_native_type_id_list();
	for (const String &type_id : non_native_type_ids)
	{
		type_map.erase(type_id);
	}

	for (Ref<FlowType> native_type : native_type_list)
	{
		add_type_to_type_map(native_type);
	}

	PackedStringArray non_native_type_directories = FlowConfigManager::get_singleton()->get_flow_type_directories();
	if (non_native_type_directories.is_empty())
	{
		return;
	}

	Ref<DirAccess> dir_access = DirAccess::create(DirAccess::ACCESS_RESOURCES);
	dir_access->set_include_hidden(false);
	dir_access->set_include_navigational(false);

	for (const String &type_directory : non_native_type_directories)
	{
		if (!type_directory.begins_with("res://") || !dir_access->dir_exists(type_directory) || dir_access->change_dir(type_directory) != OK)
		{
			continue;
		}

		PackedStringArray dir_file_list = dir_access->get_files();
		for (const String &file_name : dir_file_list)
		{
			String file_path = type_directory.path_join(file_name);
			if (!ResourceLoader::exists(file_path, "Resource"))
			{
				continue;
			}

			Ref<Resource> res = ResourceLoader::load(file_path, "Resource");
			Ref<FlowType> type = Ref<FlowType>(Object::cast_to<FlowType>(*res));

			if (type == nullptr || !type.is_valid())
			{
				continue;
			}

			add_type_to_type_map(type);
		}
	}
}


bool FlowTypeDB::register_native_type(const Ref<FlowType> p_type)
{
	ERR_FAIL_COND_V(!p_type.is_valid(), false);
	ERR_FAIL_COND_V(!p_type->is_native(), false);

	native_type_list.push_back(p_type);
	return true;
}


bool FlowTypeDB::unregister_native_type(const Ref<FlowType> p_type)
{
	ERR_FAIL_COND_V(!p_type.is_valid(), false);
	return native_type_list.erase(p_type);
}


bool FlowTypeDB::unregister_native_type_using_id(const String &p_type_id)
{
	for (Ref<FlowType> native_type : native_type_list)
	{
		if (native_type->get_id() == p_type_id && unregister_native_type(native_type))
		{
			return true;
		}
	}

	return false;
}


bool FlowTypeDB::add_type_to_type_map(Ref<FlowType> p_type)
{
	if (p_type == nullptr || !p_type.is_valid())
	{
		return false;
	}

	String type_id = p_type->get_id();
	
	if (type_id.is_empty())
	{
		return false;
	}


	if (type_map.has(type_id))
	{
		type_map.erase(type_id);
	}

	type_map.insert(type_id, p_type);
	return true;
}


FlowTypeDB::FlowTypeDB()
{
	CRASH_COND_MSG(singleton != nullptr, "FlowTypeDB is a singleton and may not be instantiated multiple times.");
	singleton = this;
}


FlowTypeDB::~FlowTypeDB()
{
	singleton = nullptr;
}
