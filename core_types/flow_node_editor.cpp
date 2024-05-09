#include "flow_node_editor.hpp"
#include "scene/scene_string_names.h"
#include "editor/editor_string_names.h"
#include "scene/gui/box_container.h"
#include "core/variant/variant_utility.h"


void FlowNodeEditor::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_flow_node"), &FlowNodeEditor::get_flow_node);
	ClassDB::bind_method(D_METHOD("get_flow_node_id"), &FlowNodeEditor::get_flow_node_id);
	ClassDB::bind_method(D_METHOD("get_editor_scale"), &FlowNodeEditor::get_editor_scale);
	ClassDB::bind_method(D_METHOD("is_safe_to_edit"), &FlowNodeEditor::is_safe_to_edit);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "flow_node", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE, SNAME("FlowNode")), "", "get_flow_node");
	ADD_PROPERTY(PropertyInfo(TYPE_FLOW_NODE_ID, "flow_node_id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "", "get_flow_node_id");

	ADD_SIGNAL(MethodInfo("rename_dialog_requested"));
	ADD_SIGNAL(MethodInfo("graph_position_update_requested"));

	GDVIRTUAL_BIND(_initialize);
	GDVIRTUAL_BIND(_clean_up);
	GDVIRTUAL_BIND(_flow_node_updated);
	GDVIRTUAL_BIND(_flow_node_updated_immediate);
	GDVIRTUAL_BIND(_update_style);
	GDVIRTUAL_BIND(_get_editor_title);
	GDVIRTUAL_BIND(_get_editor_tooltip_text);

	GDVIRTUAL_BIND(_set_out_going_connection, "out_going_connection");
	GDVIRTUAL_BIND(_get_input_slot);
	GDVIRTUAL_BIND(_get_out_going_connections);
}


void FlowNodeEditor::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_POSTINITIALIZE:
			_editor_init();
			break;

		case NOTIFICATION_THEME_CHANGED:
			update_style();
			break;
		
		case NOTIFICATION_PROCESS:
			if (flow_node_update_queued)
			{
				flow_node_updated();
				flow_node_update_queued = false;
			}
			break;
		
		case NOTIFICATION_DRAW:
			Vector2 curr_size = get_size();
			Vector2 min_size = get_minimum_size();
			if (curr_size != min_size)
			{
				set_size(min_size);
			}
			break;
	}
}


void FlowNodeEditor::set_safe_to_edit(bool p_edit_safe)
{
	edit_safe = p_edit_safe;
}


bool FlowNodeEditor::is_safe_to_edit() const
{
	return edit_safe;
}


void FlowNodeEditor::request_rename()
{
	emit_signal(SNAME("rename_dialog_requested"));
}


void FlowNodeEditor::request_deletion()
{
	emit_signal(SNAME("delete_request"));
}


void FlowNodeEditor::request_graph_position_update()
{
	emit_signal(SNAME("graph_position_update_requested"));
}


bool FlowNodeEditor::is_flow_node_instance_valid() const
{
	return VariantUtilityFunctions::is_instance_valid(flow_node);
}


bool FlowNodeEditor::is_flow_node_instance_invalid() const
{
	return !is_flow_node_instance_valid();
}


int FlowNodeEditor::input_get_slot_from_port(int p_port)
{
	int slot = get_input_port_slot(p_port);
	return slot;
}


int FlowNodeEditor::output_get_slot_from_port(int p_port)
{
	int slot = get_output_port_slot(p_port);
	return slot;
}


int FlowNodeEditor::input_get_port_from_slot(int p_slot) const
{
	int port = p_slot;

	for (int i = 0; i < p_slot; i++)
	{
		if (!is_slot_enabled_left(i))
		{
			port--;
		}
	}

	return port;
}


int FlowNodeEditor::output_get_port_from_slot(int p_slot) const
{
	int port = p_slot;

	for (int i = 0; i < p_slot; i++)
	{
		if (!is_slot_enabled_right(i))
		{
			port--;
		}
	}

	return port;
}


FlowNode *FlowNodeEditor::get_flow_node() const
{
	return flow_node;
}


