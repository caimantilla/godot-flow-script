#include "flow_script_editor_plugin.hpp"
#include "editor_inspector_plugin_flow_script.hpp"
#include "../../flow_script_node_instance.hpp"
#include "../flow_script_node_type_db.hpp"
#include "../flow_script_node_type_info.hpp"
#include "editor/editor_string_names.h"
#include "editor/editor_node.h"
#include "editor/editor_interface.h"
#include "editor/editor_inspector.h"
#include "editor/themes/editor_scale.h"
#include "editor/gui/editor_bottom_panel.h"
#include "editor/editor_settings.h"
#include "editor/editor_command_palette.h"
#include "editor/editor_undo_redo_manager.h"


FlowScriptEditorPlugin::ScriptItemList::ScriptItemList(FlowScriptEditorPlugin *p_plugin)
{
	plugin = p_plugin;
	set_auto_translate(false);
}


void FlowScriptEditorPlugin::ScriptGraph::_notification(int p_what)
{
	if (p_what == NOTIFICATION_THEME_CHANGED)
	{
		Ref<Font> main_font = EditorNode::get_singleton()->get_editor_theme()->get_font("main_msdf", EditorStringName(EditorFonts));
		Ref<Font> bold_font = EditorNode::get_singleton()->get_editor_theme()->get_font("main_bold_msdf", EditorStringName(EditorFonts));

		msdf_theme->set_default_font(main_font);
		msdf_theme->set_font("font", "Label", main_font);
		msdf_theme->set_font("font", "GraphNodeTitleLabel", bold_font);
		msdf_theme->set_font("normal_font", "RichTextLabel", main_font);
		msdf_theme->set_font("bold_font", "RichTextLabel", bold_font);
	}
}


void FlowScriptEditorPlugin::ScriptGraph::add_node_editor(FlowScriptNodeEditor *p_editor)
{
	p_editor->set_theme(msdf_theme);
	add_child(p_editor);
}


Point2 FlowScriptEditorPlugin::ScriptGraph::point_convert_data_to_graph(const Point2i &p_data_position) const
{
	Point2 ret = p_data_position;
	ret *= EDSCALE;
	return ret;
}


Point2i FlowScriptEditorPlugin::ScriptGraph::point_convert_graph_to_data(const Point2 &p_screen_position) const
{
	Point2 retf;
	retf /= EDSCALE;
	retf = retf.round();
	// autoconvert type
	return retf;
}


Point2 FlowScriptEditorPlugin::ScriptGraph::point_convert_rect_to_graph(const Point2 &p_rect_position) const
{
	Point2 ret = p_rect_position;
	ret += get_scroll_offset();
	ERR_FAIL_COND_V_MSG(Math::is_zero_approx(get_zoom()), ret, "WHY IS THE ZOOM ZERO!!!");
	ret /= get_zoom();
	return ret;
}


Point2 FlowScriptEditorPlugin::ScriptGraph::point_convert_graph_to_rect(const Point2 &p_graph_position) const
{
	// TODO: WRITE THIS!! STOP BEING LAZY!!!!!
	ERR_FAIL_V(p_graph_position);
}


const Vector<FlowScriptEditorPlugin::EditedNode *> &FlowScriptEditorPlugin::ScriptGraph::get_selected_nodes() const
{
	return selected_nodes;
}


void FlowScriptEditorPlugin::ScriptGraph::on_begin_node_move()
{
	plugin->set_drag_state(DRAG_NODE_EDITOR);
}


void FlowScriptEditorPlugin::ScriptGraph::on_end_node_move()
{
}


void FlowScriptEditorPlugin::ScriptGraph::on_node_selected(Node *p_node)
{
}


void FlowScriptEditorPlugin::ScriptGraph::on_node_deselected(Node *p_node)
{
}


void FlowScriptEditorPlugin::ScriptGraph::on_copy_nodes_request()
{
}


