#ifndef FLOW_SCRIPT_NODE_HPP
#define FLOW_SCRIPT_NODE_HPP


#include "flow_script_typedefs.hpp"
#include "flow_script_constants.hpp"
#include "flow_script_node_context.hpp"
#include "flow_script_node_translation.hpp"
#include "core/io/resource.h"


class FlowScriptNode : public Resource
{
	GDCLASS(FlowScriptNode, Resource);

public:
	// Executed first, mainly used to initialize variables
	virtual void exec_startup(FlowScriptNodeContext *p_context);
	// Cleanup
	virtual void exec_cleanup(FlowScriptNodeContext *p_context);
	// Main execution method
	virtual void exec_step(FlowScriptNodeContext *p_context);
	// Used when saving the game
	virtual void set_runtime_state(FlowScriptNodeContext *p_context, const Dictionary &p_state);
	// Used when loading the game
	virtual Dictionary get_runtime_state(const FlowScriptNodeContext *p_context) const;
	// Used to load the node data (eg. for a game mod)
	virtual void set_data_state(const Dictionary &p_data);
	// Return node data
	virtual Dictionary get_data_state() const;
	// Used to update the FlowScript connection data
	virtual void get_output_connection_list_lengths(List<FlowScriptNodeConnectionListLength> *p_lengths) const;

	FlowScriptNode() {}
	~FlowScriptNode() {}
};


#endif // FLOW_SCRIPT_NODE_HPP
