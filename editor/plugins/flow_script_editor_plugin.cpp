#include "flow_script_editor_plugin.hpp"
#include "editor_inspector_plugin_flow_script.hpp"
#include "../../flow_script_node_instance.hpp"
#include "../flow_script_node_type_db.hpp"
#include "../flow_script_node_type_info.hpp"
#include "editor/editor_string_names.h"
#include "editor/editor_node.h"
#include "editor/editor_interface.h"
#include "editor/editor_inspector.h"
#include "editor/editor_settings.h"
#include "editor/editor_command_palette.h"
#include "editor/editor_undo_redo_manager.h"
#include "editor/themes/editor_scale.h"
#include "editor/gui/editor_bottom_panel.h"


void FlowScriptEditorPlugin::ScriptCloseConfirmationDialog::_bind_methods()
{
	ADD_SIGNAL(MethodInfo("act_save"));
	ADD_SIGNAL(MethodInfo("act_discard"));
}


void FlowScriptEditorPlugin::ScriptCloseConfirmationDialog::_notification(int p_what)
{
	if (p_what == NOTIFICATION_POSTINITIALIZE)
	{
		connect(SNAME("confirmed"), callable_mp(this, &ScriptCloseConfirmationDialog::on_confirmed));
		connect(SNAME("custom_action"), callable_mp(this, &ScriptCloseConfirmationDialog::on_custom_action));
	}
}


void FlowScriptEditorPlugin::ScriptCloseConfirmationDialog::set_script_name(const String &p_text)
{
	set_text(vformat(TTR("Close and save changes?\n\"%s\""), p_text));
}


void FlowScriptEditorPlugin::ScriptCloseConfirmationDialog::prompt_action(const String &p_script_name)
{
	set_script_name(p_script_name);
	popup_centered();
}


void FlowScriptEditorPlugin::ScriptCloseConfirmationDialog::on_confirmed()
{
	hide();
	emit_signal(SNAME("act_save"));
}


void FlowScriptEditorPlugin::ScriptCloseConfirmationDialog::on_custom_action(const StringName &p_action_name)
{
	if (p_action_name == SNAME("discard"))
	{
		hide();
		emit_signal(SNAME("act_discard"));
	}
}


FlowScriptEditorPlugin::ScriptCloseConfirmationDialog::ScriptCloseConfirmationDialog()
{
	set_ok_button_text(TTR("Save"));
	add_button(TTR("Discard"), DisplayServer::get_singleton()->get_swap_cancel_ok(), "discard");
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


void FlowScriptEditorPlugin::ScriptGraph::_bind_methods()
{
	ADD_SIGNAL(MethodInfo("flow_script_node_create_prompt_request"));
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
	p_editor->set_h_grow_direction(Control::GROW_DIRECTION_BOTH);
	p_editor->set_v_grow_direction(Control::GROW_DIRECTION_BOTH);
	p_editor->set_h_size_flags(Control::SIZE_SHRINK_CENTER);
	p_editor->set_v_size_flags(Control::SIZE_SHRINK_CENTER);
	add_child(p_editor);
}


void FlowScriptEditorPlugin::ScriptGraph::add_include_frame(IncludeFrame *p_frame)
{
	p_frame->set_theme(msdf_theme);
	p_frame->set_h_grow_direction(Control::GROW_DIRECTION_BOTH);
	p_frame->set_v_grow_direction(Control::GROW_DIRECTION_BOTH);
	p_frame->set_h_size_flags(Control::SIZE_SHRINK_CENTER);
	p_frame->set_v_size_flags(Control::SIZE_SHRINK_CENTER);
	add_child(p_frame);
}


void FlowScriptEditorPlugin::ScriptGraph::attach_node_editor_to_include_frame(FlowScriptNodeEditor *p_editor, IncludeFrame *p_frame)
{
	attach_graph_element_to_frame(p_editor->get_name(), p_frame->get_name());
}


Point2 FlowScriptEditorPlugin::ScriptGraph::point_convert_data_to_graph(const Point2i &p_data_position) const
{
	Point2 ret = p_data_position;
	ret *= EDSCALE;
	return ret;
}


Point2i FlowScriptEditorPlugin::ScriptGraph::point_convert_graph_to_data(const Point2 &p_screen_position) const
{
	Point2 retf = p_screen_position;
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


Point2 FlowScriptEditorPlugin::ScriptGraph::get_visible_center_as_graph_point() const
{
	return get_scroll_offset() + (get_size() * 0.5);
}


Point2i FlowScriptEditorPlugin::ScriptGraph::get_visible_center_as_data_point() const
{
	return point_convert_graph_to_data(get_visible_center_as_graph_point());
}


void FlowScriptEditorPlugin::ScriptGraph::on_add_node_button_pressed()
{
	emit_signal(SNAME("flow_script_node_create_prompt_request"));
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

	add_node_button = memnew(Button);
	add_node_button->set_text(TTR("Add Node..."));
	add_node_button->set_flat(true);
	add_node_button->connect(SceneStringName(pressed), callable_mp(this, &ScriptGraph::on_add_node_button_pressed));
	get_menu_hbox()->add_child(add_node_button);
	get_menu_hbox()->move_child(add_node_button, 0);
}


FlowScriptEditorPlugin::EditedScript::GraphItem FlowScriptEditorPlugin::EditedScript::GraphItem::create_node(const FlowScriptNodeID p_node_id)
{
	return create_node(FlowScriptNodeReference(FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID, p_node_id));
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


const List<FlowScriptNodeEditor *> FlowScriptEditorPlugin::EditedScript::get_selected_node_editors() const
{
	List<FlowScriptNodeEditor *> selected_editor_list;
	if (!is_root())
	{
		return selected_editor_list;
	}
	int child_count = graph->get_child_count(false);
	for (int child_idx = 0; child_idx < child_count; child_idx++)
	{
		Node *child = graph->get_child(child_idx, false);
		FlowScriptNodeEditor *node_editor = Object::cast_to<FlowScriptNodeEditor>(child);
		if (node_editor != nullptr && node_editor->edited_flow_script == flow_script.ptr())
		{
			selected_editor_list.push_back(node_editor);
		}
	}
	return selected_editor_list;
}


void FlowScriptEditorPlugin::EditedScript::copy_selected_nodes()
{
	plugin->clipboard_handler.copy_selected_nodes();
}


void FlowScriptEditorPlugin::EditedScript::connect_graph()
{
	DEV_ASSERT(is_root());

	graph->connect(SceneStringName(visibility_changed), callable_mp(this, &EditedScript::on_graph_visibility_changed));
	graph->connect(SceneStringName(gui_input), callable_mp(this, &EditedScript::on_graph_gui_input));
	graph->connect(SNAME("popup_request"), callable_mp(this, &EditedScript::on_graph_popup_request));
	graph->connect(SNAME("scroll_offset_changed"), callable_mp(this, &EditedScript::on_graph_scroll_offset_changed));
	graph->connect(SNAME("frame_rect_changed"), callable_mp(this, &EditedScript::on_graph_frame_rect_changed));
	graph->connect(SNAME("delete_nodes_request"), callable_mp(this, &EditedScript::on_graph_delete_nodes_request));
	graph->connect(SNAME("copy_nodes_request"), callable_mp(this, &EditedScript::copy_selected_nodes));
	graph->connect(SNAME("paste_nodes_request"), callable_mp(this, &EditedScript::on_graph_paste_nodes_request));
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
	if (is_root())
	{
		flow_script->connect(SNAME("node_added"), callable_mp(this, &EditedScript::on_script_node_added));
		flow_script->connect(SNAME("node_removed"), callable_mp(this, &EditedScript::on_script_node_removed));
		flow_script->connect(SNAME("node_position_changed"), callable_mp(this, &EditedScript::on_script_node_position_changed));
		flow_script->connect(SNAME("node_connection_changed"), callable_mp(this, &EditedScript::on_script_node_connection_changed));
		flow_script->connect(SNAME("include_added"), callable_mp(this, &EditedScript::on_script_include_added));
		flow_script->connect(SNAME("include_removed"), callable_mp(this, &EditedScript::on_script_include_removed));
		flow_script->connect(SNAME("include_position_changed"), callable_mp(this, &EditedScript::on_script_include_position_changed));
	}
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
			return GraphItem::create_node(FlowScriptNodeReference(editor->edited_include_id, editor->edited_node_id));
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
	// ReflectOperation op;
	// op.type = ReflectOperation::TYPE_ITEM_SYNC;
	// op.data.item_sync.item = GraphItem::create_node(p_node_id);
	// queue_reflect_operation(op);
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
	// selected_item_set.insert(GraphItem::create_node(p_node_id));
}


void FlowScriptEditorPlugin::EditedScript::on_node_deselected(const FlowScriptNodeID p_node_id)
{
	// selected_item_set.erase(GraphItem::create_node(p_node_id));
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
	// selected_item_set.insert(GraphItem::create_include(p_include_id));
}


void FlowScriptEditorPlugin::EditedScript::on_include_deselected(const FlowScriptIncludeID p_include_id)
{
	// selected_item_set.erase(GraphItem::create_include(p_include_id));
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
	if (from_editor->edited_flow_script != flow_script)
	{
		return;
	}
	// and only allow connections TO nodes of the root script or immediate includes
	if (to_editor->edited_flow_script != flow_script && to_editor->parent_flow_script != flow_script)
	{
		return;
	}
	MutateOperation op;
	op.type = MutateOperation::TYPE_NODE_CONNECT;
	op.data.node_connect.from_node_id = from_editor->edited_node_id;
	op.data.node_connect.from_node_output = from_editor->output_graph_slot_to_connection(from_editor->get_output_port_slot(p_from_port));
	op.data.node_connect.to_node = FlowScriptNodeReference(to_editor->edited_include_id, to_editor->edited_node_id);
	queue_mutate_operation(op);
}


void FlowScriptEditorPlugin::EditedScript::unload_everything()
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
		E.value->queue_free();
	}
	if (frame != nullptr)
	{
		frame->queue_free();
	}
	node_editor_map.clear();
}


void FlowScriptEditorPlugin::EditedScript::root_unload_everything()
{
	DEV_ASSERT(is_root());
	unload_everything();
}


void FlowScriptEditorPlugin::EditedScript::root_load_everything()
{
	DEV_ASSERT(is_root());
	root_load_everything_recursive(this);
	root_init_draw_connections();
}


void FlowScriptEditorPlugin::EditedScript::root_reload_everything()
{
	DEV_ASSERT(is_root());
	root_unload_everything();
	root_load_everything();
}


void FlowScriptEditorPlugin::EditedScript::root_load_everything_recursive(EditedScript *p_current_level)
{
	for (FlowScriptIncludeID i = 0; i < FlowScript::INCLUDE_FLOW_SCRIPT_MAX; i++)
	{
		if (p_current_level->flow_script->has_include_flow_script_instance(i))
		{
			ReflectOperation::ItemAdd op;
			op.item = GraphItem::create_include(i);
			p_current_level->reflect_item_add(op);
			root_load_everything_recursive(p_current_level->children[i]);
		}
	}
	for (const KeyValue<FlowScriptNodeID, FlowScriptNodeInstance> &kv : p_current_level->flow_script->node_map)
	{
		ReflectOperation::ItemAdd op;
		op.item = GraphItem::create_node(kv.key);
		p_current_level->reflect_item_add(op);
	}
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
	}
	else
	{
		parent->queue_handle_buffers();
	}
}