FlowNodeID FlowNodeEditor::get_flow_node_id() const
{
	return flow_node->get_flow_node_id();
}


String FlowNodeEditor::get_flow_node_name() const
{
	return flow_node->get_flow_node_name();
}


String FlowNodeEditor::get_flow_type_id() const
{
	return flow_node->get_flow_type_id();
}


String FlowNodeEditor::get_flow_type_name() const
{
	return flow_node->get_flow_type_name();
}


bool FlowNodeEditor::is_flow_node_nameable() const
{
	return flow_node->is_flow_node_nameable();
}


float FlowNodeEditor::get_editor_scale() const
{
	return EDSCALE;
}


int FlowNodeEditor::get_input_slot()
{
	int slot;
	
	if (!GDVIRTUAL_CALL(_get_input_slot, slot))
	{
		slot = _get_input_slot();
	}

	return slot;
}


int FlowNodeEditor::get_input_port()
{
	return input_get_port_from_slot(get_input_slot());
}


void FlowNodeEditor::set_flow_node(FlowNode *p_flow_node)
{
	ERR_FAIL_COND_MSG(!is_inside_tree(), "FlowNodeEditor cannot have its FlowNode assigned unless it's inside the tree.");
	ERR_FAIL_COND_MSG(!is_ready(), "FlowNodeEditor cannot have a node assigned before it's ready.");

	if (flow_node != nullptr)
	{
		clean_up();
	}

	flow_node = p_flow_node;

	if (flow_node != nullptr)
	{
		initialize();
	}
}


void FlowNodeEditor::initialize()
{
	ERR_FAIL_COND(is_flow_node_instance_invalid());
	ERR_FAIL_COND_MSG(has_initialized, "Cannot re-initialize FlowNodeEditor without cleaning it up first.");
	ERR_FAIL_NULL_MSG(flow_node, "Cannot initialize FlowNodeEditor without a FlowNode.");

	flow_node->connect(SNAME("flow_graph_position_changed"), callable_mp(this, &FlowNodeEditor::request_graph_position_update));
	flow_node->connect(SNAME("changed"), callable_mp(this, &FlowNodeEditor::flow_node_updated_immediate));
	flow_node->connect(SNAME("changed"), callable_mp(this, &FlowNodeEditor::queue_flow_node_update));
	flow_node->connect(SNAME("name_changed"), callable_mp(this, &FlowNodeEditor::on_flow_node_name_changed));

	if (flow_node->is_flow_node_nameable())
	{
		// just change name in the inspector dude
		// btn_rename->show();
		btn_rename->hide();
	}
	else
	{
		btn_rename->hide();
	}

	set_safe_to_edit(true);
	_initialize();
	GDVIRTUAL_CALL(_initialize);
	update_style();

	set_safe_to_edit(false);

	has_initialized = true;
	flow_node_updated();
	set_safe_to_edit(true);
}


void FlowNodeEditor::clean_up()
{
	ERR_FAIL_COND(is_flow_node_instance_invalid());
	ERR_FAIL_COND_MSG(!has_initialized, "Cannot clean up FlowNodeEditor without initializing it first.");
	ERR_FAIL_NULL_MSG(flow_node, "Cannot clean up FlowNodeEditor without a FlowNode.");

	flow_node->disconnect(SNAME("flow_graph_position_changed"), callable_mp(this, &FlowNodeEditor::request_graph_position_update));
	flow_node->disconnect(SNAME("changed"), callable_mp(this, &FlowNodeEditor::flow_node_updated_immediate));
	flow_node->disconnect(SNAME("changed"), callable_mp(this, &FlowNodeEditor::queue_flow_node_update));
	flow_node->disconnect(SNAME("name_changed"), callable_mp(this, &FlowNodeEditor::on_flow_node_name_changed));

	_clean_up();
	GDVIRTUAL_CALL(_clean_up);

	has_initialized = false;
}


void FlowNodeEditor::flow_node_updated()
{
	ERR_FAIL_COND(is_flow_node_instance_invalid());
	ERR_FAIL_COND(!has_initialized);

	set_safe_to_edit(false);

	update_editor_title();
	update_editor_tooltip_text();

	_flow_node_updated();
	GDVIRTUAL_CALL(_flow_node_updated);

	set_safe_to_edit(true);
}


