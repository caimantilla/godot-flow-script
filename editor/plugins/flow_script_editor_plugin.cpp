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


void FlowScriptEditorPlugin::NodeConnectionBreakElement::_notification(int p_what)
{
	// REPLACE WITH CUSTOM ICON LATER
	Ref<Texture2D> break_icon = get_theme_icon(SNAME("DebugSkipBreakpointsOn"), EditorStringName(EditorIcons));
	break_rect->set_texture(break_icon);
}


FlowScriptEditorPlugin::NodeConnectionBreakElement::NodeConnectionBreakElement()
{
	set_draggable(false);
	set_selectable(false);
	set_resizable(false);
	set_auto_translate(false);

	break_rect = memnew(TextureRect);
	break_rect->set_mouse_filter(MOUSE_FILTER_IGNORE);
	break_rect->set_focus_mode(FOCUS_NONE);
	add_child(break_rect);
}


void FlowScriptEditorPlugin::IncludeFrame::_notification(int p_what)
{
	if (p_what == NOTIFICATION_THEME_CHANGED)
	{
		Ref<Texture2D> edit_icon = get_theme_icon(SNAME("Edit"), EditorStringName(EditorIcons));
		Ref<Texture2D> delete_icon = get_theme_icon(SNAME("Remove"), EditorStringName(EditorIcons));
		edit_button->set_icon(edit_icon);
		delete_button->set_icon(delete_icon);
	}
}


void FlowScriptEditorPlugin::IncludeFrame::on_edit_button_pressed()
{
	EditorInterface::get_singleton()->edit_resource(edited_script->flow_script);
}


void FlowScriptEditorPlugin::IncludeFrame::on_delete_button_pressed()
{
	emit_signal(SNAME("delete_request"));
}