FlowScriptEditorPlugin::ScriptGraph::ScriptGraph(FlowScriptEditorPlugin *p_plugin)
{
	plugin = p_plugin;
	msdf_theme.instantiate();

	set_h_size_flags(Control::SIZE_EXPAND_FILL);
	set_v_size_flags(Control::SIZE_EXPAND_FILL);
}


void FlowScriptEditorPlugin::GraphHoverConnectionBreakElement::_notification(int p_what)
{
}


FlowScriptEditorPlugin::GraphHoverConnectionBreakElement::GraphHoverConnectionBreakElement()
{
}


void FlowScriptEditorPlugin::EditedNode::copy_position_to_node()
{
	if (!is_edit_permitted())
	{
		return;
	}
	Point2 input_scr_pos = editor->get_position_offset();
	input_scr_pos += editor->get_size() * 0.5;
	Point2i new_data_pos = plugin->graph->point_convert_graph_to_data(input_scr_pos);
	editor->get_edited_flow_script_ptr()->set_node_position(editor->get_edited_node_id(), new_data_pos);
}


void FlowScriptEditorPlugin::EditedNode::match_position_of_node()
{
	edit_block_counter++;

	Point2 new_scr_pos = plugin->graph->point_convert_data_to_graph(editor->get_edited_flow_script_ptr()->get_node_position(editor->get_edited_node_id()));
	new_scr_pos -= editor->get_size() * 0.5;
	editor->set_position_offset(new_scr_pos);

	edit_block_counter--;
	position_match_request_dirty = false;
}


void FlowScriptEditorPlugin::EditedNode::on_delete_request()
{
	if (!is_edit_permitted())
	{
		return;
	}
}


void FlowScriptEditorPlugin::EditedNode::on_resized()
{
	if (position_match_request_dirty)
	{
		return;
	}
	position_match_request_dirty = true;
	callable_mp(this, &EditedNode::match_position_of_node).call_deferred();
}


void FlowScriptEditorPlugin::EditedNode::on_dragged(Point2 p_from, Point2 p_to)
{
	Point2i data_to = plugin->graph->point_convert_graph_to_data(p_to + (editor->get_size() * 0.5));
	edited_script->edited_node_on_dragged(this, edited_script->flow_script->get_node_position(get_edited_node_id()), p_to);
}


void FlowScriptEditorPlugin::EditedNode::on_node_deselected()
{
	edited_script->edited_node_on_selected(this);
}


void FlowScriptEditorPlugin::EditedNode::on_node_selected()
{
	edited_script->edited_node_on_deselected(this);
}


void FlowScriptEditorPlugin::EditedNode::on_position_offset_changed()
{
}


void FlowScriptEditorPlugin::EditedNode::on_raise_request()
{
	edited_script->edited_node_on_raise_request(this);
}


void FlowScriptEditorPlugin::EditedNode::inspect_edited_node()
{
	if (is_edit_permitted())
	{
		EditorInterface::get_singleton()->edit_resource(editor->get_edited_node_ref());
	}
}


bool FlowScriptEditorPlugin::EditedNode::is_edit_permitted() const
{
	return editor->is_edited_flow_script_root() && edit_block_counter == 0;
}


FlowScriptEditorPlugin::EditedNode::EditedNode(FlowScriptEditorPlugin *p_plugin, FlowScriptNodeEditor *p_editor, EditedScript *p_edited_script)
{
	plugin = p_plugin;
	editor = p_editor;
	edited_script = p_edited_script;

	editor->connect(SNAME("delete_request"), callable_mp(this, &EditedNode::on_delete_request));
	editor->connect(SNAME("dragged"), callable_mp(this, &FlowScriptEditorPlugin::EditedNode::on_dragged));
	editor->connect(SNAME("node_deselected"), callable_mp(this, &EditedNode::on_node_deselected));
	editor->connect(SNAME("node_selected"), callable_mp(this, &EditedNode::on_node_selected));
	editor->connect(SNAME("position_offset_changed"), callable_mp(this, &EditedNode::on_position_offset_changed));
	editor->connect(SNAME("raise_request"), callable_mp(this, &EditedNode::on_raise_request));
}