void FlowNodeEditor::flow_node_updated_immediate()
{
	ERR_FAIL_COND(is_flow_node_instance_invalid());
	ERR_FAIL_COND(!has_initialized);

	set_safe_to_edit(false);

	_flow_node_updated_immediate();
	GDVIRTUAL_CALL(_flow_node_updated_immediate);

	set_safe_to_edit(true);
}


void FlowNodeEditor::queue_flow_node_update()
{
	flow_node_update_queued = true;
}


void FlowNodeEditor::update_style()
{
	bool was_edit_safe = is_safe_to_edit();
	if (!was_edit_safe)
	{
		return;
	}

	set_safe_to_edit(false);

	Ref<Texture2D> icn_rename = get_theme_icon(SNAME("EditInternal"), EditorStringNames::get_singleton()->EditorIcons);
	Ref<Texture2D> icn_close = get_theme_icon(SNAME("Close"), EditorStringNames::get_singleton()->EditorIcons);

	btn_rename->set_icon(icn_rename);
	btn_close->set_icon(icn_close);

	_update_style();
	GDVIRTUAL_CALL(_update_style);

	set_safe_to_edit(was_edit_safe);
}


void FlowNodeEditor::update_editor_title()
{
	String new_title = "";

	if (flow_node != nullptr)
	{
		if (!GDVIRTUAL_CALL(_get_editor_title, new_title))
		{
			new_title = _get_editor_title();
		}
	}

	set_title(new_title);
}


void FlowNodeEditor::update_editor_tooltip_text()
{
	String new_tooltip_text = "";

	if (flow_node != nullptr)
	{
		if (!GDVIRTUAL_CALL(_get_editor_tooltip_text, new_tooltip_text))
		{
			new_tooltip_text = _get_editor_tooltip_text();
		}
	}

	set_tooltip_text(new_tooltip_text);
}


void FlowNodeEditor::silent_copy_graph_position_to_flow_node()
{
	ERR_FAIL_COND(is_flow_node_instance_invalid());

	Vector2 new_pos = get_position_offset();
	new_pos /= get_editor_scale();
	new_pos = new_pos.floor();

	flow_node->set_flow_graph_position_no_signal(new_pos);
}


void FlowNodeEditor::on_flow_node_name_changed(const String &p_from, const String &p_to)
{
	queue_flow_node_update();
}


void FlowNodeEditor::readjust_graph_size()
{
	set_size(get_minimum_size());
}


void FlowNodeEditor::set_out_going_connection(Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection)
{
	// THIS IS VERY VERY IMPORTANT THIS IS WHERE PORT IS CONVERTED TO SLOT PLEASE REMEMBER THIS AND DO NOT REMOVE THIS !!!!!!!!!!
	p_out_going_connection->set_origin_slot(output_get_slot_from_port(p_out_going_connection->get_origin_port()));

	// Don't allow connections to self.
	if (p_out_going_connection->get_target_flow_node_id() == get_flow_node_id())
	{
		p_out_going_connection->set_target_flow_node_id(FLOW_NODE_ID_NIL);
	}

	_set_out_going_connection(p_out_going_connection);

	Dictionary dict_connection;
	dict_connection["target_flow_node_id"] = p_out_going_connection->get_target_flow_node_id();
	dict_connection["origin_slot"] = p_out_going_connection->get_origin_slot();
	dict_connection["origin_port"] = p_out_going_connection->get_origin_port();

	GDVIRTUAL_CALL(_set_out_going_connection, dict_connection);
}


Ref<FlowNodeEditorOutGoingConnectionParameters> FlowNodeEditor::create_out_going_connection(const FlowNodeID p_target_node_id, const int p_origin_slot) const
{
	Ref<FlowNodeEditorOutGoingConnectionParameters> connection = FlowNodeEditorOutGoingConnectionParameters::create_using_slot(p_origin_slot, p_target_node_id);
	connection->set_origin_port(output_get_port_from_slot(p_origin_slot));

	return connection;
}