void FlowScriptEditorPlugin::EditedScript::queue_mutate_operation(const MutateOperation &p_operation)
{
	ERR_FAIL_COND_MSG(!is_root(), TTR("It shouldn't be possible to perform a mutate operation on an include."));
	ERR_FAIL_COND_MSG(!graph->is_visible_in_tree(), TTR("It shouldn't be possible to perform a mutate operation when the script's graph isn't visible."));

	buffer_mutate.push_back(p_operation);
	queue_handle_buffers();
}


void FlowScriptEditorPlugin::EditedScript::queue_reflect_operation(const ReflectOperation &p_operation)
{
	if (!is_root())
	{
		return;
	}
	if (graph->is_visible_in_tree())
	{
		buffer_reflect.push_back(p_operation);
		queue_handle_buffers();
	}
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


void FlowScriptEditorPlugin::EditedScript::root_draw_connections_recursive(EditedScript *p_current_level)
{
	for (FlowScriptIncludeID i = 0; i < FlowScript::INCLUDE_FLOW_SCRIPT_MAX; i++)
	{
		if (p_current_level->children[i] != nullptr)
		{
			root_draw_connections_recursive(p_current_level->children[i]);
		}
	}
	for (const KeyValue<FlowScriptNodeID, FlowScriptNodeEditor *> &kv : p_current_level->node_editor_map)
	{
		ERR_CONTINUE(!p_current_level->flow_script->has_node(kv.key));
		for (uint8_t list_idx = 0; list_idx < p_current_level->flow_script->node_map[kv.key].connection_lists.size(); list_idx++)
		{
			for (int64_t slot_idx = 0; slot_idx < p_current_level->flow_script->node_map[kv.key].connection_lists[list_idx].size(); slot_idx++)
			{
				const FlowScriptNodeReference &target = p_current_level->flow_script->node_map[kv.key].connection_lists[list_idx][slot_idx];
				if (target.node_id != FlowScript::NODE_ID_INVALID)
				{
					FlowScriptNodeEditor *target_editor;
					if (target.flow_script_id == FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID)
					{
						ERR_CONTINUE(!p_current_level->node_editor_map.has(target.node_id));
						target_editor = p_current_level->node_editor_map[target.node_id];
					}
					else
					{
						ERR_CONTINUE(target.flow_script_id < 0 || target.flow_script_id >= FlowScript::INCLUDE_FLOW_SCRIPT_MAX);
						ERR_CONTINUE(p_current_level->children[target.flow_script_id] == nullptr);
						ERR_CONTINUE(!p_current_level->children[target.flow_script_id]->node_editor_map.has(target.node_id));
						target_editor = p_current_level->children[target.flow_script_id]->node_editor_map[target.node_id];
					}

					int output_slot = kv.value->output_connection_to_graph_slot(FlowScriptNodeOutputConnection(list_idx, slot_idx));
					ERR_CONTINUE(!kv.value->is_slot_enabled_right(output_slot));
					int output_port = kv.value->get_slot_port_index_right(output_slot);
					int input_slot = target_editor->get_input_slot();
					ERR_CONTINUE(!target_editor->is_slot_enabled_left(input_slot));
					int input_port = target_editor->get_slot_port_index_left(input_slot);

					p_current_level->graph->connect_node(kv.value->get_name(), output_port, target_editor->get_name(), input_port);
				}
			}
		}
	}
}


void FlowScriptEditorPlugin::EditedScript::root_init_buffer_handle()
{
	DEV_ASSERT(is_root());

	// start by clearing the connections
	graph->clear_connections();

	root_handle_mutate_buffer_recursive(this);
	root_handle_reflect_buffer_early_recursive(this);
	root_handle_reflect_buffer_late_recursive(this);
	root_make_buffer_clean_recursive(this);

	// redraw connections after handling all the buffers
	root_init_draw_connections();
}


void FlowScriptEditorPlugin::EditedScript::root_init_draw_connections()
{
	DEV_ASSERT(is_root());

	root_draw_connections_recursive(this);
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
		List<int> operation_indices;
	};

	List<GroupedOperationList> operation_lists;

	for (int i = 0; i < buffer_mutate.size(); i++)
	{
		GroupedOperationList op_list = { .type = buffer_mutate[i].type };
		while (i < buffer_mutate.size() && buffer_mutate[i].type == op_list.type)
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
				delete_list.resize(op_list.operation_indices.size());
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
				drag_list.resize(op_list.operation_indices.size());
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
				add_list.resize(op_list.operation_indices.size());
				int i = 0;
				for (int op_idx : op_list.operation_indices)
				{
					add_list.write[i] = buffer_mutate[op_idx].data.node_add;
					i++;
				}
				handle_mutate_operation_list_node_add(add_list);
			} break;
			case MutateOperation::TYPE_NODE_CONNECT: {
				Vector<MutateOperation::NodeConnect> connect_list;
				connect_list.resize(op_list.operation_indices.size());
				int i = 0;
				for (int op_idx : op_list.operation_indices)
				{
					connect_list.write[i] = buffer_mutate[op_idx].data.node_connect;
					i++;
				}
				handle_mutate_operation_list_node_connect(connect_list);
			} break;
			case MutateOperation::TYPE_NODE_DISCONNECT: {
				Vector<MutateOperation::NodeDisconnect> disconnect_list;
				disconnect_list.resize(op_list.operation_indices.size());
				int i = 0;
				for (int op_idx : op_list.operation_indices)
				{
					disconnect_list.write[i] = buffer_mutate[op_idx].data.node_disconnect;
					i++;
				}
				handle_mutate_operation_list_node_disconnect(disconnect_list);
			} break;
			// TODO:
			// TYPE_NODE_RENAME
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

			node_editor->edited_flow_script = flow_script;
			node_editor->root_flow_script = get_root()->flow_script;
			if (parent != nullptr)
			{
				node_editor->parent_flow_script = parent->flow_script;
			}
			node_editor->edited_include_id = include_id;
			node_editor->edited_node_id = node_id;
			node_editor->edited_node = flow_script->get_node_ref(node_id);

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

			ReflectOperation::ItemSync sync_op;
			sync_op.item = op.item;
			reflect_item_sync(sync_op);
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
			node_editor_map.erase(node_id);
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
			node_editor->set_size(Size2(0, 0));

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


void FlowScriptEditorPlugin::EditedScript::handle_mutate_operation_list_item_delete(const Vector<MutateOperation::ItemDelete> &p_deletes)
{
	if (p_deletes.is_empty())
	{
		return;
	}
	int deleted_type_flags = 0;
	HashSet<FlowScriptNodeID> desired_node_id_delete_set;
	HashSet<FlowScriptIncludeID> desired_include_id_delete_set;
	for (const MutateOperation::ItemDelete &del : p_deletes)
	{
		switch (del.item.type)
		{
			case GraphItem::TYPE_NODE: {
				ERR_CONTINUE(del.item.node.flow_script_id != FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID);
				desired_node_id_delete_set.insert(del.item.node.node_id);
			} break;
			case GraphItem::TYPE_INCLUDE: {
				desired_include_id_delete_set.insert(del.item.include_id);
			} break;
		}
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

	struct UndoRestorableConnection
	{
		FlowScriptNodeID node_id;
		FlowScriptNodeOutputConnection output;
		FlowScriptNodeReference target;
	};
	List<UndoRestorableConnection> undo_restore_connection_list;

	for (const KeyValue<FlowScriptNodeID, FlowScriptNodeInstance> &kv : flow_script->node_map)
	{
		for (uint8_t list_idx = 0; list_idx < kv.value.connection_lists.size(); list_idx++)
		{
			for (int64_t slot_idx = 0; slot_idx < kv.value.connection_lists[list_idx].size(); slot_idx++)
			{
				if (desired_node_id_delete_set.has(kv.value.connection_lists[list_idx][slot_idx].node_id))
				{
					undo_restore_connection_list.push_back(UndoRestorableConnection {
						.node_id = kv.key,
						.output = FlowScriptNodeOutputConnection(list_idx, slot_idx),
						.target = kv.value.connection_lists[list_idx][slot_idx],
					});
				}
				else if (desired_include_id_delete_set.has(kv.value.connection_lists[list_idx][slot_idx].flow_script_id))
				{
					undo_restore_connection_list.push_back(UndoRestorableConnection {
						.node_id = kv.key,
						.output = FlowScriptNodeOutputConnection(list_idx, slot_idx),
						.target = kv.value.connection_lists[list_idx][slot_idx],
					});
				}
			}
		}
	}

	EditorUndoRedoManager *udrd = EditorUndoRedoManager::get_singleton();
	udrd->create_action(action_name, UndoRedo::MERGE_DISABLE);

	HashMap<FlowScriptNodeID, FlowScriptNodeInstance> deleted_node_instance_map;
	HashMap<FlowScriptIncludeID, FlowScriptIncludeInstance> deleted_include_instance_map;

	for (const MutateOperation::ItemDelete &del : p_deletes)
	{
		if (del.item.type == GraphItem::TYPE_NODE)
		{
			ERR_CONTINUE(del.item.node.flow_script_id != FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID);
			ERR_CONTINUE(!flow_script->node_map.has(del.item.node.node_id));

			FlowScriptNodeID node_id = del.item.node.node_id;
			FlowScriptNodeInstance node_instance_copy = flow_script->node_map[del.item.node.node_id];

			if (flow_script->remove_node(del.item.node.node_id))
			{
				udrd->add_do_method(flow_script.ptr(), SNAME("remove_node"), node_id);
				deleted_node_instance_map[node_id] = node_instance_copy;
				// save outgoing connections to nodes that aren't being deleted, too
				for (uint8_t list_idx = 0; list_idx < node_instance_copy.connection_lists.size(); list_idx++)
				{
					for (int64_t slot_idx = 0; slot_idx < node_instance_copy.connection_lists[list_idx].size(); slot_idx++)
					{
						if (node_instance_copy.connection_lists[list_idx][slot_idx].node_id != FlowScript::NODE_ID_INVALID)
						{
							undo_restore_connection_list.push_back(UndoRestorableConnection {
								.node_id = node_id,
								.output = FlowScriptNodeOutputConnection(list_idx, slot_idx),
								.target = node_instance_copy.connection_lists[list_idx][slot_idx],
							});
						}
					}
				}
			}
		}
	}
	for (const MutateOperation::ItemDelete &del : p_deletes)
	{
		if (del.item.type == GraphItem::TYPE_INCLUDE)
		{
			ERR_CONTINUE(!flow_script->has_include_flow_script_instance(del.item.include_id));

			FlowScriptIncludeID include_id = del.item.include_id;
			FlowScriptIncludeInstance include_instance_copy = flow_script->get_include_flow_script_instance(include_id);

			if (flow_script->remove_include_flow_script(include_id))
			{
				udrd->add_do_method(flow_script.ptr(), SNAME("remove_include_flow_script"), include_id);
				deleted_include_instance_map[include_id] = include_instance_copy;
			}
		}
	}

	for (const KeyValue<FlowScriptIncludeID, FlowScriptIncludeInstance> &kv : deleted_include_instance_map)
	{
		udrd->add_undo_method(flow_script.ptr(), SNAME("set_include_flow_script"), kv.key, kv.value.flow_script);
		udrd->add_undo_method(flow_script.ptr(), SNAME("set_include_position"), kv.key, kv.value.position);
	}
	for (const KeyValue<FlowScriptNodeID, FlowScriptNodeInstance> &kv : deleted_node_instance_map)
	{
		udrd->add_undo_method(flow_script.ptr(), SNAME("set_node"), kv.key, kv.value.node);
		udrd->add_undo_method(flow_script.ptr(), SNAME("set_node_position"), kv.key, kv.value.position);
	}

	// after all items have been restored on undo, restore the connections
	for (const UndoRestorableConnection &restorable_connection : undo_restore_connection_list)
	{
		if (restorable_connection.target.flow_script_id == FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID)
		{
			if (!deleted_node_instance_map.has(restorable_connection.target.node_id))
			{
				continue;
			}
		}
		else if (!deleted_include_instance_map.has(restorable_connection.target.flow_script_id))
		{
			continue;
		}
		udrd->add_undo_method(flow_script.ptr(), SNAME("set_node_connection"), restorable_connection.node_id, restorable_connection.output.list, restorable_connection.output.slot, restorable_connection.target.flow_script_id, restorable_connection.target.node_id);
	}

	udrd->commit_action(false);
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

				Point2i from = graph->point_convert_graph_to_data(drag.from + (node_editor->get_size() * 0.5));
				Point2i to = graph->point_convert_graph_to_data(drag.to + (node_editor->get_size() * 0.5));

				print_line("from ", from, " to ", to);

				udrd->add_do_method(flow_script.ptr(), SNAME("set_node_position"), drag.item.node.node_id, to);
				udrd->add_undo_method(flow_script.ptr(), SNAME("set_node_position"), drag.item.node.node_id, from);
			} break;
			case GraphItem::TYPE_INCLUDE: {
				DEV_ASSERT(children[drag.item.include_id] != nullptr);
				IncludeFrame *child_frame = children[drag.item.include_id]->frame;
				DEV_ASSERT(child_frame != nullptr);
				
				Point2i from = graph->point_convert_graph_to_data(drag.from + (child_frame->get_size() * 0.5));
				Point2i to = graph->point_convert_graph_to_data(drag.from + (child_frame->get_size() * 0.5));

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

	for (const MutateOperation::NodeAdd &add : p_adds)
	{
		Ref<FlowScriptNode> node = add.get_node();
		node->unreference();
		FlowScriptNodeID node_id = flow_script->get_first_available_node_slot();
		ERR_BREAK_EDMSG(node_id == FlowScript::NODE_ID_INVALID, TTR("No more node slots available."));

		Point2i pos = graph->point_convert_graph_to_data(add.position);

		udrd->add_do_method(flow_script.ptr(), SNAME("set_node"), node_id, node);
		udrd->add_do_method(flow_script.ptr(), SNAME("set_node_position"), node_id, pos);
		udrd->add_undo_method(flow_script.ptr(), SNAME("remove_node"), node_id);
	}

	udrd->commit_action(true); // since getting the id is performed by executing, don't commit the do actions, they've already been performed...
}


// void FlowScriptEditorPlugin::EditedScript::handle_mutate_operation_list_node_rename(const Vector<MutateOperation::NodeRename> &p_renames)
// {
// 	if (p_renames.is_empty())
// 	{
// 		return;
// 	}
// 	EditorUndoRedoManager *udrd = EditorUndoRedoManager::get_singleton();
// 	udrd->create_action(TTRN("Rename FlowScript Node", "Rename FlowScript Nodes", p_renames.size()), UndoRedo::MERGE_DISABLE);

// 	for (const MutateOperation::NodeRename &rename : p_renames)
// 	{
// 		FlowScriptNode *node = flow_script->get_node_ptr(rename.node_id);
// 		ERR_CONTINUE(node == nullptr);
// 		udrd->add_do_method(node, SNAME("set_name"), rename.new_name);
// 		udrd->add_undo_method(node, SNAME("set_name"), node->get_name());
// 	}

// 	udrd->commit_action(true);
// }


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
		udrd->add_do_method(flow_script.ptr(), SNAME("set_node_connection"), connection.from_node_id, connection.from_node_output.list, connection.from_node_output.slot, connection.to_node.flow_script_id, connection.to_node.node_id);
		udrd->add_undo_method(flow_script.ptr(), SNAME("set_node_connection"), connection.from_node_id, connection.from_node_output.list, connection.from_node_output.slot, curr_target.flow_script_id, curr_target.node_id);
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
		udrd->add_do_method(flow_script.ptr(), SNAME("set_node_connection"), disconnection.from_node_id, disconnection.from_node_output.list, disconnection.from_node_output.slot, FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID, FlowScript::NODE_ID_INVALID);
		udrd->add_undo_method(flow_script.ptr(), SNAME("set_node_connection"), disconnection.from_node_id, disconnection.from_node_output.list, disconnection.from_node_output.slot, curr_target.flow_script_id, curr_target.node_id);
	}

	udrd->commit_action(true);
}