void FlowScriptEditorPlugin::EditedScript::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("_undo_element_drag"), &EditedScript::_undo_element_drag);
	ClassDB::bind_method(D_METHOD("_redo_element_drag"), &EditedScript::_redo_element_drag);
	ClassDB::bind_method(D_METHOD("_undo_add_node"), &EditedScript::_undo_add_node);
	ClassDB::bind_method(D_METHOD("_redo_add_node"), &EditedScript::_redo_add_node);
	ClassDB::bind_method(D_METHOD("_undo_delete_node"), &EditedScript::_undo_delete_node);
	ClassDB::bind_method(D_METHOD("_redo_delete_node"), &EditedScript::_redo_delete_node);
	ClassDB::bind_method(D_METHOD("_undo_add_include"), &EditedScript::_undo_add_include);
	ClassDB::bind_method(D_METHOD("_redo_add_include"), &EditedScript::_redo_add_include);
	ClassDB::bind_method(D_METHOD("_undo_delete_include"), &EditedScript::_undo_delete_include);
	ClassDB::bind_method(D_METHOD("_redo_delete_include"), &EditedScript::_redo_delete_include);
	ClassDB::bind_method(D_METHOD("_undo_connect_nodes"), &EditedScript::_undo_connect_nodes);
	ClassDB::bind_method(D_METHOD("_redo_connect_nodes"), &EditedScript::_redo_connect_nodes);
	ClassDB::bind_method(D_METHOD("_undo_disconnect_nodes"), &EditedScript::_undo_disconnect_nodes);
	ClassDB::bind_method(D_METHOD("_redo_disconnect_nodes"), &EditedScript::_redo_disconnect_nodes);
}


bool FlowScriptEditorPlugin::EditedScript::is_root() const
{
	return parent == nullptr;
}


bool FlowScriptEditorPlugin::EditedScript::is_first_level_include() const
{
	return parent != nullptr && parent->is_root();
}


void FlowScriptEditorPlugin::EditedScript::connect_graph()
{
	graph->connect(SNAME("begin_node_move"), callable_mp(this, &EditedScript::on_graph_begin_node_move));
	graph->connect(SNAME("connection_drag_ended"), callable_mp(this, &EditedScript::on_graph_connection_drag_ended));
	graph->connect(SNAME("connection_drag_started"), callable_mp(this, &EditedScript::on_graph_connection_drag_started));
	graph->connect(SNAME("connection_from_empty"), callable_mp(this, &EditedScript::on_graph_connection_from_empty));
	graph->connect(SNAME("connection_request"), callable_mp(this, &EditedScript::on_graph_connection_request));
	graph->connect(SNAME("connection_to_empty"), callable_mp(this, &EditedScript::on_graph_connection_to_empty));
	graph->connect(SNAME("copy_nodes_request"), callable_mp(this, &EditedScript::on_graph_copy_nodes_request));
	graph->connect(SNAME("delete_nodes_request"), callable_mp(this, &EditedScript::on_graph_delete_nodes_request));
	graph->connect(SNAME("disconnection_request"), callable_mp(this, &EditedScript::on_graph_disconnection_request));
	graph->connect(SNAME("duplicate_nodes_request"), callable_mp(this, &EditedScript::on_graph_duplicate_nodes_request));
	graph->connect(SNAME("end_node_move"), callable_mp(this, &EditedScript::on_graph_end_node_move));
	graph->connect(SNAME("frame_rect_changed"), callable_mp(this, &EditedScript::on_graph_frame_rect_changed));
	graph->connect(SNAME("node_deselected"), callable_mp(this, EditedScript::on_graph_node_deselected));
	graph->connect(SNAME("node_selected"), callable_mp(this, &EditedScript::on_graph_node_selected));
	graph->connect(SNAME("paste_nodes_request"), callable_mp(this, &EditedScript::on_graph_paste_nodes_request));
	graph->connect(SNAME("popup_request"), callable_mp(this, &EditedScript::on_graph_popup_request));
	graph->connect(SNAME("scroll_offset_changed"), callable_mp(this, &EditedScript::on_graph_scroll_offset_changed));
}


