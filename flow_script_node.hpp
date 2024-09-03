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

public:
	// Executed first, mainly used to initialize variables
	virtual void exec_startup(FlowScriptNodeContext *p_context);
	// Cleanup
	virtual void exec_cleanup(FlowScriptNodeContext *p_context);
	// Main execution method
	virtual void exec_step(FlowScriptNodeContext *p_context);
	// If true, the node can be translated
	virtual bool can_translate_text() const;
	// If can_translate returns true, a translation can be initialized here
	virtual void init_text_translation(const FlowScriptNodeID p_node_id, FlowScriptNodeTranslation *p_translation);
	// Used when saving the game
	virtual void set_state(FlowScriptNodeContext *p_context, const Dictionary &p_state);
	// Used when loading the game
	virtual void get_state(const FlowScriptNodeContext *p_context, Dictionary &r_state) const;
	// Used to load the node data from JSON (eg. for a game mod)
	virtual void set_json_data(const Dictionary &p_data);
	// Write JSON-compatible node data
	virtual void get_json_data(Dictionary &r_data) const;
	// Used to update the FlowScript connection data
	virtual void get_output_connection_list_lengths(List<int64_t> &r_lengths) const;

	FlowScriptNode() {}
	~FlowScriptNode() {}
};


#endif // FLOW_SCRIPT_NODE_HPP