void FlowScriptEditorPlugin::EditedScript::on_graph_visibility_changed()
{
	if (!is_root())
	{
		return;
	}
	if (graph->is_visible_in_tree())
	{
		if (root_reload_on_visible_queued)
		{
			root_reload_on_visible_queued = false;
			root_reload_everything();
		}
	}
}


void FlowScriptEditorPlugin::EditedScript::on_graph_gui_input(const Ref<InputEvent> &p_event)
{
	DEV_ASSERT(is_root());

	InputEventMouseButton *event = Object::cast_to<InputEventMouseButton>(p_event.ptr());
	if (event == nullptr)
	{
		return;
	}

	Ref<GraphEdit::Connection> closest_connection = graph->get_closest_connection_at_point(graph->get_local_mouse_position());
	if (!closest_connection.is_valid())
	{
		return;
	}
	FlowScriptNodeEditor *from_editor = Object::cast_to<FlowScriptNodeEditor>(graph->get_node(String(closest_connection->from_node)));
	if (from_editor == nullptr || from_editor->edited_flow_script != flow_script)
	{
		return;
	}

	MutateOperation op;
	op.type = MutateOperation::TYPE_NODE_DISCONNECT;
	op.data.node_disconnect.from_node_id = from_editor->edited_node_id;
	op.data.node_disconnect.from_node_output = from_editor->output_graph_slot_to_connection(from_editor->get_output_port_slot(closest_connection->from_port));
	queue_mutate_operation(op);
}


