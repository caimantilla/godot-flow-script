#include "flow_script_editor_graph.hpp"
#include "../singletons/flow_factory.hpp"


void FlowScriptEditorGraph::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_edited_flow_script", "flow_script"), &FlowScriptEditorGraph::set_edited_flow_script);
	ClassDB::bind_method(D_METHOD("get_edited_flow_script"), &FlowScriptEditorGraph::get_edited_flow_script);
	ClassDB::bind_method(D_METHOD("has_editor_for_flow_node", "flow_node_id"), &FlowScriptEditorGraph::has_editor_for_flow_node_with_id);
	ClassDB::bind_method(D_METHOD("get_editor_for_flow_node", "flow_node_id"), &FlowScriptEditorGraph::get_editor_for_flow_node_using_id);
	ClassDB::bind_method(D_METHOD("get_flow_node_editor_using_scene_tree_name", "flow_node_editor_scene_tree_name"), &FlowScriptEditorGraph::get_flow_node_editor_using_scene_tree_name);

	ADD_SIGNAL(MethodInfo("flow_node_selected", PropertyInfo(TYPE_FLOW_NODE_ID, "flow_node_id")));
	ADD_SIGNAL(MethodInfo("flow_node_creation_dialog_requested", PropertyInfo(Variant::VECTOR2, "at_position")));
	ADD_SIGNAL(MethodInfo("flow_nodes_deletion_requested", PropertyInfo(TYPE_FLOW_NODE_ID_ARRAY, "flow_node_ids")));
	// ADD_SIGNAL(MethodInfo("flow_node_rename_dialog_requested", PropertyInfo(TYPE_FLOW_NODE_ID, "for_flow_node_id")));
}


void FlowScriptEditorGraph::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_POSTINITIALIZE:
			init_signals();
			break;
		case NOTIFICATION_PROCESS:
			if (edited_flow_script != nullptr)
			{
				if (graph_nodes_redraw_queued)
				{
					redraw_graph_nodes();
				}
				else if (graph_connections_redraw_queued)
				{
					redraw_graph_connections();
				}
			}

			graph_nodes_redraw_queued = false;
			graph_connections_redraw_queued = false;
			break;
	}
}


void FlowScriptEditorGraph::set_edited_flow_script(FlowScript *p_flow_script)
{
	ERR_FAIL_COND_MSG(!is_ready(), "Attempted to assign a FlowScript to the FlowScriptEditorGraph before ready.");

	if (p_flow_script == edited_flow_script)
	{
		return;
	}


	if (edited_flow_script != nullptr)
	{
		edited_flow_script->disconnect(SNAME("flow_node_added"), on_flow_script_flow_node_added_callback);
		edited_flow_script->disconnect(SNAME("removing_flow_node"), on_flow_script_removing_flow_node_callback);
		edited_flow_script->disconnect(SNAME("flow_node_removed"), on_flow_script_flow_node_removed_callback);
		edited_flow_script->disconnect(SNAME("flow_node_id_changed"), on_flow_script_flow_node_id_changed_callback);
		edited_flow_script->disconnect(SNAME("multiple_flow_nodes_removed"), on_flow_script_multiple_flow_nodes_removed_callback);
	}

	edited_flow_script = p_flow_script;

	// Immediately clear everything out if the new script is null, otherwise queue for the next frame
	if (p_flow_script != nullptr)
	{
		p_flow_script->connect(SNAME("flow_node_added"), on_flow_script_flow_node_added_callback);
		p_flow_script->connect(SNAME("removing_flow_node"), on_flow_script_removing_flow_node_callback);
		p_flow_script->connect(SNAME("flow_node_removed"), on_flow_script_flow_node_removed_callback);
		p_flow_script->connect(SNAME("flow_node_id_changed"), on_flow_script_flow_node_id_changed_callback);
		p_flow_script->connect(SNAME("multiple_flow_nodes_removed"), on_flow_script_multiple_flow_nodes_removed_callback);
	}

	graph_nodes_redraw_queued = false;
	graph_connections_redraw_queued = false;
	redraw_graph_nodes();
}