void FlowScriptEditorPlugin::EditedScript::disconnect_graph()
{
	graph->disconnect(SNAME("begin_node_move"), callable_mp(this, &EditedScript::on_graph_begin_node_move));
	graph->disconnect(SNAME("connection_drag_ended"), callable_mp(this, &EditedScript::on_graph_connection_drag_ended));
	graph->disconnect(SNAME("connection_drag_started"), callable_mp(this, &EditedScript::on_graph_connection_drag_started));
	graph->disconnect(SNAME("connection_from_empty"), callable_mp(this, &EditedScript::on_graph_connection_from_empty));
	graph->disconnect(SNAME("connection_request"), callable_mp(this, &EditedScript::on_graph_connection_request));
	graph->disconnect(SNAME("connection_to_empty"), callable_mp(this, &EditedScript::on_graph_connection_to_empty));
	graph->disconnect(SNAME("copy_nodes_request"), callable_mp(this, &EditedScript::on_graph_copy_nodes_request));
	graph->disconnect(SNAME("delete_nodes_request"), callable_mp(this, &EditedScript::on_graph_delete_nodes_request));
	graph->disconnect(SNAME("disconnection_request"), callable_mp(this, &EditedScript::on_graph_disconnection_request));
	graph->disconnect(SNAME("duplicate_nodes_request"), callable_mp(this, &EditedScript::on_graph_duplicate_nodes_request));
	graph->disconnect(SNAME("end_node_move"), callable_mp(this, &EditedScript::on_graph_end_node_move));
	graph->disconnect(SNAME("frame_rect_changed"), callable_mp(this, &EditedScript::on_graph_frame_rect_changed));
	graph->disconnect(SNAME("node_deselected"), callable_mp(this, EditedScript::on_graph_node_deselected));
	graph->disconnect(SNAME("node_selected"), callable_mp(this, &EditedScript::on_graph_node_selected));
	graph->disconnect(SNAME("paste_nodes_request"), callable_mp(this, &EditedScript::on_graph_paste_nodes_request));
	graph->disconnect(SNAME("popup_request"), callable_mp(this, &EditedScript::on_graph_popup_request));
	graph->disconnect(SNAME("scroll_offset_changed"), callable_mp(this, &EditedScript::on_graph_scroll_offset_changed));
}


const FlowScriptEditorPlugin::EditedScript::GraphItem FlowScriptEditorPlugin::EditedScript::get_graph_node_as_item_by_ptr(Node *node) const
{
	ERR_FAIL_NULL_V(node, GraphItem());
	FlowScriptNodeEditor *editor = Object::cast_to<FlowScriptNodeEditor>(node);
	if (editor != nullptr)
	{
		return GraphItem {
			.type = GraphItem::TYPE_NODE,
			.node_id = editor->get_edited_node_id(),
		};
	}
	FlowScriptIncludeEditorFrame *frame = Object::cast_to<FlowScriptIncludeEditorFrame>(node);
	if (frame != nullptr)
	{
		return GraphItem {
			.type = GraphItem::TYPE_INCLUDE,
			.include_id = frame->include_id,
		};
	}
	ERR_FAIL_V(GraphItem());
}