void FlowScriptEditorPlugin::EditedScript::on_graph_connection_drag_started(const StringName &p_from_node_name, const int p_from_port, const bool p_is_output)
{
	FlowScriptNodeEditor *node_editor = Object::cast_to<FlowScriptNodeEditor>(graph->get_node(String(p_from_node_name)));
	if (node_editor == nullptr)
	{
		return;
	}
	// TODO: Let the ports reach out for life
}


void FlowScriptEditorPlugin::EditedScript::on_graph_connection_drag_ended()
{
	// TODO: Stop the port reaching
}


void FlowScriptEditorPlugin::EditedScript::on_graph_connection_from_empty(const StringName &p_to_node_name, const int p_to_port, const Point2 &p_release_position)
{
	// NOTE: is this needed?
}


void FlowScriptEditorPlugin::EditedScript::on_graph_connection_to_empty(const StringName &p_from_node_name, const int p_from_port, const Point2 &p_release_position)
{
	// TODO: Pop up node create dialog, and if a node is created, automatically connect
}


void FlowScriptEditorPlugin::EditedScript::on_graph_delete_nodes_request(const TypedArray<StringName> &p_item_names)
{
	for (int i = 0; i < p_item_names.size(); i++)
	{
		Node *untyped_node = graph->get_node(String(p_item_names[i]));

		FlowScriptNodeEditor *node_editor = Object::cast_to<FlowScriptNodeEditor>(untyped_node);
		if (node_editor != nullptr)
		{
			if (node_editor->edited_flow_script == flow_script.ptr())
			{
				MutateOperation op;
				op.type = MutateOperation::TYPE_ITEM_DELETE;
				op.data.item_delete.item = GraphItem::create_node(node_editor->edited_node_id);
				queue_mutate_operation(op);
			}
		}
		else
		{
			IncludeFrame *child_frame = Object::cast_to<IncludeFrame>(untyped_node);
			if (child_frame->edited_script->parent == this)
			{
				MutateOperation op;
				op.type = MutateOperation::TYPE_ITEM_DELETE;
				op.data.item_delete.item = GraphItem::create_include(child_frame->edited_script->include_id);
				queue_mutate_operation(op);
			}
		}
	}
}