FlowScript *FlowScriptEditorGraph::get_edited_flow_script() const
{
	return edited_flow_script;
}


bool FlowScriptEditorGraph::has_editor_for_flow_node_with_id(const FlowNodeID p_flow_node_id) const
{
	return flow_node_editor_map.has(p_flow_node_id);
}


FlowNodeEditor *FlowScriptEditorGraph::get_editor_for_flow_node_using_id(const FlowNodeID p_flow_node_id) const
{
	ERR_FAIL_COND_V(!has_editor_for_flow_node_with_id(p_flow_node_id), nullptr);
	return flow_node_editor_map.get(p_flow_node_id);
}


Vector<FlowNodeEditor *> FlowScriptEditorGraph::get_flow_node_editor_list() const
{
	Vector<FlowNodeEditor *> editor_list;
	editor_list.resize(flow_node_editor_map.size());

	int i = 0;
	for (const KeyValue<FlowNodeID, FlowNodeEditor *> &editor_map_entry : flow_node_editor_map)
	{
		editor_list.set(i, editor_map_entry.value);
		i += 1;
	}

	return editor_list;
}


FlowNodeEditor *FlowScriptEditorGraph::get_flow_node_editor_using_scene_tree_name(const String &p_node_name) const
{
	NodePath path = NodePath(p_node_name);
	if (has_node(path))
	{
		Node *node = get_node(path);
		if (node != nullptr)
		{
			return Object::cast_to<FlowNodeEditor>(node);
		}
	}

	return nullptr;
}


void FlowScriptEditorGraph::queue_redraw_graph_nodes()
{
	graph_nodes_redraw_queued = true;
}


void FlowScriptEditorGraph::queue_redraw_graph_connections()
{
	graph_connections_redraw_queued = true;
}


// TODO: Write this later!
Vector2 FlowScriptEditorGraph::display_coordinates_to_graph_position(const Vector2i p_coordinates) const
{
	Vector2 graph_pos = Vector2(p_coordinates);
	return graph_pos;
}


void FlowScriptEditorGraph::clear_graph_nodes()
{
	flow_node_editor_map.clear();

	TypedArray<Node> children = get_children();
	for (int i = 0; i < children.size(); i++)
	{
		FlowNodeEditor *editor = Object::cast_to<FlowNodeEditor>(children.get(i));
		if (editor != nullptr)
		{
			editor->queue_free();
		}
	}

	clear_graph_connections();
}


void FlowScriptEditorGraph::clear_graph_connections()
{
	clear_connections();
}


void FlowScriptEditorGraph::redraw_graph_nodes()
{
	clear_graph_nodes();
	if (edited_flow_script == nullptr)
	{
		return;
	}

	for (const KeyValue<FlowNodeID, FlowNode *> &script_map_entry : edited_flow_script->flow_node_map)
	{
		instantiate_editor_for_flow_node(script_map_entry.key);
	}

	redraw_graph_connections();
}


void FlowScriptEditorGraph::redraw_graph_connections()
{
	clear_graph_connections();

	for (const KeyValue<FlowNodeID, FlowNodeEditor *> &editor_map_entry : flow_node_editor_map)
	{
		FlowNodeEditor *origin_node_editor = editor_map_entry.value;
		TypedArray<FlowNodeEditorOutGoingConnectionParameters> out_going_connections = origin_node_editor->get_out_going_connections();

		for (int i = 0; i < out_going_connections.size(); i++)
		{
			Variant conn_var = out_going_connections.get(i);
			ERR_FAIL_COND(conn_var.get_type() != Variant::OBJECT);

			Ref<FlowNodeEditorOutGoingConnectionParameters> connection = Object::cast_to<FlowNodeEditorOutGoingConnectionParameters>(conn_var);

			if (!has_editor_for_flow_node_with_id(connection->get_target_flow_node_id()))
			{
				continue;
			}

			FlowNodeEditor *target_node_editor = get_editor_for_flow_node_using_id(connection->get_target_flow_node_id());
			int target_node_editor_input_port = target_node_editor->get_input_port();

			if (target_node_editor_input_port > -1 && target_node_editor_input_port < target_node_editor->get_input_port_count())
			{
				connect_node(origin_node_editor->get_name(), connection->get_origin_port(), target_node_editor->get_name(), target_node_editor_input_port);
			}
		}
	}
}