const FlowScriptEditorPlugin::EditedScript::GraphItem FlowScriptEditorPlugin::EditedScript::get_graph_node_as_item_by_name(const StringName &p_name) const
{
	int child_count = graph->get_child_count(false);
	for (int i = 0; i < child_count; i++)
	{
		Node *current_node = graph->get_child(i, false);
		if (current_node->get_name() == p_name)
		{
			return get_graph_node_as_item_by_ptr(current_node);
		}
	}
	ERR_FAIL_V(GraphItem());
}


Point2i FlowScriptEditorPlugin::EditedScript::get_graph_item_data_position(const GraphItem &p_item) const
{
	switch (p_item.type)
	{
		case GraphItem::TYPE_NODE:
			return flow_script->get_node_position(p_item.node_id);
		case GraphItem::TYPE_INCLUDE:
			return flow_script->get_include_flow_script_position(p_item.include_id);
		default:
			ERR_FAIL_V(Point2i());
	}
}


void FlowScriptEditorPlugin::EditedScript::edited_node_on_dragged(EditedNode *p_node, const Point2i &p_from, const Point2i &p_to)
{
	if (item_drag_buffer.size() == 0)
	{
		callable_mp(this, &EditedScript::handle_drag_buffer).call_deferred();
	}
	item_drag_buffer.push_back(ItemDragOperation {
		.item = GraphItem {
			.type = GraphItem::TYPE_NODE,
			.node_id = p_node->get_edited_node_id()
		},
		.from = p_from,
		.to = p_to,
	});
}


// Apply drags, add to undo/redo stack and clear buffer
void FlowScriptEditorPlugin::EditedScript::handle_drag_buffer()
{
	if (item_drag_buffer.size() == 0)
	{
		return;
	}
	int dragged_type_flags = 0;
	for (int i = 0; i < item_drag_buffer.size(); i++)
	{
		dragged_type_flags |= item_drag_buffer[i].item.type;
	}
	String action_name;
	switch (dragged_type_flags)
	{
		case GraphItem::TYPE_NODE | GraphItem::TYPE_INCLUDE:
			action_name = TTR("Move FlowScript Item(s)");
			break;
		case GraphItem::TYPE_NODE:
			action_name = TTR("Move FlowScript Node(s)");
			break;
		case GraphItem::TYPE_INCLUDE:
			action_name = TTR("Move FlowScript Include(s)");
			break;
	}
	if (action_name.is_empty())
	{
		item_drag_buffer.clear();
		ERR_FAIL_MSG(TTR("No valid items dragged..."));
	}
	EditorUndoRedoManager *udrd = EditorUndoRedoManager::get_singleton();
	udrd->create_action(action_name, UndoRedo::MERGE_DISABLE, this, false);
	for (int i = 0; i < item_drag_buffer.size(); i++)
	{
		const ItemDragOperation &op = item_drag_buffer[i];
		switch (op.item.type)
		{
			case GraphItem::TYPE_NODE:
				udrd->add_do_method(flow_script.ptr(), SNAME("set_node_position"), op.item.node_id, op.to);
				udrd->add_undo_method(flow_script.ptr(), SNAME("set_node_position"), op.item.node_id, op.from);
				break;
			case GraphItem::TYPE_INCLUDE:
				udrd->add_do_method(flow_script.ptr(), SNAME("set_include_position"), op.item.include_id, op.to);
				udrd->add_do_method(flow_script.ptr(), SNAME("set_include_position"), op.item.include_id, op.from);
				flow_script->set_include_flow_script_position(op.item.include_id, op.to);
				break;
		}
	}
	udrd->commit_action(true);
	item_drag_buffer.clear();
}


void FlowScriptEditorPlugin::EditedScript::on_graph_begin_node_move()
{
	active_drag_operations.resize(selected_items.size());
	for (int i = 0; i < selected_items.size(); i++)
	{
		active_drag_operations.write[i] = {
			.item = selected_items[i],
			.from = get_graph_item_data_position(selected_items[i]),
		};
	}
}