void FlowScriptEditorPlugin::EditedScript::on_graph_paste_nodes_request()
{
	plugin->clipboard_handler.paste_copied_nodes();
}


void FlowScriptEditorPlugin::EditedScript::on_graph_duplicate_nodes_request()
{
	plugin->clipboard_handler.duplicate_selected_nodes();
}


void FlowScriptEditorPlugin::EditedScript::on_graph_disconnection_request(const StringName &p_from_node_name, const int p_from_port, const StringName &p_to_node_name, const int p_to_port)
{
	ERR_FAIL_COND(!is_root());

	FlowScriptNodeEditor *from_editor = Object::cast_to<FlowScriptNodeEditor>(graph->get_node(String(p_from_node_name)));

	if ( from_editor == nullptr || from_editor->edited_flow_script != flow_script)
	{
		return;
	}

	MutateOperation op;
	op.type = MutateOperation::TYPE_NODE_DISCONNECT;
	op.data.node_disconnect.from_node_id = from_editor->edited_node_id;
	op.data.node_disconnect.from_node_output = from_editor->output_graph_slot_to_connection(from_editor->get_output_port_slot(p_from_port));
	queue_mutate_operation(op);
}


void FlowScriptEditorPlugin::EditedScript::on_graph_frame_rect_changed(GraphFrame *p_frame, const Size2 &p_new_rect)
{
}


void FlowScriptEditorPlugin::EditedScript::on_graph_node_deselected(Node *p_node)
{
}


void FlowScriptEditorPlugin::EditedScript::on_graph_node_selected(Node *p_node)
{
}


void FlowScriptEditorPlugin::EditedScript::on_graph_popup_request(const Point2 &p_at_position)
{
}


void FlowScriptEditorPlugin::EditedScript::on_graph_scroll_offset_changed(const Point2 &p_offset)
{
}


void FlowScriptEditorPlugin::EditedScript::on_script_changed()
{
	if (is_root())
	{
		if (!graph->is_visible_in_tree())
		{
			if (!root_reload_on_visible_queued)
			{
				root_reload_on_visible_queued = true;
				root_unload_everything();
			}
		}
	}
	if (!is_root())
	{
		EditedScript *root = get_root();
		if (graph->is_visible_in_tree())
		{
			if (!root->root_reload_queue_dirty)
			{
				root->root_reload_queue_dirty = true;
				callable_mp(root, &EditedScript::root_reload_everything).call_deferred();
			}
		}
		else
		{
			if (!root->root_reload_on_visible_queued)
			{
				root->root_reload_on_visible_queued = true;
				root->root_unload_everything();
			}
		}
	}
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
	ReflectOperation op;
	op.type = ReflectOperation::TYPE_ITEM_SYNC;
	op.data.item_sync.item = GraphItem::create_node(p_node_id);
	queue_reflect_operation(op);
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
	if (!is_root())
	{
		unload_everything();
	}
}


