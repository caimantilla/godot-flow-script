#ifndef FLOW_SCRIPT_NODE_EDITOR_HPP
#define FLOW_SCRIPT_NODE_EDITOR_HPP


#include "../flow_script_node_type_info.hpp"
#include "../../flow_script.hpp"
#include "../../flow_script_node.hpp"
#include "../../flow_script_node_reference.hpp"
#include "../../flow_script_typedefs.hpp"
#include "scene/gui/graph_node.h"


class Button;


class FlowScriptNodeEditor : public GraphNode
{
	GDCLASS(FlowScriptNodeEditor, GraphNode);

private:
	FlowScriptNodeTypeInfo type_info;
	Ref<FlowScript> root_flow_script;
	Ref<FlowScript> owner_flow_script;
	FlowScriptNodeReference edited_node_reference = FlowScriptNodeReference::create_null_reference();
	Ref<FlowScriptNode> edited_node;

	Button *rename_button;
	Button *delete_button;

	void on_rename_button_pressed();
	void on_delete_button_pressed();

protected:
	static void _bind_methods();
	void _notification(int p_what);

	GDVIRTUAL0(_startup);
	GDVIRTUAL0(_cleanup);
	GDVIRTUAL0(_sync);
	GDVIRTUAL0RC(int, _get_input_slot);
	GDVIRTUAL1RC(Dictionary, _output_graph_slot_to_connection, int);
	GDVIRTUAL2RC(int, _output_connection_to_graph_slot, FlowScriptNodeConnectionListNo, FlowScriptNodeConnectionListSlotNo);

public:

	bool is_include() const;

	Button *get_rename_button() const;
	Button *get_delete_button() const;

	FlowScriptNodeTypeInfo get_type_info() const;
	Ref<FlowScript> get_root_flow_script() const;
	Ref<FlowScript> get_owner_flow_script() const;
	FlowScriptNodeReference get_edited_node_reference() const;
	FlowScriptIncludeID get_edited_include_id() const;
	FlowScriptNodeID get_edited_node_id() const;
	Ref<FlowScriptNode> get_edited_node() const;

	virtual bool is_placeholder() const;
	virtual void startup();
	virtual void cleanup();
	virtual void sync();
	virtual FlowScriptNodeOutputConnection output_graph_slot_to_connection(const int p_graph_slot) const;
	virtual int output_connection_to_graph_slot(const FlowScriptNodeOutputConnection &p_connection) const;
	virtual int get_input_slot() const;

	void init_dependencies(const FlowScriptNodeTypeInfo &p_type_info, const Ref<FlowScript> &p_root_flow_script, const FlowScriptNodeReference &p_edited_node_reference, const Ref<Theme> &p_msdf_theme);
	void set_show_rename_button(const bool p_visible);
	void set_show_delete_button(const bool p_visible);

	FlowScriptNodeEditor();
};


#endif // FLOW_SCRIPT_NODE_EDITOR_HPP