FlowNodeEditor *FlowScriptEditorGraph::instantiate_editor_for_flow_node(const FlowNodeID p_flow_node_id)
{
	ERR_FAIL_NULL_V(edited_flow_script, nullptr);
	ERR_FAIL_COND_V(!edited_flow_script->has_flow_node(p_flow_node_id), nullptr);
	ERR_FAIL_COND_V(has_editor_for_flow_node_with_id(p_flow_node_id), nullptr);

	FlowNode *flow_node = edited_flow_script->get_flow_node(p_flow_node_id);
	String flow_type_id = flow_node->get_flow_type_id();
	FlowNodeEditor *flow_node_editor = FlowFactory::get_singleton()->create_flow_node_editor_for_flow_type(flow_type_id);
	flow_node_editor_map.insert(p_flow_node_id, flow_node_editor);
	add_child(flow_node_editor);

	// this needs to come AFTER adding the editor as a child!! PLEASE!!!!!!!!!!!!!!!!
	flow_node_editor->set_flow_node(flow_node);

	Callable cb_graph_position_update_requested = callable_mp(this, &FlowScriptEditorGraph::on_flow_node_editor_graph_position_update_requested).bind(flow_node_editor);
	// Callable cb_rename_dialog_requested = callable_mp(this, &FlowScriptEditorGraph::on_flow_node_editor_rename_dialog_requested).bind(flow_node_editor);
	Callable cb_delete_request = callable_mp(this, &FlowScriptEditorGraph::on_flow_node_editor_delete_request).bind(flow_node_editor);

	flow_node_editor->connect(SNAME("graph_position_update_requested"), cb_graph_position_update_requested);
	// flow_node_editor->connect(SNAME("rename_dialog_requested"), cb_rename_dialog_requested);
	flow_node_editor->connect(SNAME("delete_request"), cb_delete_request);

	flow_node_editor->request_graph_position_update();
	return flow_node_editor;
}


void FlowScriptEditorGraph::delete_editor_for_flow_node(const FlowNodeID p_flow_node_id)
{
	ERR_FAIL_COND(!has_editor_for_flow_node_with_id(p_flow_node_id));

	FlowNodeEditor *node_editor = get_editor_for_flow_node_using_id(p_flow_node_id);
	node_editor->queue_free();
	flow_node_editor_map.erase(p_flow_node_id);

	queue_redraw_graph_connections();
}


void FlowScriptEditorGraph::delete_invalid_flow_node_editors()
{
	if (edited_flow_script == nullptr)
	{
		clear_graph_nodes();
		return;
	}

	for (KeyValue<FlowNodeID, FlowNodeEditor *> editor_map_entry : flow_node_editor_map)
	{
		FlowNodeID flow_node_id = editor_map_entry.key;

		if (!edited_flow_script->has_flow_node(editor_map_entry.key))
		{
			delete_editor_for_flow_node(flow_node_id);
		}
	}
}


Ref<FlowNodeEditorOutGoingConnectionParameters> FlowScriptEditorGraph::create_flow_node_editor_out_going_connection_parameters(const int p_origin_port, const FlowNodeID p_target_node_id)
{
	return FlowNodeEditorOutGoingConnectionParameters::create_using_port(p_origin_port, p_target_node_id);
}