Vector<FlowScriptNodeInstance> FlowScriptEditorPlugin::ClipboardHandler::get_node_copy_list(const EditedScript *p_edited_script, const List<FlowScriptNodeEditor *> p_node_editors) const
{
	Vector<FlowScriptNodeInstance> ret;

	HashMap<FlowScriptNodeID, FlowScriptNodeID> connection_target_remaps;
	Point2i center_point = p_edited_script->graph->get_visible_center_as_data_point();

	FlowScriptNodeID curr_node_copy_id = 0;
	for (const FlowScriptNodeEditor *node_editor : p_node_editors)
	{
		DEV_ASSERT(node_editor != nullptr);
		FlowScriptNodeInstance node_instance = node_editor->edited_flow_script->node_map[node_editor->edited_node_id];
		node_instance.node = node_instance.node->duplicate(false); // duplicate when copying, but not subresources... right?
		node_instance.node->set_name(""); // reset name when copying
		node_instance.position -= center_point; // save relative to screen center
		ret.push_back(node_instance);
		connection_target_remaps.insert(node_editor->edited_node_id, curr_node_copy_id);
		curr_node_copy_id++;
	}

	for (FlowScriptNodeInstance &node_instance : ret)
	{
		for (uint8_t list_idx = 0; list_idx < node_instance.connection_lists.size(); list_idx++)
		{
			for (int64_t slot_idx = 0; slot_idx < node_instance.connection_lists[list_idx].size(); slot_idx++)
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

	return ret;
}


Vector<FlowScriptNodeInstance> FlowScriptEditorPlugin::ClipboardHandler::get_selected_node_copy_list() const
{
	EditedScript *script = plugin->get_current_edited_script();
	if (script == nullptr)
	{
		return Vector<FlowScriptNodeInstance>();
	}
	else
	{
		return get_node_copy_list(script, script->get_selected_node_editors());
	}
}


void FlowScriptEditorPlugin::ClipboardHandler::paste_copied_nodes()
{
	if (current_copied_nodes.is_empty())
	{
		return;
	}
	insert_node_copies(current_copied_nodes, MSG_PASTE);
}


void FlowScriptEditorPlugin::ClipboardHandler::duplicate_selected_nodes()
{
	Vector<FlowScriptNodeInstance> copies;
	if (copies.is_empty())
	{
		return;
	}
	Point2i dupe_ofs = EDITOR_GET("editors/flow_script_editor/duplicate_item_offset");
	for (FlowScriptNodeInstance &copy : copies)
	{
		copy.position += dupe_ofs;
	}
	insert_node_copies(copies, MSG_DUPLICATE);
}


void FlowScriptEditorPlugin::ClipboardHandler::copy_selected_nodes()
{
	Point2i copy_ofs = EDITOR_GET("editors/flow_script_editor/copy_item_offset");
	current_copied_nodes = get_selected_node_copy_list();
	for (FlowScriptNodeInstance &copy : current_copied_nodes)
	{
		copy.position += copy_ofs;
	}
}


void FlowScriptEditorPlugin::ClipboardHandler::insert_node_copies(const Vector<FlowScriptNodeInstance> &p_copies, const InsertCopyMessage copy_msg_type)
{
	if (p_copies.is_empty())
	{
		return;
	}
	EditedScript *script = plugin->get_current_edited_script();
	if (script == nullptr)
	{
		return;
	}

	String copy_msg;
	switch (copy_msg_type)
	{
		case MSG_PASTE: {
			copy_msg = TTRN("Paste FlowScript Node", "Paste FlowScript Nodes", p_copies.size());
		} break;
		case MSG_DUPLICATE: {
			copy_msg = TTRN("Duplicate FlowScript Node", "Duplicate FlowScript Nodes", p_copies.size());
		} break;
	}

	EditorUndoRedoManager *udrd = EditorUndoRedoManager::get_singleton();
	udrd->create_action(copy_msg, UndoRedo::MERGE_DISABLE);

	Point2i center_point = script->graph->get_visible_center_as_data_point();
	HashMap<int, FlowScriptNodeID> inserted_node_script_id_map;

	for (int i = 0; i < p_copies.size(); i++)
	{
		const FlowScriptNodeInstance &copy = p_copies[i];

		FlowScriptNodeID id = script->flow_script->get_first_available_node_slot();
		ERR_BREAK_EDMSG(id == FlowScript::NODE_ID_INVALID, TTR("No more node slots available."));

		inserted_node_script_id_map[i] = id;

		udrd->add_do_method(script->flow_script.ptr(), SNAME("set_node"), id, copy.node);
		udrd->add_do_method(script->flow_script.ptr(), SNAME("set_node_position"), id, center_point + copy.position);

		udrd->add_undo_method(script->flow_script.ptr(), SNAME("remove_node"), id);
	}

	for (int i = 0; i < p_copies.size(); i++)
	{
		if (!inserted_node_script_id_map.has(i))
		{
			continue;
		}
		FlowScriptNodeID copy_id = inserted_node_script_id_map[i];
		const FlowScriptNodeInstance &copy = p_copies[i];
		for (uint8_t list_idx = 0; list_idx < copy.connection_lists.size(); list_idx++)
		{
			for (int64_t slot_idx = 0; slot_idx < copy.connection_lists[list_idx].size(); slot_idx++)
			{
				FlowScriptNodeReference target = copy.connection_lists[list_idx][slot_idx];
				if (target.flow_script_id == FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID && inserted_node_script_id_map.has(target.node_id))
				{
					FlowScriptNodeID target_id = inserted_node_script_id_map[target.node_id];
					udrd->add_do_method(script->flow_script.ptr(), SNAME("set_node_connection"), copy_id, list_idx, slot_idx, FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID, target_id);
				}
			}
		}
	}

	udrd->commit_action(true);
}


void FlowScriptEditorPlugin::prompt_script_file_create_new()
{
	current_file_action = FILE_ACTION_CREATE_SCRIPT;
	file_dialog->set_file_mode(EditorFileDialog::FILE_MODE_SAVE_FILE);
	file_dialog->popup_file_dialog();
}


void FlowScriptEditorPlugin::prompt_script_file_load_existing()
{
	current_file_action = FILE_ACTION_EDIT_SCRIPT;
	file_dialog->set_file_mode(EditorFileDialog::FILE_MODE_OPEN_FILE);
	file_dialog->popup_file_dialog();
}


void FlowScriptEditorPlugin::on_flow_script_saved(const Ref<FlowScript> &p_flow_script)
{
	for (EditedScript *edited_script : open_script_list)
	{
		if (edited_script->save_state_dirty && edited_script->flow_script == p_flow_script)
		{
			edited_script->save_state_dirty = false;
			refresh_script_item_list();
		}
	}
}


bool FlowScriptEditorPlugin::save_script_file_at(int p_idx)
{
	ERR_FAIL_INDEX_V(p_idx, open_script_list.size(), false);
	EditedScript *script = get_edited_script_at(p_idx);
	if (script != nullptr && script->flow_script.is_valid())
	{
		if (ResourceSaver::save(script->flow_script, script->flow_script->get_path()) == OK)
		{
			on_flow_script_saved(script->flow_script);
			return true;
		}
	}
	return false;
}


bool FlowScriptEditorPlugin::save_current_edited_script_file()
{
	int idx = get_current_edited_script_index();
	if (idx != -1)
	{
		return save_script_file_at(idx);
	}
	return false;
}


bool FlowScriptEditorPlugin::save_all_open_script_files()
{
	bool ok = true;
	int count = get_open_script_count();
	for (int i = 0; i < count; i++)
	{
		if (!save_script_file_at(i))
		{
			ok = false;
		}
	}
	return ok;
}


bool FlowScriptEditorPlugin::is_editing_any_script() const
{
	return !open_script_list.is_empty();
}


bool FlowScriptEditorPlugin::is_editing_specific_script_at(const int p_idx) const
{
	ERR_FAIL_INDEX_V(p_idx, open_script_list.size(), false);
	return get_current_edited_script_index() == p_idx;
}


int FlowScriptEditorPlugin::get_edited_script_index(const EditedScript *p_script) const
{
	for (int i = 0; i < open_script_list.size(); i++)
	{
		if (open_script_list[i] == p_script)
		{
			return i;
		}
	}
	return -1;
}


int FlowScriptEditorPlugin::get_current_edited_script_index() const
{
	return current_edited_script_idx;
}


FlowScriptEditorPlugin::EditedScript *FlowScriptEditorPlugin::get_edited_script_at(const int p_idx) const
{
	ERR_FAIL_INDEX_V(p_idx, open_script_list.size(), nullptr);
	return open_script_list[p_idx];
}


FlowScriptEditorPlugin::EditedScript *FlowScriptEditorPlugin::get_current_edited_script() const
{
	return get_edited_script_at(get_current_edited_script_index());
}


void FlowScriptEditorPlugin::close_edited_script_at(int p_idx, bool p_warn_if_unsaved)
{
	ERR_FAIL_INDEX(p_idx, open_script_list.size());
	EditedScript *script = get_edited_script_at(p_idx);
	ERR_FAIL_NULL(script);
	if (script->save_state_dirty && p_warn_if_unsaved)
	{
		script->close_confirm_dialog->prompt_action(script->flow_script->get_path());
	}
	else
	{
		// accommodate edited script
		int new_edited_idx = current_edited_script_idx;
		for (int between_num = p_idx; between_num <= current_edited_script_idx; between_num++)
		{
			new_edited_idx--;
		}
		set_open_script_to_idx(new_edited_idx);

		open_script_list.remove_at(p_idx);

		script->graph->queue_free();
		memdelete(script);

		refresh_script_item_list();
	}
}


void FlowScriptEditorPlugin::close_edited_script_by_ptr(EditedScript *p_script, bool p_warn_if_unsaved)
{
	close_edited_script_at(get_edited_script_index(p_script), p_warn_if_unsaved);
}


void FlowScriptEditorPlugin::close_current_edited_script(bool p_warn_if_unsaved)
{
	close_edited_script_at(get_current_edited_script_index(), p_warn_if_unsaved);
}


void FlowScriptEditorPlugin::close_all_scripts(bool p_warn_if_unsaved)
{
	for (int i = open_script_list.size() - 1; i > -1; i--)
	{
		close_edited_script_at(i, p_warn_if_unsaved);
	}
}


int FlowScriptEditorPlugin::get_open_script_count() const
{
	return open_script_list.size();
}


void FlowScriptEditorPlugin::on_script_close_dialog_discard(EditedScript *p_script)
{
	close_edited_script_by_ptr(p_script, false);
}


void FlowScriptEditorPlugin::on_script_close_dialog_save(EditedScript *p_script)
{
	int idx = get_edited_script_index(p_script);
	save_script_file_at(idx);
	close_edited_script_at(idx, true);
}


void FlowScriptEditorPlugin::on_make_floating_button_open_to_screen_request(int p_screen_id)
{
	window_wrapper->enable_window_on_screen(p_screen_id, true);
}


void FlowScriptEditorPlugin::on_window_wrapper_visibility_changed(bool p_visible)
{
	make_floating_button->set_visible(!p_visible);
}


void FlowScriptEditorPlugin::on_file_menu_item_pressed(int p_idx)
{
	switch (p_idx)
	{
		case FILE_NEW: {
			prompt_script_file_create_new();
		} break;
		case FILE_OPEN: {
			prompt_script_file_load_existing();
		} break;
		case FILE_SAVE: {
			save_current_edited_script_file();
		} break;
		case FILE_SAVE_ALL: {
			save_all_open_script_files();
		} break;
		case FILE_CLOSE: {
			close_current_edited_script(true);
		} break;
		case FILE_CLOSE_ALL: {
			close_all_scripts(true);
		} break;
	}
}


void FlowScriptEditorPlugin::file_menu_update_clickable()
{
	bool none_open = !is_editing_any_script();

	file_menu->get_popup()->set_item_disabled(FILE_SAVE, none_open);
	file_menu->get_popup()->set_item_disabled(FILE_CLOSE, none_open);
	file_menu->get_popup()->set_item_disabled(FILE_CLOSE_ALL, open_script_list.is_empty());
}


void FlowScriptEditorPlugin::edit_flow_script_if_not_open(FlowScript *p_script)
{
	for (int i = 0; i < open_script_list.size(); i++)
	{
		if (open_script_list[i]->flow_script.ptr() == p_script)
		{
			return;
		}
	}
	edit_unopened_script(p_script);
}


void FlowScriptEditorPlugin::edit_flow_script_even_if_open(FlowScript *p_script)
{
	if (p_script == nullptr)
	{
		return;
	}
	bool should_update = false;
	bool found = false;
	for (int i = 0; i < open_script_list.size(); i++)
	{
		if (open_script_list[i]->flow_script.ptr() == p_script)
		{
			found = true;
			if (i != current_edited_script_idx)
			{
				current_edited_script_idx = i;
				should_update = true;
			}
			break;
		}
	}
	if (found)
	{
		if (should_update)
		{
			update_open_script();
		}
	}
	else
	{
		edit_unopened_script(p_script);
	}
}


void FlowScriptEditorPlugin::edit_unopened_script(FlowScript *p_script)
{
	if (p_script == nullptr)
	{
		return;
	}
	for (const EditedScript *edited_script : open_script_list)
	{
		ERR_FAIL_COND(edited_script->flow_script.ptr() == p_script);
	}
	ScriptGraph *graph = memnew(ScriptGraph(this));
	graph_tab_container->add_child(graph);

	ScriptCloseConfirmationDialog *close_confirm_dialog = memnew(ScriptCloseConfirmationDialog);
	graph->add_child(close_confirm_dialog);

	EditedScript *edited_script = EditedScript::create_root_edited_script(this, p_script, graph);
	edited_script->close_confirm_dialog = close_confirm_dialog;

	edited_script->flow_script->connect_changed(callable_mp(this, &FlowScriptEditorPlugin::on_script_changed).bind(edited_script));
	edited_script->graph->connect(SNAME("popup_request"), callable_mp(this, &FlowScriptEditorPlugin::on_script_graph_popup_request).bind(edited_script));
	edited_script->graph->connect(SNAME("flow_script_node_create_prompt_request"), callable_mp(this, &FlowScriptEditorPlugin::on_script_graph_node_create_prompt_request).bind(edited_script));
	edited_script->close_confirm_dialog->connect(SNAME("act_save"), callable_mp(this, &FlowScriptEditorPlugin::on_script_close_dialog_save).bind(edited_script));
	edited_script->close_confirm_dialog->connect(SNAME("act_discard"), callable_mp(this, &FlowScriptEditorPlugin::on_script_close_dialog_discard).bind(edited_script));

	current_edited_script_idx = open_script_list.size();
	open_script_list.push_back(edited_script);

	edited_script->root_load_everything();

	refresh_script_item_list();
}


void FlowScriptEditorPlugin::update_open_script()
{
	set_open_script_to_idx(current_edited_script_idx);
}


void FlowScriptEditorPlugin::set_open_script_to_idx(int p_idx)
{
	if (p_idx < 0 || p_idx >= open_script_list.size())
	{
		p_idx = -1;
	}
	current_edited_script_idx = p_idx;
	if (p_idx == -1)
	{
		script_item_list->deselect_all();
		graph_tab_container->set_current_tab(-1);
	}
	else
	{
		script_item_list->select(p_idx);
		ScriptGraph *graph = open_script_list[current_edited_script_idx]->graph;
		int graph_idx = graph_tab_container->get_tab_idx_from_control(graph);
		graph_tab_container->set_current_tab(graph_idx);
	}
}


void FlowScriptEditorPlugin::refresh_script_item_list()
{
	if (!script_item_list_refresh_timer->is_stopped())
	{
		script_item_list_refresh_timer->stop();
	}
	script_item_list->clear();
	for (EditedScript *open_script : open_script_list)
	{
		String path = open_script->flow_script->get_path();
		String name = path.get_file();
		if (open_script->save_state_dirty)
		{
			name = vformat(TTR("%s(*)"), name);
		}
		int id = script_item_list->add_item(name);
		script_item_list->set_item_tooltip(id, path);
		script_item_list->set_item_tooltip_enabled(id, true);
	}
	update_open_script();
}


void FlowScriptEditorPlugin::on_script_item_list_item_selected(int p_item)
{
	set_open_script_to_idx(p_item);
}


void FlowScriptEditorPlugin::on_script_item_list_item_clicked(int p_item, Point2 p_local_mouse_pos, MouseButton p_btn_idx)
{
	if (p_btn_idx == MouseButton::RIGHT)
	{
		// TODO: Implement a PopupMenu for the script ItemList with options like close, save...
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
		if (type.enabled && (type.node_script_class_name == p_script_class && type.node_class == p_native_class))
		{
			node = FlowScriptNodeTypeDB::get_singleton()->instantiate_node_for_type(type);
			break;
		}
	}

	ERR_FAIL_COND(!node.is_valid());

	EditedScript *script = get_current_edited_script();
	DEV_ASSERT(script != nullptr);

	node->reference();

	EditedScript::MutateOperation op;
	op.type = EditedScript::MutateOperation::TYPE_NODE_ADD;
	op.data.node_add.set_node(node.ptr());
	op.data.node_add.position = next_node_create_point;
	script->queue_mutate_operation(op);
}


void FlowScriptEditorPlugin::on_script_graph_popup_request(const Point2 &p_at_position, EditedScript *p_script)
{
	next_node_create_point = p_script->graph->point_convert_rect_to_graph(p_at_position);

	Rect2i pop_rect;
	pop_rect.size = node_create_dialog->get_size_with_decorations();
	pop_rect.position = DisplayServer::get_singleton()->mouse_get_position() - (pop_rect.size / 2);
	node_create_dialog->popup(pop_rect);
}


void FlowScriptEditorPlugin::on_script_graph_node_create_prompt_request(EditedScript *p_script)
{
	next_node_create_point = p_script->graph->get_visible_center_as_data_point();

	node_create_dialog->popup_centered();
}


void FlowScriptEditorPlugin::on_file_dialog_file_selected(const String &p_path)
{
	switch (current_file_action)
	{
		case FILE_ACTION_EDIT_SCRIPT: {
			if (ResourceLoader::exists(p_path, "FlowScript"))
			{
				Ref<FlowScript> flow_script = ResourceLoader::load(p_path, "FlowScript");
				if (flow_script.is_valid())
				{
					edit_flow_script_even_if_open(flow_script.ptr());
				}
			}
		} break;
		case FILE_ACTION_CREATE_SCRIPT: {
			if (ResourceLoader::exists(p_path, "FlowScript"))
			{
				current_file_action = FILE_ACTION_EDIT_SCRIPT;
				on_file_dialog_file_selected(p_path);
			}
			else
			{
				Ref<FlowScript> new_script;
				new_script.instantiate();
				if (ResourceSaver::save(new_script, p_path) == OK)
				{
					edit_flow_script_even_if_open(new_script.ptr());
				}
			}
		} break;
		// unimplemented
		case FILE_ACTION_SAVE_SCRIPT: {
		} break;
	}
}


void FlowScriptEditorPlugin::on_script_changed(EditedScript *script_ptr)
{
	script_ptr->save_state_dirty = true;
	// reset cooldown, the user should stop editing for a given period of time for it to actually do its thing
	if (!script_item_list_refresh_timer->is_stopped())
	{
		script_item_list_refresh_timer->stop();
	}
	script_item_list_refresh_timer->start();
}


void FlowScriptEditorPlugin::on_resource_saved(const Ref<Resource> &p_resource)
{
	Ref<FlowScript> script_res = p_resource;
	if (script_res.is_valid())
	{
		on_flow_script_saved(script_res);
	}
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
	edit_flow_script_if_not_open(flow_script_ptr);
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
	node_type_db_singleton = memnew(FlowScriptNodeTypeDB);
	add_child(node_type_db_singleton);

	clipboard_handler.plugin = this;

	window_wrapper = memnew(WindowWrapper);
	window_wrapper->set_window_title(vformat(TTR("%s - Godot Engine"), TTR("FlowScript Editor")));
	window_wrapper->set_margins_enabled(true);
	window_wrapper->connect("window_visibility_changed", callable_mp(this, &FlowScriptEditorPlugin::on_window_wrapper_visibility_changed));

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
	file_menu->get_popup()->add_separator();
	file_menu->get_popup()->add_item(TTR("Close"), FILE_CLOSE);
	file_menu->get_popup()->add_item(TTR("Close All"), FILE_CLOSE_ALL);
	file_menu->get_popup()->connect(SceneStringName(id_pressed), callable_mp(this, &FlowScriptEditorPlugin::on_file_menu_item_pressed));
	menu_hbox->add_child(file_menu);

	file_menu_update_clickable();

	Control *menu_middle_pad = memnew(Control);
	menu_middle_pad->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	menu_hbox->add_child(menu_middle_pad);

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
	graph_tab_container->set_deselect_enabled(true);
	graph_tab_container->add_theme_style_override(SceneStringName(panel), empty_style);
	main_split->add_child(graph_tab_container);

	node_create_dialog = memnew(FlowScriptNodeCreateDialog);
	node_create_dialog->connect("type_chosen", callable_mp(this, &FlowScriptEditorPlugin::on_node_create_dialog_type_chosen));
	add_child(node_create_dialog);

	file_dialog = memnew(EditorFileDialog);
	file_dialog->connect(SNAME("file_selected"), callable_mp(this, &FlowScriptEditorPlugin::on_file_dialog_file_selected));
	file_dialog->add_filter("*.tres", TTR("Text FlowScript Resource"));
	file_dialog->add_filter("*.res", TTR("Binary FlowScript Resource"));
	add_child(file_dialog);

	script_item_list_refresh_timer = memnew(Timer);
	script_item_list_refresh_timer->set_autostart(false);
	script_item_list_refresh_timer->set_one_shot(true);
	script_item_list_refresh_timer->set_wait_time(0.5); // NOTE: should the wait time be configurable?
	script_item_list_refresh_timer->connect("timeout", callable_mp(this, &FlowScriptEditorPlugin::refresh_script_item_list));
	add_child(script_item_list_refresh_timer);

	inspector_plugin.instantiate();
	inspector_plugin->plugin = this;
	add_inspector_plugin(inspector_plugin);
}


FlowScriptEditorPlugin::~FlowScriptEditorPlugin()
{
}


void FlowScriptEditorPlugin::_bind_methods()
{
	EDITOR_DEF("editors/flow_script_editor/copy_item_offset", Point2i(40, 40));
	EDITOR_DEF("editors/flow_script_editor/duplicate_item_offset", Point2i(40, 40));
}


void FlowScriptEditorPlugin::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_READY: {
			connect("resource_saved", callable_mp(this, &FlowScriptEditorPlugin::on_resource_saved));
		} break;
	}
}
