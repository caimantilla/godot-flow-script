#ifndef FLOW_SCRIPT_NODE_EDITOR_HPP
#define FLOW_SCRIPT_NODE_EDITOR_HPP


#include "scene/gui/graph_node.h"
#include "scene/gui/label.h"
#include "typedefs.hpp"
#include "flow_script.hpp"
#include "flow_script_node.hpp"
#include "flow_script_node_editor_outgoing_connection_parameters.hpp"


class FlowScriptNodeEditor : public GraphNode
{
	GDCLASS(FlowScriptNodeEditor, GraphNode);

private:
	FlowScript *edited_flow_script = nullptr;
	FlowScriptNodeID edited_node_id = FlowScript::NODE_ID_INVALID;
	bool current_editable = false;

protected:
	static void _bind_methods();
	void _notification(int p_what);

	GDVIRTUAL0(_startup);
	GDVIRTUAL0(_cleanup);
	GDVIRTUAL0(_sync);
	GDVIRTUAL0(_update_theme);
	GDVIRTUAL0RC(String, _get_new_title);
	GDVIRTUAL0RC(String, _get_new_tooltip_text);
	GDVIRTUAL1(_set_outgoing_connection, Dictionary);
	GDVIRTUAL0RC(TypedArray<Dictionary>, _get_outgoing_connections);
	GDVIRTUAL0RC(int, _get_input_slot);

public:
	void block_editing();
	void permit_editing();
	bool is_editable() const;

	void set_edited_flow_script(FlowScript *p_flow_script);
	Ref<FlowScript> get_edited_flow_script_ref() const;
	FlowScript *get_edited_flow_script_ptr() const;
	void set_edited_node_id(const FlowScriptNodeID p_node_id);
	FlowScriptNodeID get_edited_node_id() const;
	Ref<FlowScriptNode> get_edited_node_ref() const;
	FlowScriptNode *get_edited_node_ptr() const;

	int input_port_to_slot(const int p_port) const;
	int input_slot_to_port(const int p_slot) const;
	int output_port_to_slot(const int p_port) const;
	int output_slot_to_port(const int p_slot) const;

	virtual void startup();
	virtual void cleanup();
	virtual void sync();
	virtual void update_theme();
	virtual String get_new_title() const;
	virtual String get_new_tooltip_text() const;
	virtual void set_outgoing_connection(FlowScriptNodeEditorOutgoingConnectionParameters p_connection);
	virtual void get_outgoing_connections(List<FlowScriptNodeEditorOutgoingConnectionParameters> *p_list) const;
	virtual int get_input_slot() const;

	FlowScriptNodeEditor();
};


#endif // FLOW_SCRIPT_NODE_EDITOR_HPP