void FlowScriptEditorPlugin::EditedScript::on_graph_end_node_move()
{
	ERR_FAIL_COND(active_drag_operations.is_empty());

	for (int i = 0; i < active_drag_operations.size(); i++)
	{
		graph_item_copy_position_to_data(active_drag_operations[i].item);
		active_drag_operations.write[i].to = get_graph_item_data_position(active_drag_operations[i].item);
	}

	EditorUndoRedoManager *udrd = EditorUndoRedoManager::get_singleton();
	udrd->create_action(TTR("Move FlowScript Item(s)"), UndoRedo::MERGE_DISABLE, this, false);

	for (int i = 0; i < active_drag_operations.size(); i++)
	{
		const ItemDragOperation &op = active_drag_operations[i];
		switch (active_drag_operations[i].item.type)
		{
			case GraphItem::TYPE_NODE:
				udrd->add_do_method(flow_script.ptr(), SNAME("set_node_position"), op.item.node_id, op.to);
				udrd->add_undo_method(flow_script.ptr(), SNAME("set_node_position"), op.item.node_id, op.from);
				break;
			case GraphItem::TYPE_INCLUDE:
				udrd->add_do_method(flow_script.ptr(), SNAME("set_include_position"), op.item.include_id, op.to);
				udrd->add_undo_method(flow_script.ptr(), SNAME("set_include_position"), op.item.include_id, op.from);
				break;
		}
	}

	active_drag_operations.clear();
}


FlowScriptEditorPlugin::EditedScript::EditedScript(EditedScript *p_parent, FlowScriptEditorPlugin *p_plugin, const Ref<FlowScript> &p_flow_script)
{
	parent = p_parent;
	plugin = p_plugin;
	flow_script = p_flow_script;

	graph = plugin->graph;
}


FlowScriptEditorPlugin::EditedScript::~EditedScript()
{
	for (FlowScriptIncludeID i = 0; i < FlowScript::INCLUDE_FLOW_SCRIPT_MAX; i++)
	{
		if (flow_script->has_include_flow_script_instance(i))
		{
			memdelete(children[i]);
		}
	}
}


void FlowScriptEditorPlugin::on_file_menu_item_pressed(int p_idx)
{
	switch (p_idx)
	{
		case FILE_NEW:
			break;
		case FILE_OPEN:
			break;
		case FILE_SAVE:
			break;
		case FILE_SAVE_AS:
			break;
		case FILE_CLOSE:
			close_edited_script();
			break;
	}
}


void FlowScriptEditorPlugin::on_node_create_dialog_type_chosen(const StringName &p_native_class, const StringName &p_script_class)
{
	node_create_dialog->hide();

	List<FlowScriptNodeTypeInfo> type_list;
	FlowScriptNodeTypeDB::get_singleton()->get_node_type_list(&type_list);

	Ref<FlowScriptNode> node;

	for (const FlowScriptNodeTypeInfo &type : type_list)
	{
		if (type.enabled && (type.node_script_class_name == p_script_class || type.node_class == p_native_class))
		{
			node = FlowScriptNodeTypeDB::get_singleton()->instantiate_node_for_type(type);
			break;
		}
	}

	ERR_FAIL_COND(!node.is_valid());
	FlowScriptNodeID id = get_edited_flow_script()->add_node_to_first_available_slot(node);
	ERR_FAIL_COND(id == FlowScript::NODE_ID_INVALID);
	get_edited_flow_script()->set_node_position(id, graph->point_convert_graph_to_data(next_node_create_point));
}


bool FlowScriptEditorPlugin::handles(Object *p_object) const
{
	return Object::cast_to<FlowScript>(p_object) != nullptr;
}


void FlowScriptEditorPlugin::edit(Object *p_object)
{
	FlowScript *flow_script_ptr = Object::cast_to<FlowScript>(p_object);
	if (flow_script_ptr == nullptr)
	{
		return;
	}
	// TODO: Automatically open the script
	make_visible(true);
}