TypedArray<FlowNodeEditorOutGoingConnectionParameters> FlowNodeEditor::get_out_going_connections() const
{
	TypedArray<FlowNodeEditorOutGoingConnectionParameters> out_going_connections = _get_out_going_connections();

	TypedArray<Dictionary> virtual_connections;
	if (GDVIRTUAL_CALL(_get_out_going_connections, virtual_connections))
	{
		for (int i = 0; i < virtual_connections.size(); i++)
		{
			FlowNodeID new_target_flow_node_id = FLOW_NODE_ID_NIL;
			int new_origin_slot = 0;

			Dictionary current_virtual_connection = virtual_connections.get(i);

			if (current_virtual_connection.has("target_flow_node_id") && current_virtual_connection["target_flow_node_id"].get_type() == TYPE_FLOW_NODE_ID)
			{
				new_target_flow_node_id = current_virtual_connection["target_flow_node_id"];
			}
			if (current_virtual_connection.has("origin_slot") && current_virtual_connection["origin_slot"].get_type() == Variant::INT)
			{
				new_origin_slot = current_virtual_connection["origin_slot"];
			}

			if (new_target_flow_node_id != FLOW_NODE_ID_NIL)
			{
				out_going_connections.append(create_out_going_connection(new_target_flow_node_id, new_origin_slot));
			}
		}
	}

	return out_going_connections;
}


void FlowNodeEditor::_editor_init()
{
}


void FlowNodeEditor::_initialize()
{
}


void FlowNodeEditor::_clean_up()
{
}


void FlowNodeEditor::_flow_node_updated()
{
}


void FlowNodeEditor::_flow_node_updated_immediate()
{
}


void FlowNodeEditor::_update_style()
{
}


String FlowNodeEditor::_get_editor_title() const
{
	if (is_flow_node_nameable())
	{
		String node_name = get_flow_node_name().strip_edges();

		if (node_name.is_empty())
		{
			return vformat("UNNAMED %s", get_flow_type_name());
		}
		else
		{
			return vformat("%s (%s)", node_name, get_flow_type_name());
		}
	}
	else
	{
		return flow_node->get_flow_type_name();
	}
}


String FlowNodeEditor::_get_editor_tooltip_text() const
{
	PackedStringArray lines;

	String node_name = get_flow_node_name().strip_edges();
	if (!node_name.is_empty())
	{
		lines.push_back(vformat("Name: %s", node_name));
	}

	lines.push_back(vformat("ID: %d", get_flow_node_id()));
	lines.push_back(vformat("Type: %s", get_flow_type_name()));

	const static String nl_sep = "\n";
	return nl_sep.join(lines);
}


void FlowNodeEditor::_set_out_going_connection(Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection)
{
}


int FlowNodeEditor::_get_input_slot() const
{
	return -1;
}


TypedArray<FlowNodeEditorOutGoingConnectionParameters> FlowNodeEditor::_get_out_going_connections() const
{
	TypedArray<FlowNodeEditorOutGoingConnectionParameters> dummy_connections;
	return dummy_connections;
}


FlowNodeEditor::FlowNodeEditor()
{
	set_process(true);

	btn_rename = memnew(Button);
	btn_rename->set_name("Rename Button");
	btn_rename->set_h_size_flags(SIZE_SHRINK_END);
	btn_rename->set_v_size_flags(SIZE_EXPAND_FILL);
	btn_rename->connect(SNAME("pressed"), callable_mp(this, &FlowNodeEditor::request_rename));
	get_titlebar_hbox()->add_child(btn_rename, true, INTERNAL_MODE_BACK);

	btn_close = memnew(Button);
	btn_close->set_name("Close Button");
	btn_close->set_h_size_flags(SIZE_SHRINK_END);
	btn_close->set_v_size_flags(SIZE_EXPAND_FILL);
	btn_close->connect(SNAME("pressed"), callable_mp(this, &FlowNodeEditor::request_deletion));
	get_titlebar_hbox()->add_child(btn_close, true, INTERNAL_MODE_BACK);
}


FlowNodeEditor::~FlowNodeEditor()
{
}
