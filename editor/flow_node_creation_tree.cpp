#include "flow_node_creation_tree.hpp"
#include "../singletons/flow_type_db.hpp"


static String CATEGORY_SEPARATOR = String("/");
static const int MAIN_COLUMN = 0;


void FlowNodeCreationTree::_bind_methods()
{
	ADD_SIGNAL(MethodInfo("flow_type_selected", PropertyInfo(Variant::STRING, "flow_type_id")));
	ADD_SIGNAL(MethodInfo("flow_type_chosen", PropertyInfo(Variant::STRING, "flow_type_id")));
}


void FlowNodeCreationTree::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_POSTINITIALIZE:
			init_signals();
			break;
		case NOTIFICATION_VISIBILITY_CHANGED:
		case NOTIFICATION_READY:
			reload_types();
			break;
	}
}


String FlowNodeCreationTree::get_selected_flow_node_type_id() const
{
	TreeItem *selected_item = get_selected();
	Variant item_meta = selected_item->get_metadata(MAIN_COLUMN);

	if (item_meta.get_type() == Variant::STRING)
	{
		return String(item_meta);
	}
	else
	{
		return "";
	}
}


void FlowNodeCreationTree::create_category_if_necessary(const String &p_category_path)
{
	String category_path = p_category_path.strip_edges();
	if (category_path.is_empty())
	{
		category_item_map.insert(category_path, get_root());
		return;
	}

	PackedStringArray categories = category_path.split(CATEGORY_SEPARATOR);

	for (int current_category_depth = 0; current_category_depth < categories.size(); current_category_depth++)
	{

		String current_category_name_part = categories.get(current_category_depth);
		String current_category_name = CATEGORY_SEPARATOR.join(categories.slice(0, current_category_depth + 1));

		if (category_item_map.has(current_category_name))
		{
			continue;
		}

		TreeItem *parent_item = nullptr;

		for (int previous_category_depth = current_category_depth - 1; previous_category_depth > -1; previous_category_depth -= 1)
		{
			String previous_category_name = CATEGORY_SEPARATOR.join(categories.slice(0, previous_category_depth + 1));
			if (category_item_map.has(previous_category_name))
			{
				parent_item = category_item_map.get(previous_category_name);
				break;
			}
		}

		if (parent_item == nullptr)
		{
			parent_item = get_root();
		}

		TreeItem *current_category_item = parent_item->create_child();
		current_category_item->set_text(MAIN_COLUMN, current_category_name_part);
		current_category_item->set_selectable(MAIN_COLUMN, false);

		category_item_map.insert(current_category_name, current_category_item);
	}
}


void FlowNodeCreationTree::clear_types()
{
	category_item_map.clear();
	type_item_map.clear();
	clear();
}


void FlowNodeCreationTree::reload_types()
{
	if (!is_ready())
	{
		return;
	}

	clear_types();
	if (!is_visible_in_tree())
	{
		return;
	}

	TreeItem *root_item = create_item();
	root_item->set_text(MAIN_COLUMN, "FlowNodes");
	root_item->set_selectable(MAIN_COLUMN, false);

	PackedStringArray type_id_list = FlowTypeDB::get_singleton()->get_type_id_list();

	for (const String &type_id : type_id_list)
	{
		Ref<FlowType> type = FlowTypeDB::get_singleton()->get_type(type_id);
		String str_id = type->get_id();
		String str_name = type->get_name();
		String str_category = type->get_category();
		String str_description = type->get_description();
		Ref<Texture2D> tex_icon = type->get_icon_texture();

		create_category_if_necessary(str_category);

		TreeItem *parent_item;
		if (category_item_map.has(str_category))
		{
			parent_item = category_item_map.get(str_category);
		}
		else
		{
			parent_item = root_item;
		}

		TreeItem *type_item = parent_item->create_child();
		type_item_map.insert(str_id, type_item);

		type_item->set_text(MAIN_COLUMN, str_name);
		type_item->set_tooltip_text(MAIN_COLUMN, str_description);
		type_item->set_metadata(MAIN_COLUMN, type_id);
		if (tex_icon != nullptr)
		{
			type_item->set_icon(MAIN_COLUMN, tex_icon);
		}
	}
}


void FlowNodeCreationTree::on_item_selected()
{
	String selected_type = get_selected_flow_node_type_id();
	emit_signal(SNAME("flow_type_selected"), selected_type);
}


void FlowNodeCreationTree::on_item_activated()
{
	String selected_type = get_selected_flow_node_type_id();
	if (selected_type.is_empty())
	{
		return;
	}

	emit_signal(SNAME("flow_type_chosen"), selected_type);
}


void FlowNodeCreationTree::init_signals()
{
	connect(SNAME("item_selected"), callable_mp(this, &FlowNodeCreationTree::on_item_selected));
	connect(SNAME("item_activated"), callable_mp(this, &FlowNodeCreationTree::on_item_activated));
}


FlowNodeCreationTree::FlowNodeCreationTree()
{
	set_hide_root(true);
	set_allow_reselect(false);
}