void FlowScriptEditorPlugin::refresh_connection_hover_break_spots()
{
	if (!is_editing_script())
	{
		connection_hover_break_spot_list.clear();
		return;
	}
	const EditedScript &edited_script = get_current_edited_script();
	int64_t connection_count = 0;
	for (const KeyValue<FlowScriptNodeID, FlowScriptNodeInstance> &kv : edited_script.flow_script->node_map)
	{
		const FlowScriptNodeInstance &node_instance = kv.value;
		for (const Vector<FlowScriptNodeReference> &references : node_instance.connection_lists)
		{
			connection_count += references.size();
		}
	}
	connection_hover_break_spot_list.resize(connection_count);
	int64_t spot_idx = 0;
	for (const KeyValue<FlowScriptNodeID, FlowScriptNodeInstance> &kv : edited_script.flow_script->node_map)
	{
		const FlowScriptNodeID origin_node_id = kv.key;
		ERR_CONTINUE(!edited_script.node_editor_map.has(origin_node_id));

		EditedNode *origin_node_editor = edited_script.node_editor_map[origin_node_id];
		const FlowScriptNodeInstance &origin_node_instance = kv.value;
		for (int list_idx = 0; list_idx < origin_node_instance.connection_lists.size(); list_idx++)
		{
			for (int slot_idx = 0; slot_idx < origin_node_instance.connection_lists.get(list_idx).size(); slot_idx++)
			{
				GraphHoverConnectionBreakSpot spot_data = {
					.enabled = false,
					.node_id = origin_node_id,
					.connection = FlowScriptNodeOutputConnection(list_idx, slot_idx),
				};
				const FlowScriptNodeReference &reference = origin_node_instance.connection_lists.get(list_idx).get(slot_idx);
				if (reference.is_valid())
				{
					EditedNode *target_node_editor = edited_script.get_edited_node_by_reference(reference);
					ERR_CONTINUE(target_node_editor == nullptr);

					spot_data.enabled = true;

					int origin_graph_port = origin_node_editor->editor->get_slot_port_index_right(origin_node_editor->editor->output_connection_to_graph_slot(spot_data.connection));
					int target_graph_port = target_node_editor->editor->get_slot_port_index_left(origin_node_editor->editor->get_input_slot());

					Point2 origin_position = origin_node_editor->editor->get_output_port_position(origin_graph_port);
					Point2 target_position = target_node_editor->editor->get_input_port_position(target_graph_port);

					spot_data.position = origin_position.lerp(target_position, 0.5);
				}
				connection_hover_break_spot_list.write[spot_idx] = spot_data;
				spot_idx++;
			}
		}
	}
}


void FlowScriptEditorPlugin::redraw_graph_connections()
{
	graph->clear_connections();

	Ref<FlowScript> flow_script = get_edited_flow_script();
	if (!flow_script.is_valid())
	{
		return;
	}
}


String FlowScriptEditorPlugin::get_name() const
{
	return "FlowScript";
}


const Ref<Texture2D> FlowScriptEditorPlugin::get_icon() const
{
	Ref<Image> img = EditorNode::get_singleton()->get_editor_theme()->get_icon(SNAME("GraphEdit"), EditorStringName(EditorIcons))->get_image();
	img->adjust_bcs(5, 5, 0);
	Ref<Texture2D> tex = ImageTexture::create_from_image(img);
	return tex;
}


String FlowScriptEditorPlugin::get_plugin_version() const
{
	return "1.0";
}


void FlowScriptEditorPlugin::make_visible(bool p_visible)
{
	make_bottom_panel_item_visible(window_wrapper);
}