void FlowScriptEditorGraph::create_connection_between_flow_node_editors(FlowNodeEditor *p_from_editor, const int p_from_port, FlowNodeEditor *p_to_editor)
{
	ERR_FAIL_NULL(p_from_editor);
	ERR_FAIL_NULL(p_to_editor);

	Ref<FlowNodeEditorOutGoingConnectionParameters> connection = FlowNodeEditorOutGoingConnectionParameters::create_using_port(p_from_port, p_to_editor->get_flow_node()->get_flow_node_id());

	p_from_editor->set_out_going_connection(connection);
	queue_redraw_graph_connections();
}


void FlowScriptEditorGraph::erase_connection_from_flow_node_editor(FlowNodeEditor *p_from_editor, const int p_from_port)
{
	ERR_FAIL_NULL(p_from_editor);

	Ref<FlowNodeEditorOutGoingConnectionParameters> connection = FlowNodeEditorOutGoingConnectionParameters::create_using_port(p_from_port, FLOW_NODE_ID_NIL);
	p_from_editor->set_out_going_connection(connection);

	queue_redraw_graph_connections();
}


void FlowScriptEditorGraph::update_flow_node_editor_graph_position(FlowNodeEditor *p_flow_node_editor)
{
	ERR_FAIL_NULL(p_flow_node_editor);
	ERR_FAIL_NULL(p_flow_node_editor->get_flow_node());

	Vector2i internal_position = p_flow_node_editor->get_flow_node()->get_flow_graph_position();
	Vector2 screen_position = EDSCALE * Vector2(internal_position);

	print_line("Updating position of FlowNodeEditor ", p_flow_node_editor->get_flow_node_id(), " to ", screen_position);

	p_flow_node_editor->set_position_offset(screen_position.floor());
}


void FlowScriptEditorGraph::emit_flow_node_selected(const FlowNodeID p_flow_node_id)
{
	emit_signal(SNAME("flow_node_selected"), p_flow_node_id);
}


// void FlowScriptEditorGraph::request_flow_node_rename_dialog(const FlowNodeID p_current_flow_node_id)
// {
// 	emit_signal(SNAME("flow_node_rename_dialog_requested"), p_current_flow_node_id);
// }


void FlowScriptEditorGraph::request_flow_node_creation_dialog(const Vector2 &p_at_position)
{
	emit_signal(SNAME("flow_node_creation_dialog_requested"), p_at_position);
}


void FlowScriptEditorGraph::on_graph_connection_request(const StringName &p_from_node_tree_name, int p_from_port, const StringName &p_to_node_tree_name, int p_to_port)
{
	FlowNodeEditor *origin_editor = get_flow_node_editor_using_scene_tree_name(p_from_node_tree_name);
	FlowNodeEditor *target_editor = get_flow_node_editor_using_scene_tree_name(p_to_node_tree_name);

	if (origin_editor == nullptr || target_editor == nullptr)
	{
		return;
	}

	create_connection_between_flow_node_editors(origin_editor, p_from_port, target_editor);
}


// TODO: Add something for connections to empty? Even something simple like disconnection, idk. Later!
void FlowScriptEditorGraph::on_graph_connection_to_empty(const StringName &p_from_node_tree_name, int p_from_port, const Vector2 &p_release_position)
{}


// TODO: Write the copy request handler. I think that what I should do is save the state of the node, its type, etc. but clear all connection IDs.
// Any nodes which have sub-objects will probably break, though. Maybe I should make them Resources rather than Objects so that I can auto-duplicate subresources?
// Hmm...
void FlowScriptEditorGraph::on_graph_copy_nodes_request()
{}


void FlowScriptEditorGraph::on_graph_delete_nodes_request(const TypedArray<StringName> &p_nodes)
{
	FlowNodeIDArray flow_node_ids;
	flow_node_ids.resize(p_nodes.size());

	for (int i = 0; i < p_nodes.size(); i++)
	{
		FlowNodeEditor *editor = get_flow_node_editor_using_scene_tree_name(p_nodes.get(i));
		flow_node_ids.set(i, editor->get_flow_node()->get_flow_node_id());
	}

	emit_signal(SNAME("flow_nodes_deletion_requested"), flow_node_ids);
}


