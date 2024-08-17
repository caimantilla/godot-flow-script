#include "register_types.h"
#include "flow_script.hpp"
#include "flow_script_node.hpp"
#include "flow_script_node_editor.hpp"
#include "flow_script_node_translation.hpp"
#include "flow_script_node_custom.hpp"
#include "flow_script_bridge.hpp"
#include "flow_script_execution_controller.hpp"

#include "included_nodes/procedure.hpp"


void initialize_flow_script_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		GDREGISTER_ABSTRACT_CLASS(FlowScriptNode);
		GDREGISTER_ABSTRACT_CLASS(FlowScriptNodeContext);
		
		GDREGISTER_CLASS(FlowScript);
		GDREGISTER_CLASS(FlowScriptNodeEditor);
		GDREGISTER_CLASS(FlowScriptNodeTranslation);
		GDREGISTER_CLASS(FlowScriptBridge);
		GDREGISTER_CLASS(FlowScriptNodeCustom);
		GDREGISTER_CLASS(FlowScriptBridge);

		GDREGISTER_CLASS(FlowScriptNodeProcedure);
		GDREGISTER_CLASS(FlowScriptNodeEditorProcedure);
	}
}


void uninitialize_flow_script_module(ModuleInitializationLevel p_level)
{
}