FlowScriptEditorPlugin::FlowScriptEditorPlugin()
{
	clipboard_handler.init(this);

	window_wrapper = memnew(WindowWrapper);
	window_wrapper->set_window_title(vformat(TTR("%s - Godot Engine"), TTR("FlowScript Editor")));
	window_wrapper->set_margins_enabled(true);
	window_wrapper->connect("window_visibility_changed", callable_mp(this, &FlowScriptEditorPlugin::on_window_visibility_changed));

	bottom_panel_button = EditorNode::get_bottom_panel()->add_item(TTR("FlowScript Editor"), window_wrapper, ED_SHORTCUT_AND_COMMAND("bottom_panels/toggle_flow_script_editor_bottom_panel", TTR("Toggle FlowScript Editor Bottom Panel")));

	main_split = memnew(HSplitContainer);
	Ref<Shortcut> make_floating_shortcut = ED_SHORTCUT_AND_COMMAND("flow_script_editor/make_floating", TTR("Make Floating"));
	window_wrapper->set_wrapped_control(main_split, make_floating_shortcut);

	VBoxContainer *left_vbox = memnew(VBoxContainer);
	left_vbox->set_custom_minimum_size(Size2(200, 300) * EDSCALE);
	main_split->add_child(left_vbox);

	HBoxContainer *menu_hbox = memnew(HBoxContainer);
	left_vbox->add_child(menu_hbox);

	file_menu = memnew(MenuButton);
	file_menu->set_text(TTR("File"));
	file_menu->set_shortcut_context(main_split);
	file_menu->get_popup()->add_item(TTR("New FlowScript..."), FILE_NEW);
	file_menu->get_popup()->add_separator();
	file_menu->get_popup()->add_item(TTR("Load FlowScript File..."), FILE_OPEN);
	file_menu->get_popup()->add_shortcut(ED_SHORTCUT("flow_script_editor/save", TTR("Save File"), KeyModifierMask::ALT | KeyModifierMask::CMD_OR_CTRL | Key::S), FILE_SAVE);
	file_menu->get_popup()->add_shortcut(ED_SHORTCUT("flow_script_editor/save_as", TTR("Save File As...")), FILE_SAVE_AS);
	file_menu->get_popup()->add_separator();
	file_menu->get_popup()->add_item(TTR("Close"), FILE_CLOSE);
	file_menu->get_popup()->add_item(TTR("Close All"), FILE_CLOSE_ALL);
	file_menu->connect(SceneStringName(id_pressed), callable_mp(this, &FlowScriptEditorPlugin::on_file_menu_item_pressed));
	menu_hbox->add_child(file_menu);

	file_menu_update_clickable();

	Control *menu_middle_pad = memnew(Control);
	menu_middle_pad->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	menu_middle_pad->add_child(menu_middle_pad);

	make_floating_button = memnew(ScreenSelect);
	make_floating_button->set_flat(true);
	make_floating_button->connect("request_open_in_screen", callable_mp(this, &FlowScriptEditorPlugin::on_make_floating_button_open_to_screen_request));
	if (!make_floating_button->is_disabled())
	{
		make_floating_button->set_tooltip_text(TTR("Make the FlowScript editor floating."));
	}
	menu_hbox->add_child(make_floating_button);

	script_item_list = memnew(ScriptItemList(this));
	script_item_list->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	// SET_DRAG_FORWARDING_GCD(script_list, FlowScriptEditorPlugin);
	left_vbox->add_child(script_item_list);

	Ref<StyleBoxEmpty> empty_style;
	empty_style.instantiate();

	graph_tab_container = memnew(TabContainer);
	graph_tab_container->set_tabs_visible(false);
	graph_tab_container->add_theme_style_override(SceneStringName(panel), empty_style);
	main_split->add_child(graph_tab_container);

	node_create_dialog = memnew(FlowScriptNodeCreateDialog);
	node_create_dialog->connect("type_chosen", callable_mp(this, &FlowScriptEditorPlugin::on_node_create_dialog_type_chosen));
	add_child(node_create_dialog);

	inspector_plugin.instantiate();
	add_inspector_plugin(inspector_plugin);
}


FlowScriptEditorPlugin::~FlowScriptEditorPlugin()
{
}