FlowScriptEditorPlugin::IncludeFrame::IncludeFrame(EditedScript *p_edited_script)
{
	edited_script = p_edited_script;
	DEV_ASSERT(!edited_script->is_root());

	edit_button = memnew(Button);
	edit_button->connect(SceneStringName(pressed), callable_mp(this, &IncludeFrame::on_edit_button_pressed));
	get_titlebar_hbox()->add_child(edit_button);

	delete_button = memnew(Button);
	delete_button->set_visible(edited_script->is_first_level_include());
	delete_button->connect(SceneStringName(pressed), callable_mp(this, &IncludeFrame::on_delete_button_pressed));
	get_titlebar_hbox()->add_child(delete_button);
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


void FlowScriptEditorPlugin::ScriptGraph::add_include_frame(IncludeFrame *p_frame)
{
	p_frame->set_theme(msdf_theme);
	add_child(p_frame);
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


FlowScriptEditorPlugin::ScriptGraph::ScriptGraph(FlowScriptEditorPlugin *p_plugin)
{
	plugin = p_plugin;
	msdf_theme.instantiate();

	set_h_size_flags(Control::SIZE_EXPAND_FILL);
	set_v_size_flags(Control::SIZE_EXPAND_FILL);

	set_auto_translate(false);
	set_snapping_enabled(false);
	set_show_arrange_button(false);
	set_grid_pattern(GRID_PATTERN_DOTS);
}


FlowScriptEditorPlugin::EditedScript::GraphItem FlowScriptEditorPlugin::EditedScript::GraphItem::create_node(const FlowScriptNodeID p_node_id)
{
	return create_node(FlowScriptNodeReference(p_node_id, FlowScript::NODE_ID_INVALID));
}


FlowScriptEditorPlugin::EditedScript::GraphItem FlowScriptEditorPlugin::EditedScript::GraphItem::create_node(const FlowScriptNodeReference p_node_reference)
{
	GraphItem ret;
	ret.type = TYPE_NODE;
	ret.node = p_node_reference;
	return ret;
}


FlowScriptEditorPlugin::EditedScript::GraphItem FlowScriptEditorPlugin::EditedScript::GraphItem::create_include(const FlowScriptIncludeID p_include_id)
{
	GraphItem ret;
	ret.type = TYPE_INCLUDE;
	ret.include_id = p_include_id;
	return ret;
}


bool FlowScriptEditorPlugin::EditedScript::is_root() const
{
	return parent == nullptr;
}


bool FlowScriptEditorPlugin::EditedScript::is_first_level_include() const
{
	return parent != nullptr && parent->is_root();
}


FlowScriptEditorPlugin::EditedScript *FlowScriptEditorPlugin::EditedScript::get_root()
{
	EditedScript *p = this;
	while (p->parent != nullptr)
	{
		p = p->parent;
	}
	return p;
}


void FlowScriptEditorPlugin::EditedScript::copy_selected_nodes()
{
	List<const FlowScriptNodeEditor *> selected_editor_list;
	for (const GraphItem &item : selected_item_set)
	{
		if (item.type == GraphItem::TYPE_NODE)
		{
			const FlowScriptNodeEditor *editor = get_node_editor_by_reference(item.node);
			if (editor != nullptr)
			{
				selected_editor_list.push_back(editor);
			}
		}
	}
	if (!selected_editor_list.is_empty())
	{
		plugin->clipboard_handler.copy_nodes(this, selected_editor_list);
	}
}


void FlowScriptEditorPlugin::EditedScript::connect_graph()
{
	graph->connect(SceneStringName(visibility_changed), callable_mp(this, &EditedScript::on_graph_visibility_changed));
	graph->connect(SNAME("popup_request"), callable_mp(this, &EditedScript::on_graph_popup_request));
	graph->connect(SNAME("scroll_offset_changed"), callable_mp(this, &EditedScript::on_graph_scroll_offset_changed));
	graph->connect(SNAME("frame_rect_changed"), callable_mp(this, &EditedScript::on_graph_frame_rect_changed));
	graph->connect(SNAME("copy_nodes_request"), callable_mp(this, &EditedScript::copy_selected_nodes));
	graph->connect(SNAME("pase_nodes_request"), callable_mp(this, &EditedScript::on_graph_paste_nodes_request));
	graph->connect(SNAME("duplicate_nodes_request"), callable_mp(this, &EditedScript::on_graph_duplicate_nodes_request));
	graph->connect(SNAME("node_selected"), callable_mp(this, &EditedScript::on_graph_node_selected));
	graph->connect(SNAME("node_deselected"), callable_mp(this, &EditedScript::on_graph_node_deselected));
	graph->connect(SNAME("connection_drag_started"), callable_mp(this, &EditedScript::on_graph_connection_drag_started));
	graph->connect(SNAME("connection_drag_ended"), callable_mp(this, &EditedScript::on_graph_connection_drag_ended));
	graph->connect(SNAME("connection_request"), callable_mp(this, &EditedScript::on_graph_connection_request));
	graph->connect(SNAME("connection_from_empty"), callable_mp(this, &EditedScript::on_graph_connection_from_empty));
	graph->connect(SNAME("connection_to_empty"), callable_mp(this, &EditedScript::on_graph_connection_to_empty));
	graph->connect(SNAME("disconnection_request"), callable_mp(this, &EditedScript::on_graph_disconnection_request));
}


void FlowScriptEditorPlugin::EditedScript::connect_flow_script()
{
	flow_script->connect_changed(callable_mp(this, &EditedScript::on_script_changed));
	flow_script->connect(SNAME("node_added"), callable_mp(this, &EditedScript::on_script_node_added));
	flow_script->connect(SNAME("node_removed"), callable_mp(this, &EditedScript::on_script_node_removed));
	flow_script->connect(SNAME("node_position_changed"), callable_mp(this, &EditedScript::on_script_node_position_changed));
	flow_script->connect(SNAME("node_connection_changed"), callable_mp(this, &EditedScript::on_script_node_connection_changed));
	flow_script->connect(SNAME("include_added"), callable_mp(this, &EditedScript::on_script_include_added));
	flow_script->connect(SNAME("include_removed"), callable_mp(this, &EditedScript::on_script_include_removed));
	flow_script->connect(SNAME("include_position_changed"), callable_mp(this, &EditedScript::on_script_include_position_changed));
}


const FlowScriptEditorPlugin::EditedScript::GraphItem FlowScriptEditorPlugin::EditedScript::get_graph_node_as_item_by_ptr(Node *node) const
{
	ERR_FAIL_NULL_V(node, GraphItem());
	FlowScriptNodeEditor *editor = Object::cast_to<FlowScriptNodeEditor>(node);
	if (editor != nullptr)
	{
		if (editor->edited_flow_script == flow_script.ptr())
		{
			return GraphItem::create_node(editor->edited_node_id);
		}
		else if (editor->parent_flow_script == flow_script.ptr())
		{
			return GraphItem::create_node(FlowScriptNodeReference(editor->edited_node_id, editor->edited_include_id));
		}
		else
		{
			ERR_FAIL_V(GraphItem());
		}
	}
	IncludeFrame *frame = Object::cast_to<IncludeFrame>(node);
	if (frame != nullptr)
	{
		return GraphItem::create_include(frame->edited_script->include_id);
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
			return flow_script->get_node_position(p_item.node.node_id);
		case GraphItem::TYPE_INCLUDE:
			return flow_script->get_include_flow_script_position(p_item.include_id);
		default:
			ERR_FAIL_V(Point2i());
	}
}


void FlowScriptEditorPlugin::EditedScript::on_node_changed(const FlowScriptNodeID p_node_id)
{
	ReflectOperation op;
	op.type = ReflectOperation::TYPE_ITEM_SYNC;
	op.data.item_sync.item = GraphItem::create_node(p_node_id);
	queue_reflect_operation(op);
}


void FlowScriptEditorPlugin::EditedScript::on_node_resized(const FlowScriptNodeID p_node_id)
{
	ReflectOperation op;
	op.type = ReflectOperation::TYPE_ITEM_SYNC;
	op.data.item_sync.item = GraphItem::create_node(p_node_id);
	queue_reflect_operation(op);
}


void FlowScriptEditorPlugin::EditedScript::on_node_dragged(const Point2 &p_from, const Point2 &p_to, const FlowScriptNodeID p_node_id)
{
	MutateOperation op;
	op.type = MutateOperation::TYPE_ITEM_DRAG;
	op.data.item_drag.from = p_from;
	op.data.item_drag.to = p_to;
	op.data.item_drag.item = GraphItem::create_node(p_node_id);
	queue_mutate_operation(op);
}


void FlowScriptEditorPlugin::EditedScript::on_node_selected(const FlowScriptNodeID p_node_id)
{
	selected_item_set.insert(GraphItem::create_node(p_node_id));
}


void FlowScriptEditorPlugin::EditedScript::on_node_deselected(const FlowScriptNodeID p_node_id)
{
	selected_item_set.erase(GraphItem::create_node(p_node_id));
}


void FlowScriptEditorPlugin::EditedScript::on_node_raise_request(const FlowScriptNodeID p_node_id)
{
	EditorInterface::get_singleton()->edit_resource(node_editor_map[p_node_id]->get_edited_node_ref());
}


void FlowScriptEditorPlugin::EditedScript::on_node_delete_request(const FlowScriptNodeID p_node_id)
{
	MutateOperation op;
	op.type = MutateOperation::TYPE_ITEM_DELETE;
	op.data.item_delete.item = GraphItem::create_node(p_node_id);
	queue_mutate_operation(op);
}


void FlowScriptEditorPlugin::EditedScript::on_include_changed(const FlowScriptIncludeID p_include_id)
{
	ReflectOperation op;
	op.type = ReflectOperation::TYPE_ITEM_SYNC;
	op.data.item_sync.item = GraphItem::create_include(p_include_id);
	queue_reflect_operation(op);
}


void FlowScriptEditorPlugin::EditedScript::on_include_resized(const FlowScriptIncludeID p_include_id)
{
	ReflectOperation op;
	op.type = ReflectOperation::TYPE_ITEM_SYNC;
	op.data.item_sync.item = GraphItem::create_include(p_include_id);
	queue_reflect_operation(op);
}


void FlowScriptEditorPlugin::EditedScript::on_include_dragged(const Point2 &p_from, const Point2 &p_to, const FlowScriptIncludeID p_include_id)
{
	MutateOperation op;
	op.type = MutateOperation::TYPE_ITEM_DRAG;
	op.data.item_drag.from = p_from;
	op.data.item_drag.to = p_to;
	op.data.item_drag.item = GraphItem::create_include(p_include_id);
	queue_mutate_operation(op);
}


void FlowScriptEditorPlugin::EditedScript::on_include_selected(const FlowScriptIncludeID p_include_id)
{
	selected_item_set.insert(GraphItem::create_include(p_include_id));
}


void FlowScriptEditorPlugin::EditedScript::on_include_deselected(const FlowScriptIncludeID p_include_id)
{
	selected_item_set.erase(GraphItem::create_include(p_include_id));
}


void FlowScriptEditorPlugin::EditedScript::on_include_delete_request(const FlowScriptIncludeID p_include_id)
{
	MutateOperation op;
	op.type = MutateOperation::TYPE_ITEM_DELETE;
	op.data.item_delete.item = GraphItem::create_include(p_include_id);
	queue_mutate_operation(op);
}


FlowScriptNodeEditor *FlowScriptEditorPlugin::EditedScript::get_node_editor_by_reference(const FlowScriptNodeReference &p_reference) const
{
	if (p_reference.flow_script_id == FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID)
	{
		ERR_FAIL_COND_V(!node_editor_map.has(p_reference.node_id), nullptr);
		return node_editor_map[p_reference.node_id];
	}
	else
	{
		ERR_FAIL_INDEX_V(p_reference.flow_script_id, FlowScript::INCLUDE_FLOW_SCRIPT_MAX, nullptr);
		ERR_FAIL_COND_V(children[p_reference.flow_script_id] == nullptr, nullptr);
		ERR_FAIL_COND_V(!children[p_reference.flow_script_id]->node_editor_map.has(p_reference.node_id), nullptr);
		return children[p_reference.flow_script_id]->node_editor_map[p_reference.node_id];
	}
}


void FlowScriptEditorPlugin::EditedScript::on_graph_connection_request(const StringName &p_from_node_name, const int p_from_port, const StringName &p_to_node_name, const int p_to_port)
{
	FlowScriptNodeEditor *from_editor = Object::cast_to<FlowScriptNodeEditor>(graph->get_node(String(p_from_node_name)));
	FlowScriptNodeEditor *to_editor = Object::cast_to<FlowScriptNodeEditor>(graph->get_node(String(p_to_node_name)));
	if (from_editor == nullptr || to_editor == nullptr)
	{
		return;
	}
	// only allow connections FROM the root script
	if (from_editor->edited_flow_script != flow_script.ptr())
	{
		return;
	}
	// and only allow connections TO nodes of the root script or immediate includes
	if (to_editor->edited_flow_script != flow_script.ptr() && to_editor->parent_flow_script != flow_script.ptr())
	{
		return;
	}
	MutateOperation op;
	op.type = MutateOperation::TYPE_NODE_CONNECT;
	op.data.node_connect.from_node_id = from_editor->edited_node_id;
	op.data.node_connect.from_node_output = from_editor->output_graph_slot_to_connection(from_editor->get_slot_port_index_right(p_from_port));
	op.data.node_connect.to_node = FlowScriptNodeReference(to_editor->edited_node_id, to_editor->edited_include_id);
	queue_mutate_operation(op);
}


void FlowScriptEditorPlugin::EditedScript::queue_handle_buffers()
{
	if (buffers_dirty)
	{
		return;
	}
	buffers_dirty = true;
	if (parent == nullptr)
	{
		callable_mp(this, &EditedScript::root_init_buffer_handle).call_deferred();
		// if (graph->is_visible_in_tree())
		// {
		// 	callable_mp(this, &EditedScript::root_init_buffer_handle).call_deferred();
		// }
	}
	else
	{
		parent->queue_handle_buffers();
	}
}


void FlowScriptEditorPlugin::EditedScript::queue_mutate_operation(const MutateOperation &p_operation)
{
	buffer_mutate.push_back(p_operation);
	queue_handle_buffers();
}


void FlowScriptEditorPlugin::EditedScript::queue_reflect_operation(const ReflectOperation &p_operation)
{
	buffer_reflect.push_back(p_operation);
	queue_handle_buffers();
}


void FlowScriptEditorPlugin::EditedScript::root_handle_mutate_buffer_recursive(EditedScript *p_current_level)
{
	for (FlowScriptIncludeID i = 0; i < FlowScript::INCLUDE_FLOW_SCRIPT_MAX; i++)
	{
		if (p_current_level->children[i] != nullptr && p_current_level->children[i]->buffers_dirty)
		{
			root_handle_mutate_buffer_recursive(p_current_level->children[i]);
		}
	}
	p_current_level->handle_mutate_buffer();
}


void FlowScriptEditorPlugin::EditedScript::root_handle_reflect_buffer_early_recursive(EditedScript *p_current_level)
{
	p_current_level->handle_reflect_buffer_early();
	for (FlowScriptIncludeID i = 0; i < FlowScript::INCLUDE_FLOW_SCRIPT_MAX; i++)
	{
		if (p_current_level->children[i] != nullptr && p_current_level->children[i]->buffers_dirty)
		{
			root_handle_reflect_buffer_early_recursive(p_current_level->children[i]);
		}
	}
}


void FlowScriptEditorPlugin::EditedScript::root_handle_reflect_buffer_late_recursive(EditedScript *p_current_level)
{
	for (FlowScriptIncludeID i = 0; i < FlowScript::INCLUDE_FLOW_SCRIPT_MAX; i++)
	{
		if (p_current_level->children[i] != nullptr && p_current_level->children[i]->buffers_dirty)
		{
			root_handle_reflect_buffer_late_recursive(p_current_level->children[i]);
		}
	}
	p_current_level->handle_reflect_buffer_late();
}


void FlowScriptEditorPlugin::EditedScript::root_make_buffer_clean_recursive(EditedScript *p_current_level)
{
	for (FlowScriptIncludeID i = 0; i < FlowScript::INCLUDE_FLOW_SCRIPT_MAX; i++)
	{
		if (p_current_level->children[i] != nullptr && p_current_level->children[i]->buffers_dirty)
		{
			root_make_buffer_clean_recursive(p_current_level->children[i]);
		}
	}
	p_current_level->buffers_dirty = false;
	p_current_level->buffer_mutate.clear();
	p_current_level->buffer_reflect.clear();
}


void FlowScriptEditorPlugin::EditedScript::root_init_buffer_handle()
{
	root_handle_mutate_buffer_recursive(this);
	root_handle_reflect_buffer_early_recursive(this);
	root_handle_reflect_buffer_late_recursive(this);
	root_make_buffer_clean_recursive(this);
}


void FlowScriptEditorPlugin::EditedScript::handle_buffers()
{
	DEV_ASSERT(is_root());
	// handle children first as we may rely on the presence of items of the children
	for (FlowScriptIncludeID i = 0; i < FlowScript::INCLUDE_FLOW_SCRIPT_MAX; i++)
	{
		if (children[i] != nullptr && children[i]->buffers_dirty)
		{
			children[i]->handle_mutate_buffer();
		}
	}

	handle_mutate_buffer();
	handle_reflect_buffer();

	buffers_dirty = false;
}


void FlowScriptEditorPlugin::EditedScript::handle_mutate_buffer()
{
	if (buffer_mutate.size() == 0)
	{
		return;
	}

	struct GroupedOperationList
	{
		MutateOperation::Type type;
		List<const int> operation_indices;
	};

	List<const GroupedOperationList> operation_lists;

	for (int i = 0; i < buffer_mutate.size(); i++)
	{
		GroupedOperationList op_list = { .type = buffer_mutate[i].type };
		while (buffer_mutate[i].type == op_list.type && i < buffer_mutate.size())
		{
			op_list.operation_indices.push_back(i);
			i++;
		}
		operation_lists.push_back(op_list);
	}

	for (const GroupedOperationList &op_list : operation_lists)
	{
		switch (op_list.type)
		{
			case MutateOperation::TYPE_ITEM_DELETE: {
				Vector<MutateOperation::ItemDelete> delete_list;
				int i = 0;
				for (int op_idx : op_list.operation_indices)
				{
					delete_list.write[i] = buffer_mutate[op_idx].data.item_delete;
					i++;
				}
				handle_mutate_operation_list_item_delete(delete_list);
			} break;
			case MutateOperation::TYPE_ITEM_DRAG: {
				Vector<MutateOperation::ItemDrag> drag_list;
				int i = 0;
				for (int op_idx : op_list.operation_indices)
				{
					drag_list.write[i] = buffer_mutate[op_idx].data.item_drag;
					i++;
				}
				handle_mutate_operation_list_item_drag(drag_list);
			} break;
			case MutateOperation::TYPE_NODE_ADD: {
				Vector<MutateOperation::NodeAdd> add_list;
				int i = 0;
				for (int op_idx : op_list.operation_indices)
				{
					add_list.write[i] = buffer_mutate[op_idx].data.node_add;
					i++;
				}
				handle_mutate_operation_list_node_add(add_list);
			} break;
		}
	}

	buffer_mutate.clear();
}


void FlowScriptEditorPlugin::EditedScript::handle_reflect_buffer_early()
{
	if (buffer_reflect.size() == 0)
	{
		return;
	}

	for (int i = 0; i < buffer_reflect.size(); i++)
	{
		const ReflectOperation &op = buffer_reflect[i];
		switch (op.type)
		{
			case ReflectOperation::TYPE_ITEM_ADD: {
				reflect_item_add(op.data.item_add);
			} break;
			case ReflectOperation::TYPE_ITEM_SYNC: {
				reflect_item_sync(op.data.item_sync);
			} break;
			case ReflectOperation::TYPE_NODE_DISCONNECT: {
				reflect_node_disconnect(op.data.node_disconnect);
			} break;
		}
	}
}


void FlowScriptEditorPlugin::EditedScript::handle_reflect_buffer_late()
{
	if (buffer_reflect.size() == 0)
	{
		return;
	}

	for (int i = 0; i < buffer_reflect.size(); i++)
	{
		const ReflectOperation &op = buffer_reflect[i];
		switch (op.type)
		{
			case ReflectOperation::TYPE_ITEM_DELETE: {
				reflect_item_delete(op.data.item_delete);
			} break;
			case ReflectOperation::TYPE_NODE_CONNECT: {
				reflect_node_connect(op.data.node_connect);
			} break;
		}
	}
}


void FlowScriptEditorPlugin::EditedScript::reflect_item_add(const ReflectOperation::ItemAdd &op)
{
	switch (op.item.type)
	{
		case GraphItem::TYPE_NODE: {
			FlowScriptNodeID node_id = op.item.node.node_id;

			ERR_FAIL_COND(node_editor_map.has(node_id));
			ERR_FAIL_COND(!flow_script->has_node(node_id));

			FlowScriptNode *node_ptr = flow_script->get_node_ptr(node_id);
			FlowScriptNodeEditor *node_editor = FlowScriptNodeTypeDB::get_singleton()->create_editor_for_node(node_ptr);
			node_editor_map[node_id] = node_editor;

			node_editor->edited_flow_script = flow_script.ptr();
			node_editor->root_flow_script = get_root()->flow_script.ptr();
			if (parent != nullptr)
			{
				node_editor->parent_flow_script = parent->flow_script.ptr();
			}
			node_editor->edited_include_id = include_id;
			node_editor->edited_node_id = node_id;

			node_ptr->connect_changed(callable_mp(this, &EditedScript::on_node_changed).bind(node_id));
			node_editor->connect(SceneStringName(resized), callable_mp(this, &EditedScript::on_node_resized).bind(node_id));

			node_editor->set_resizable(false);

			if (is_root())
			{
				const FlowScriptNodeTypeInfo &type_info = FlowScriptNodeTypeDB::get_singleton()->get_type_of_node(node_ptr);
				node_editor->rename_button->set_visible(type_info.enabled && type_info.name_assignable);
				node_editor->delete_button->show();
				node_editor->set_mouse_filter(Control::MOUSE_FILTER_STOP);
				node_editor->set_draggable(true);
				node_editor->set_selectable(true);
				// signals that should be connected if the node is part of the root script
				node_editor->connect(SNAME("dragged"), callable_mp(this, &EditedScript::on_node_dragged).bind(node_id));
				node_editor->connect(SNAME("raise_request"), callable_mp(this, &EditedScript::on_node_raise_request).bind(node_id));
				node_editor->connect(SNAME("delete_request"), callable_mp(this, &EditedScript::on_node_delete_request).bind(node_id));
				node_editor->connect(SNAME("node_selected"), callable_mp(this, &EditedScript::on_node_selected).bind(node_id));
				node_editor->connect(SNAME("node_deselected"), callable_mp(this, &EditedScript::on_node_deselected).bind(node_id));
			}
			else
			{
				node_editor->rename_button->hide();
				node_editor->delete_button->hide();
				node_editor->set_mouse_filter(Control::MOUSE_FILTER_IGNORE);
				node_editor->set_draggable(false);
				node_editor->set_selectable(false);
			}

			graph->add_node_editor(node_editor);
			if (frame != nullptr)
			{
				graph->attach_node_editor_to_include_frame(node_editor, frame);
			}
			node_editor->startup();

			ReflectOperation::ItemSync sync_op;
			sync_op.item = op.item;
			reflect_item_sync(sync_op);
		} break;
		case GraphItem::TYPE_INCLUDE: {
			FlowScriptIncludeID include_id = op.item.include_id;

			ERR_FAIL_INDEX(include_id, FlowScript::INCLUDE_FLOW_SCRIPT_MAX);
			ERR_FAIL_COND(!flow_script->has_include_flow_script_instance(include_id));
			ERR_FAIL_COND(children[include_id] != nullptr);

			EditedScript *child_script = EditedScript::create_include_edited_script(plugin, this, include_id);
			children[include_id] = child_script;
		} break;
	}
}


void FlowScriptEditorPlugin::EditedScript::reflect_item_delete(const ReflectOperation::ItemDelete &op)
{
	switch (op.item.type)
	{
		case GraphItem::TYPE_NODE: {
			FlowScriptNodeID node_id = op.item.node.node_id;

			ERR_FAIL_COND(!node_editor_map.has(node_id));

			FlowScriptNodeEditor *node_editor = node_editor_map[node_id];
			node_editor->cleanup();
			node_editor->queue_free();
		} break;
		case GraphItem::TYPE_INCLUDE: {
			FlowScriptIncludeID include_id = op.item.include_id;

			ERR_FAIL_INDEX(include_id, FlowScript::INCLUDE_FLOW_SCRIPT_MAX);
			ERR_FAIL_COND(children[include_id] == nullptr);

			EditedScript *child_script = children[include_id];
			memdelete(child_script);
			children[include_id] = nullptr;
		} break;
	}
}


void FlowScriptEditorPlugin::EditedScript::reflect_item_sync(const ReflectOperation::ItemSync &op)
{
	switch (op.item.type)
	{
		case GraphItem::TYPE_NODE: {
			FlowScriptNodeID node_id = op.item.node.node_id;

			ERR_FAIL_COND(!flow_script->has_node(node_id));
			ERR_FAIL_COND(!node_editor_map.has(node_id));

			FlowScriptNodeEditor *node_editor = node_editor_map[node_id];
			node_editor->set_title(node_editor->get_new_title());
			node_editor->set_tooltip_text(node_editor->get_new_tooltip_text());
			node_editor->sync();

			Point2i posi = flow_script->get_node_position(node_id);
			EditedScript *p = this;
			while (p->parent != nullptr)
			{
				posi += p->parent->flow_script->get_include_flow_script_position(p->include_id);
				p = p->parent;
			}
			Point2 posf = graph->point_convert_data_to_graph(posi);
			posf -= node_editor->get_size() * 0.5;
			node_editor->set_position_offset(posf);
		} break;
		case GraphItem::TYPE_INCLUDE: {
			FlowScriptIncludeID include_id = op.item.include_id;

			ERR_FAIL_INDEX(include_id, FlowScript::INCLUDE_FLOW_SCRIPT_MAX);
			ERR_FAIL_COND(!flow_script->has_include_flow_script_instance(include_id));
			ERR_FAIL_COND(children[include_id] == nullptr);

			IncludeFrame *child_frame = children[include_id]->frame;
			child_frame->set_title(vformat("%d. %s", include_id, children[include_id]->flow_script->get_path()));

			Point2i posi = flow_script->get_include_flow_script_position(include_id);
			EditedScript *p = this;
			while (p->parent != nullptr)
			{
				posi += p->parent->flow_script->get_include_flow_script_position(p->include_id);
				p = p->parent;
			}
			Point2 posf = graph->point_convert_data_to_graph(posi);
			posf -= child_frame->get_size() * 0.5;
			child_frame->set_position_offset(posf);
		} break;
	}
}


void FlowScriptEditorPlugin::EditedScript::reflect_node_connect(const ReflectOperation::NodeConnect &op)
{
	EditorNodeConnectionKey key = EditorNodeConnectionKey(op.node_id, op.output);
	if (editor_node_connection_map.has(key))
	{
		ReflectOperation::NodeDisconnect disconnect_op;
		disconnect_op.node_id = op.node_id;
		disconnect_op.output = op.output;
		reflect_node_disconnect(disconnect_op);
	}
	else
	{
		editor_node_connection_map.insert(key, EditorNodeConnectionData());
	}
}


void FlowScriptEditorPlugin::EditedScript::handle_mutate_operation_list_item_delete(const Vector<MutateOperation::ItemDelete> &p_deletes)
{
	if (p_deletes.is_empty())
	{
		return;
	}
	int deleted_type_flags = 0;
	for (const MutateOperation::ItemDelete &del : p_deletes)
	{
		deleted_type_flags |= del.item.type;
	}
	String action_name;
	switch (deleted_type_flags)
	{
		case GraphItem::TYPE_NODE | GraphItem::TYPE_INCLUDE: {
			action_name = TTRN("Delete FlowScript Item", "Delete FlowScript Items", p_deletes.size());
		} break;
		case GraphItem::TYPE_NODE: {
			action_name = TTRN("Delete FlowScript Node", "Delete FlowScript Nodes", p_deletes.size());
		} break;
		case GraphItem::TYPE_INCLUDE: {
			action_name = TTRN("Delete FlowScript Include", "Delete FlowScript Includes", p_deletes.size());
		} break;
	}

	EditorUndoRedoManager *udrd = EditorUndoRedoManager::get_singleton();
	udrd->create_action(action_name, UndoRedo::MERGE_DISABLE);

	PackedInt32Array remove_node_id_list;
	PackedInt32Array remove_include_id_list;

	for (const MutateOperation::ItemDelete &del : p_deletes)
	{
		switch (del.item.type)
		{
			case GraphItem::TYPE_NODE: {
				DEV_ASSERT(del.item.node.flow_script_id == FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID);
				remove_node_id_list.push_back(del.item.node.node_id);
			} break;
			case GraphItem::TYPE_INCLUDE: {
				remove_include_id_list.push_back(del.item.include_id);
			} break;
		}
	}

	// for doing deletion, delete the nodes first, then the includes.
	udrd->add_do_method(flow_script.ptr(), SNAME("remove_node_list"), remove_node_id_list);
	udrd->add_do_method(flow_script.ptr(), SNAME("remove_include_list"), remove_include_id_list);

	// for undoing deletion, restore the includes first, then the nodes.
	for (const MutateOperation::ItemDelete &del : p_deletes)
	{
		if (del.item.type == GraphItem::TYPE_INCLUDE)
		{
			udrd->add_undo_method(flow_script.ptr(), SNAME("set_include_flow_script"), del.item.include_id, flow_script->get_include_flow_script(del.item.include_id));
		}
	}
	for (const MutateOperation::ItemDelete &del : p_deletes)
	{
		if (del.item.type == GraphItem::TYPE_NODE)
		{
			udrd->add_undo_method(flow_script.ptr(), SNAME("set_node"), del.item.node.node_id, flow_script->get_node_ref(del.item.node.node_id));
		}
	}

	udrd->commit_action(true);
}


void FlowScriptEditorPlugin::EditedScript::handle_mutate_operation_list_item_drag(const Vector<MutateOperation::ItemDrag> &p_drags)
{
	if (p_drags.is_empty())
	{
		return;
	}
	int dragged_type_flags = 0;
	for (const MutateOperation::ItemDrag &drag : p_drags)
	{
		dragged_type_flags |= drag.item.type;
	}
	String action_name;
	switch (dragged_type_flags)
	{
		case GraphItem::TYPE_NODE | GraphItem::TYPE_INCLUDE: {
			action_name = TTRN("Move FlowScript Item", "Move FlowScript Items", p_drags.size());
		} break;
		case GraphItem::TYPE_NODE: {
			action_name = TTRN("Move FlowScript Node", "Move FlowScript Nodes", p_drags.size());
		} break;
		case GraphItem::TYPE_INCLUDE: {
			action_name = TTRN("Move FlowScript Include", "Move FlowScript Includes", p_drags.size());
		} break;
	}

	EditorUndoRedoManager *udrd = EditorUndoRedoManager::get_singleton();
	udrd->create_action(action_name, UndoRedo::MERGE_DISABLE);

	for (const MutateOperation::ItemDrag &drag : p_drags)
	{
		switch (drag.item.type)
		{
			case GraphItem::TYPE_NODE: {
				ERR_BREAK_EDMSG(drag.item.node.flow_script_id != FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID, TTR("Dragging nodes from include scripts is not allowed."));
				ERR_BREAK(!node_editor_map.has(drag.item.node.node_id));

				const FlowScriptNodeEditor *node_editor = node_editor_map[drag.item.node.node_id];

				Point2i from = graph->point_convert_graph_to_data(drag.from - (node_editor->get_size() * 0.5));
				Point2i to = graph->point_convert_graph_to_data(drag.to - (node_editor->get_size() * 0.5));

				udrd->add_do_method(flow_script.ptr(), SNAME("set_node_position"), drag.item.node.node_id, to);
				udrd->add_undo_method(flow_script.ptr(), SNAME("set_node_position"), drag.item.node.node_id, from);
			} break;
			case GraphItem::TYPE_INCLUDE: {
				DEV_ASSERT(children[drag.item.include_id] != nullptr);
				IncludeFrame *child_frame = children[drag.item.include_id]->frame;
				DEV_ASSERT(child_frame != nullptr);
				
				Point2i from = graph->point_convert_graph_to_data(child_frame->get_size() * 0.5);
				Point2i to = graph->point_convert_graph_to_data(child_frame->get_size() * 0.5);

				udrd->add_do_method(flow_script.ptr(), SNAME("set_include_position"), drag.item.include_id, to);
				udrd->add_undo_method(flow_script.ptr(), SNAME("set_include_position"), drag.item.include_id, from);
			} break;
		}
	}

	udrd->commit_action(true);
}


void FlowScriptEditorPlugin::EditedScript::handle_mutate_operation_list_node_add(const Vector<MutateOperation::NodeAdd> &p_adds)
{
	if (p_adds.is_empty())
	{
		return;
	}
	EditorUndoRedoManager *udrd = EditorUndoRedoManager::get_singleton();
	udrd->create_action(TTRN("Add FlowScript Node", "Add FlowScript Nodes", p_adds.size()), UndoRedo::MERGE_DISABLE);

	PackedInt32Array remove_ids;

	FlowScriptNodeID curr_node_id = FlowScript::NODE_ID_MIN;
	for (const MutateOperation::NodeAdd &add : p_adds)
	{
		FlowScriptNodeID node_id = flow_script->add_node_to_first_available_slot(add.node);
		ERR_BREAK_EDMSG(node_id == FlowScript::NODE_ID_INVALID, TTR("No more node slots available."));
		Point2i pos = graph->point_convert_graph_to_data(add.position);
		flow_script->set_node_position(node_id, pos);

		udrd->add_do_method(flow_script.ptr(), SNAME("set_node"), node_id, add.node);
		udrd->add_do_method(flow_script.ptr(), SNAME("set_node_position"), node_id, pos);

		remove_ids.push_back(node_id);
	}

	if (!remove_ids.is_empty())
	{
		udrd->add_undo_method(flow_script.ptr(), SNAME("remove_node_list"), remove_ids);
	}

	udrd->commit_action(false); // since getting the id is performed by executing, don't commit the do actions, they've already been performed...
}


void FlowScriptEditorPlugin::EditedScript::handle_mutate_operation_list_node_rename(const Vector<MutateOperation::NodeRename> &p_renames)
{
	if (p_renames.is_empty())
	{
		return;
	}
	EditorUndoRedoManager *udrd = EditorUndoRedoManager::get_singleton();
	udrd->create_action(TTRN("Rename FlowScript Node", "Rename FlowScript Nodes", p_renames.size()), UndoRedo::MERGE_DISABLE);

	for (const MutateOperation::NodeRename &rename : p_renames)
	{
		FlowScriptNode *node = flow_script->get_node_ptr(rename.node_id);
		ERR_CONTINUE(node == nullptr);
		udrd->add_do_method(node, SNAME("set_name"), rename.new_name);
		udrd->add_undo_method(node, SNAME("set_name"), node->get_name());
	}

	udrd->commit_action(true);
}


void FlowScriptEditorPlugin::EditedScript::handle_mutate_operation_list_node_connect(const Vector<MutateOperation::NodeConnect> &p_connections)
{
	if (p_connections.is_empty())
	{
		return;
	}
	EditorUndoRedoManager *udrd = EditorUndoRedoManager::get_singleton();
	udrd->create_action(TTRN("Connect FlowScript Nodes", "Connect Multiple FlowScript Nodes", p_connections.size()), UndoRedo::MERGE_DISABLE);

	for (const MutateOperation::NodeConnect &connection : p_connections)
	{
		FlowScriptNodeReference curr_target = flow_script->get_node_connection(connection.from_node_id, connection.from_node_output);
		udrd->add_do_method(flow_script.ptr(), SNAME("set_node_connection"), connection.from_node_id, connection.from_node_output.list, connection.from_node_output.slot, connection.to_node.node_id, connection.to_node.flow_script_id);
		udrd->add_undo_method(flow_script.ptr(), SNAME("set_node_connection"), connection.from_node_id, connection.from_node_output.list, connection.from_node_output.slot, curr_target.node_id, curr_target.flow_script_id);
	}

	udrd->commit_action(true);
}


void FlowScriptEditorPlugin::EditedScript::handle_mutate_operation_list_node_disconnect(const Vector<MutateOperation::NodeDisconnect> &p_disconnections)
{
	if (p_disconnections.is_empty())
	{
		return;
	}
	EditorUndoRedoManager *udrd = EditorUndoRedoManager::get_singleton();
	udrd->create_action(TTRN("Disconnect FlowScript Nodes", "Disconnect Multiple FlowScript Nodes", p_disconnections.size()));

	for (const MutateOperation::NodeDisconnect &disconnection : p_disconnections)
	{
		FlowScriptNodeReference curr_target = flow_script->get_node_connection(disconnection.from_node_id, disconnection.from_node_output);
		udrd->add_do_method(flow_script.ptr(), SNAME("set_node_connection"), disconnection.from_node_id, disconnection.from_node_output.list, disconnection.from_node_output.slot, FlowScript::NODE_ID_INVALID, FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID);
		udrd->add_undo_method(flow_script.ptr(), SNAME("set_node_connection"), disconnection.from_node_id, disconnection.from_node_output.list, disconnection.from_node_output.slot, curr_target.node_id, curr_target.flow_script_id);
	}

	udrd->commit_action(true);
}


void FlowScriptEditorPlugin::EditedScript::handle_reflect_operation_list_item_sync(const Vector<ReflectOperation::ItemSync> &p_syncs)
{
	if (p_syncs.is_empty())
	{
		return;
	}

	HashSet<GraphItem> handled_item_set;

	// sync include frames first
	for (const ReflectOperation::ItemSync &sync : p_syncs)
	{
		if (handled_item_set.has(sync.item))
		{
			continue;
		}
		if (sync.item.type == GraphItem::TYPE_INCLUDE)
		{
			handled_item_set.insert(sync.item);
			ERR_CONTINUE(!flow_script->has_include_flow_script_instance(sync.item.include_id));

			Point2i posi = flow_script->get_include_flow_script_position(sync.item.include_id);
			EditedScript *current_tier = this;
			while (current_tier->parent != nullptr)
			{
				posi += current_tier->parent->flow_script->get_include_flow_script_position(current_tier->include_id);
				current_tier = current_tier->parent;
			}

			Point2 posf = graph->point_convert_data_to_graph(posi);
			children[sync.item.include_id]->frame->set_position_offset(posf);
		}
	}
	// then sync nodes
	for (const ReflectOperation::ItemSync &sync : p_syncs)
	{
		if (handled_item_set.has(sync.item))
		{
			continue;
		}
		if (sync.item.type == GraphItem::TYPE_NODE)
		{
			handled_item_set.insert(sync.item);
			ERR_CONTINUE(sync.item.node.flow_script_id != FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID);
			ERR_CONTINUE(!flow_script->has_node(sync.item.node.node_id));
			ERR_CONTINUE(!node_editor_map.has(sync.item.node.node_id));

			FlowScriptNodeEditor *node_editor = node_editor_map[sync.item.node.node_id];
			node_editor->set_title(node_editor->get_new_title());
			node_editor->set_tooltip_text(node_editor->get_new_tooltip_text());
			node_editor->sync();

			Point2 posf = graph->point_convert_data_to_graph(flow_script->get_node_position(sync.item.node.node_id));
			posf -= node_editor->get_size() * 0.5;
			if (frame != nullptr)
			{
				posf += frame->get_position_offset();
			}
			node_editor->set_position_offset(posf);
		}
	}
}


void FlowScriptEditorPlugin::EditedScript::handle_reflect_operation_list_item_delete(const Vector<ReflectOperation::ItemDelete> &p_deletes)
{
	if (p_deletes.is_empty())
	{
		return;
	}

	HashSet<GraphItem> deleted_item_set;

	for (const ReflectOperation::ItemDelete &del : p_deletes)
	{
		if (deleted_item_set.has(del.item))
		{
			continue;
		}
		if (del.item.type == GraphItem::TYPE_NODE)
		{
			deleted_item_set.insert(del.item);
			ERR_CONTINUE(del.item.node.flow_script_id != FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID);
			ERR_CONTINUE(!node_editor_map.has(del.item.node.node_id));

			FlowScriptNodeEditor *node_editor = node_editor_map[del.item.node.node_id];
			node_editor_map.erase(del.item.node.node_id);
			node_editor->queue_free();
		}
	}
}


void FlowScriptEditorPlugin::EditedScript::on_graph_visibility_changed()
{
	// if (is_root() && buffers_dirty && graph->is_visible_in_tree())
	// {
	// 	root_init_buffer_handle();
	// }
}


void FlowScriptEditorPlugin::EditedScript::on_graph_connection_drag_started(const StringName &p_from_node_name, const int p_from_port, const bool p_is_output)
{
	GraphItem item = get_graph_node_as_item_by_name(p_from_node_name);
	if (item.type != GraphItem::TYPE_NODE)
	{
		return;
	}
}


void FlowScriptEditorPlugin::EditedScript::on_graph_connection_drag_ended()
{
}


void FlowScriptEditorPlugin::EditedScript::on_graph_connection_from_empty(const StringName &o_to_node_name, const int p_to_port, const Point2 &p_release_position)
{
}


void FlowScriptEditorPlugin::EditedScript::on_graph_connection_to_empty(const StringName &p_from_node_name, const int p_from_port, const Point2 &p_release_position)
{
}


void FlowScriptEditorPlugin::EditedScript::on_script_changed()
{
}


void FlowScriptEditorPlugin::EditedScript::on_script_node_added(const FlowScriptNodeID p_node_id)
{
	ReflectOperation op;
	op.type = ReflectOperation::TYPE_ITEM_ADD;
	op.data.item_add.item = GraphItem::create_node(p_node_id);
	queue_reflect_operation(op);
}


void FlowScriptEditorPlugin::EditedScript::on_script_node_removed(const FlowScriptNodeID p_node_id)
{
	ReflectOperation op;
	op.type = ReflectOperation::TYPE_ITEM_DELETE;
	op.data.item_delete.item = GraphItem::create_node(p_node_id);
	queue_reflect_operation(op);
}


void FlowScriptEditorPlugin::EditedScript::on_script_node_position_changed(const FlowScriptNodeID p_node_id)
{
	ReflectOperation op;
	op.type = ReflectOperation::TYPE_ITEM_SYNC;
	op.data.item_sync.item = GraphItem::create_node(p_node_id);
	queue_reflect_operation(op);
}


void FlowScriptEditorPlugin::EditedScript::on_script_node_connection_changed(const FlowScriptNodeID p_node_id, const uint8_t p_list, const int64_t p_slot)
{
	FlowScriptNodeOutputConnection output = FlowScriptNodeOutputConnection(p_list, p_slot);
	FlowScriptNodeReference new_target = flow_script->get_node_connection(p_node_id, output);
	if (new_target.node_id == FlowScript::NODE_ID_INVALID)
	{
		ReflectOperation op;
		op.type = ReflectOperation::TYPE_NODE_DISCONNECT;
		op.data.node_disconnect.node_id = p_node_id;
		op.data.node_disconnect.output = output;
		queue_reflect_operation(op);
	}
	else
	{
		ReflectOperation op;
		op.type = ReflectOperation::TYPE_NODE_CONNECT;
		op.data.node_connect.node_id = p_node_id;
		op.data.node_connect.output = output;
		queue_reflect_operation(op);
	}
}


void FlowScriptEditorPlugin::EditedScript::on_script_include_added(const FlowScriptIncludeID p_include_id)
{
	ReflectOperation op;
	op.type = ReflectOperation::TYPE_ITEM_ADD;
	op.data.item_add.item = GraphItem::create_include(p_include_id);
	queue_reflect_operation(op);
}


void FlowScriptEditorPlugin::EditedScript::on_script_include_removed(const FlowScriptIncludeID p_include_id)
{
	ReflectOperation op;
	op.type = ReflectOperation::TYPE_ITEM_DELETE;
	op.data.item_delete.item = GraphItem::create_include(p_include_id);
	queue_reflect_operation(op);
}


void FlowScriptEditorPlugin::EditedScript::on_script_include_position_changed(const FlowScriptIncludeID p_include_id)
{
	ReflectOperation op;
	op.type = ReflectOperation::TYPE_ITEM_SYNC;
	op.data.item_sync.item = GraphItem::create_include(p_include_id);
	queue_reflect_operation(op);
}


FlowScriptEditorPlugin::EditedScript *FlowScriptEditorPlugin::EditedScript::create_root_edited_script(FlowScriptEditorPlugin *p_plugin, const Ref<FlowScript> &p_flow_script, ScriptGraph *p_graph)
{
	EditedScript *edited_script = memnew(EditedScript);

	edited_script->plugin = p_plugin;
	edited_script->flow_script = p_flow_script;
	edited_script->graph = p_graph;

	edited_script->connect_graph();
	edited_script->connect_flow_script();

	return edited_script;
}


FlowScriptEditorPlugin::EditedScript *FlowScriptEditorPlugin::EditedScript::create_include_edited_script(FlowScriptEditorPlugin *p_plugin, EditedScript *p_parent, FlowScriptIncludeID p_include_id)
{
	EditedScript *edited_script = memnew(EditedScript);

	edited_script->plugin = p_plugin;
	edited_script->parent = p_parent;
	edited_script->graph = p_parent->graph;
	edited_script->include_id = p_include_id;
	edited_script->flow_script = p_parent->flow_script->get_include_flow_script(p_include_id);

	edited_script->frame = memnew(IncludeFrame(edited_script));
	edited_script->graph->add_include_frame(edited_script->frame);
	edited_script->connect_flow_script();

	return edited_script;
}


FlowScriptEditorPlugin::EditedScript::EditedScript()
{
	// init children memory
	for (FlowScriptIncludeID i = 0; i < FlowScript::INCLUDE_FLOW_SCRIPT_MAX; i++)
	{
		children[i] = nullptr;
	}
}


FlowScriptEditorPlugin::EditedScript::~EditedScript()
{
	for (FlowScriptIncludeID i = 0; i < FlowScript::INCLUDE_FLOW_SCRIPT_MAX; i++)
	{
		if (children[i] != nullptr)
		{
			memdelete(children[i]);
		}
	}
	for (KeyValue<FlowScriptNodeID, FlowScriptNodeEditor *> &E : node_editor_map)
	{
		memdelete(E.value);
	}
	if (frame != nullptr)
	{
		memdelete(frame);
	}
}


void FlowScriptEditorPlugin::ClipboardHandler::copy_nodes(const EditedScript *p_edited_script, const List<const FlowScriptNodeEditor *> p_node_editors)
{
	current_copied_nodes.clear();
	HashMap<FlowScriptNodeID, FlowScriptNodeID> connection_target_remaps;
	{
		FlowScriptNodeID curr_node_copy_id = 0;
		for (const FlowScriptNodeEditor *node_editor : p_node_editors)
		{
			DEV_ASSERT(node_editor != nullptr);
			FlowScriptNodeInstance node_instance = node_editor->edited_flow_script->node_map[node_editor->edited_node_id];
			node_instance.position -= p_edited_script->graph->point_convert_graph_to_data(p_edited_script->graph->get_scroll_offset()); // save relative to screen center
			current_copied_nodes.push_back(node_instance);
			connection_target_remaps.insert(node_editor->edited_node_id, curr_node_copy_id);
			curr_node_copy_id++;
		}
	}
	for (FlowScriptNodeInstance &node_instance : current_copied_nodes)
	{
		for (uint8_t list_idx = 0; list_idx < node_instance.connection_lists.size(); list_idx++)
		{
			for (int64_t slot_idx = 0; slot_idx < node_instance.connection_lists.size(); slot_idx++)
			{
				if (
						node_instance.connection_lists[list_idx][slot_idx].flow_script_id != FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID
						|| !connection_target_remaps.has(node_instance.connection_lists[list_idx][slot_idx].node_id)
				)
				{
					node_instance.connection_lists.write[list_idx].write[slot_idx] = FlowScriptNodeReference();
				}
				else
				{
					node_instance.connection_lists.write[list_idx].write[slot_idx].node_id = connection_target_remaps[node_instance.connection_lists[list_idx][slot_idx].node_id];
				}
			}
		}
	}
}


void FlowScriptEditorPlugin::prompt_script_file_create_new()
{
	current_file_option = FILE_NEW;
	file_dialog->set_file_mode(EditorFileDialog::FILE_MODE_SAVE_FILE);
	file_dialog->popup_file_dialog();
}


void FlowScriptEditorPlugin::prompt_script_file_load_existing()
{
	current_file_option = FILE_OPEN;
	file_dialog->set_file_mode(EditorFileDialog::FILE_MODE_OPEN_FILE);
	file_dialog->popup_file_dialog();
}


void FlowScriptEditorPlugin::prompt_script_file_save_as()
{
	current_file_option = FILE_SAVE_AS;
	file_dialog->set_file_mode(EditorFileDialog::FILE_MODE_SAVE_FILE);
	file_dialog->popup_file_dialog();
}


void FlowScriptEditorPlugin::immediate_script_file_save()
{
}


void FlowScriptEditorPlugin::on_file_menu_item_pressed(int p_idx)
{
	switch (p_idx)
	{
		case FILE_NEW:
			prompt_script_file_create_new();
			break;
		case FILE_OPEN:
			prompt_script_file_load_existing();
			break;
		case FILE_SAVE:
			immediate_script_file_save();
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

	script_item_list = memnew(ItemList);
	script_item_list->set_auto_translate(false);
	script_item_list->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	script_item_list->connect("item_selected", callable_mp(this, &FlowScriptEditorPlugin::on_script_item_list_item_selected));
	script_item_list->connect("item_clicked", callable_mp(this, &FlowScriptEditorPlugin::on_script_item_list_item_clicked));
	// SET_DRAG_FORWARDING_GCD(script_item_list, FlowScriptEditorPlugin);
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

	file_dialog->add_filter("*.tres", TTR("Text FlowScript Resource"));
	file_dialog->add_filter("*.res", TTR("Binary FlowScript Resource"));
}


FlowScriptEditorPlugin::~FlowScriptEditorPlugin()
{
}
