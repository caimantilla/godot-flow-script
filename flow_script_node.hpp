#ifndef FLOW_SCRIPT_NODE_HPP
#define FLOW_SCRIPT_NODE_HPP


#include "core/io/resource.h"
#include "flow_script_node_context.hpp"
#include "flow_script_node_translation.hpp"


class FlowScriptNode : public Resource
{
	GDCLASS(FlowScriptNode, Resource);

#ifdef TOOLS_ENABLED
private:
	uint64_t editor_dependency_count = 0;
public:
	void editor_dependency_inc() { editor_dependency_count++; }
	void editor_dependency_dec() { editor_dependency_count--; }
	bool has_editor_dependencies() const { return editor_dependency_count != 0; }
#endif // TOOLS_ENABLED

protected:
	static void _bind_methods();

public:
	// Executed first, mainly used to initialize variables
	virtual void exec_startup(FlowScriptNodeContext *p_context) {};
	// Cleanup
	virtual void exec_cleanup(FlowScriptNodeContext *p_context) {};
	// Main execution method
	virtual void exec_step(FlowScriptNodeContext *p_context) {};
	// ID of the type, can be used to override a native type or for JSON serialization
	virtual String get_type_id() const = 0;
	// The user-facing name of the node's type
	virtual String get_type_name() const = 0;
	// The category that the type belongs to (sub-categories separated using '/')
	virtual String get_type_category() const = 0;
	// The user-facing description of the node's type
	virtual String get_type_description() const = 0;
	// This can return a class name, scene path, or script path, it's just gotta be a FlowScriptNodeEditor
	virtual String get_type_editor() const = 0;
	// Determines if the GUI permits naming the node.
	// I don't think that this should be exposed to scripting
	virtual bool can_name_node() const { return false; }
	// If true, the node can be translated
	virtual bool can_translate_text() const { return false; }
	// If can_translate returns true, a translation can be initialized here
	virtual void init_text_translation(const FlowScriptNodeID p_node_id, FlowScriptNodeTranslation *p_translation) {}
	// Used when saving the game
	virtual void set_state(FlowScriptNodeContext *p_context, const Dictionary &p_state) {}
	// Used when loading the game
	virtual void get_state(const FlowScriptNodeContext *p_context, Dictionary &r_state) const {}
	// Used to load the node data from JSON (eg. for a game mod)
	virtual void set_json_data(const Dictionary &p_data) {}
	// Write JSON-compatible node data
	virtual void get_json_data(Dictionary &r_data) const {}
	// Used to update the FlowScript connection data
	virtual void get_output_connection_list_lengths(List<int64_t> &r_lengths) const {}

	void request_output_connection_data_update();
};


#endif // FLOW_SCRIPT_NODE_HPP
