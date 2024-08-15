#ifndef FLOW_SCRIPT_NODE_PROCEDURE_HPP
#define FLOW_SCRIPT_NODE_PROCEDURE_HPP


#include "common.hpp"


class FlowScriptNodeProcedure final : public FlowScriptNode
{
	GDCLASS(FlowScriptNodeProcedure, FlowScriptNode);

public:
	virtual void exec_step(FlowScriptNodeContext *p_context) override;
};


#endif // FLOW_SCRIPT_NODE_PROCEDURE_HPP