void FlowScriptEditorGraph::on_graph_disconnection_request(const StringName &p_from_node_tree_name, int p_from_port, const StringName &p_to_node_tree_name, int p_to_port)
{
	FlowNodeEditor *from_editor = get_flow_node_editor_using_scene_tree_name(p_from_node_tree_name);

	if (from_editor == nullptr)
	{
		return;
	}

	erase_connection_from_flow_node_editor(from_editor, p_from_port);
}


void FlowScriptEditorGraph::on_graph_node_deselected(Node *p_node)
{
	FlowNodeEditor *editor = Object::cast_to<FlowNodeEditor>(p_node);
	if (editor == nullptr)
	{
		return;
	}

	emit_flow_node_selected(FLOW_NODE_ID_NIL);
}


void FlowScriptEditorGraph::on_graph_node_selected(Node *p_node)
{
	FlowNodeEditor *editor = Object::cast_to<FlowNodeEditor>(p_node);
	if (editor == nullptr)
	{
		return;
	}

	emit_flow_node_selected(editor->get_flow_node()->get_flow_node_id());
}


void FlowScriptEditorGraph::on_graph_end_node_move()
{
	for (KeyValue<FlowNodeID, FlowNodeEditor *> editor_map_entry : flow_node_editor_map)
	{
		FlowNodeEditor *editor = editor_map_entry.value;
		editor->silent_copy_graph_position_to_flow_node();
	}
}


void FlowScriptEditorGraph::on_graph_popup_request(const Vector2 &p_at_position)
{
	request_flow_node_creation_dialog(p_at_position);
}


void FlowScriptEditorGraph::on_flow_script_flow_node_added(const FlowNodeID p_flow_node_id)
{
	if (has_editor_for_flow_node_with_id(p_flow_node_id))
	{
		return;
	}

	instantiate_editor_for_flow_node(p_flow_node_id);
}


// Remove later probably
void FlowScriptEditorGraph::on_flow_script_removing_flow_node(const FlowNodeID p_flow_node_id)
{}


// Is this needed, or is the multiple FlowNode removal signal always emitted?
// Examine the FlowScript class later.
void FlowScriptEditorGraph::on_flow_script_flow_node_removed(const FlowNodeID p_flow_node_id)
{
	if (p_flow_node_id == FLOW_NODE_ID_NIL)
	{
		return;
	}

	FlowNodeIDArray node_id_list;
	node_id_list.push_back(p_flow_node_id);
	on_flow_script_multiple_flow_nodes_removed(node_id_list);
}


void FlowScriptEditorGraph::on_flow_script_flow_node_id_changed(const FlowNodeID p_from_id, const FlowNodeID p_to_id)
{
	if (p_from_id == p_to_id)
	{
		return;
	}

	if (has_editor_for_flow_node_with_id(p_from_id))
	{
		FlowNodeEditor *editor = get_editor_for_flow_node_using_id(p_from_id);
		flow_node_editor_map.erase(p_from_id);
		flow_node_editor_map.insert(p_to_id, editor);
	}

	// Optimize this to only change the edited node's title and tooltip...
	// I think that a public method would work, rather than exclusively doing it inside the initialize() method as it is at the time of writing.
	queue_redraw_graph_nodes();
}


// void FlowScriptEditorGraph::on_flow_node_editor_rename_dialog_requested(FlowNodeEditor *p_editor)
// {
// 	StringName node_id = p_editor->get_flow_node()->get_flow_node_id();
// 	request_flow_node_rename_dialog(node_id);
// }


void FlowScriptEditorGraph::on_flow_node_editor_graph_position_update_requested(FlowNodeEditor *p_editor)
{
	update_flow_node_editor_graph_position(p_editor);
}


void FlowScriptEditorGraph::on_flow_node_editor_delete_request(FlowNodeEditor *p_editor)
{
	TypedArray<StringName> tree_name_list;
	tree_name_list.append(p_editor->get_name());

	on_graph_delete_nodes_request(tree_name_list);
}


void FlowScriptEditorGraph::on_flow_script_multiple_flow_nodes_removed(const FlowNodeIDArray &p_flow_node_ids)
{
	for (int i = 0; i < p_flow_node_ids.size(); i++)
	{
		FlowNodeID current_node_id = p_flow_node_ids.get(i);

		print_line(vformat("is there editor for FlowNode %d?", current_node_id));
		if (has_editor_for_flow_node_with_id(current_node_id))
		{
			print_line("yes");
			delete_editor_for_flow_node(current_node_id);
		}
	}
}


void FlowScriptEditorGraph::init_signals()
{
	connect(SNAME("connection_request"), on_graph_connection_request_callback);
	connect(SNAME("connection_to_empty"), on_graph_connection_to_empty_callback);
	connect(SNAME("copy_nodes_request"), on_graph_copy_nodes_request_callback);
	connect(SNAME("delete_nodes_request"), on_graph_delete_nodes_request_callback);
	connect(SNAME("disconnection_request"), on_graph_disconnection_request_callback);
	connect(SNAME("node_deselected"), on_graph_node_deselected_callback);
	connect(SNAME("node_selected"), on_graph_node_selected_callback);
	connect(SNAME("end_node_move"), on_graph_end_node_move_callback);
	connect(SNAME("popup_request"), on_graph_popup_request_callback);
}


FlowScriptEditorGraph::FlowScriptEditorGraph()
{
	set_process(true);

	Vector2 min_size = (EDSCALE * Vector2(128, 128)).floor();
	set_custom_minimum_size(min_size);

	set_connection_lines_antialiased(true);
	set_show_grid(false);
	set_show_grid_buttons(false);
	set_snapping_enabled(false);
	set_right_disconnects(true);

	on_graph_connection_request_callback = callable_mp(this, &FlowScriptEditorGraph::on_graph_connection_request);
	on_graph_connection_to_empty_callback = callable_mp(this, &FlowScriptEditorGraph::on_graph_connection_to_empty);
	on_graph_copy_nodes_request_callback = callable_mp(this, &FlowScriptEditorGraph::on_graph_copy_nodes_request);
	on_graph_delete_nodes_request_callback = callable_mp(this, &FlowScriptEditorGraph::on_graph_delete_nodes_request);
	on_graph_disconnection_request_callback = callable_mp(this, &FlowScriptEditorGraph::on_graph_disconnection_request);
	on_graph_node_deselected_callback = callable_mp(this, &FlowScriptEditorGraph::on_graph_node_deselected);
	on_graph_node_selected_callback = callable_mp(this, &FlowScriptEditorGraph::on_graph_node_selected);
	on_graph_end_node_move_callback = callable_mp(this, &FlowScriptEditorGraph::on_graph_end_node_move);
	on_graph_popup_request_callback = callable_mp(this, &FlowScriptEditorGraph::on_graph_popup_request);

	on_flow_script_flow_node_added_callback = callable_mp(this, &FlowScriptEditorGraph::on_flow_script_flow_node_added);
	on_flow_script_removing_flow_node_callback = callable_mp(this, &FlowScriptEditorGraph::on_flow_script_removing_flow_node);
	on_flow_script_flow_node_removed_callback = callable_mp(this, &FlowScriptEditorGraph::on_flow_script_flow_node_removed);
	on_flow_script_flow_node_id_changed_callback = callable_mp(this, &FlowScriptEditorGraph::on_flow_script_flow_node_id_changed);
	on_flow_script_multiple_flow_nodes_removed_callback = callable_mp(this, &FlowScriptEditorGraph::on_flow_script_multiple_flow_nodes_removed);
}
