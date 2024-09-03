#include "register_types.h"
#include "flow_script.hpp"
#include "flow_script_node.hpp"
#include "flow_script_node_translation.hpp"
#include "flow_script_node_custom.hpp"
#include "flow_script_bridge.hpp"

// Extra classes
#include "flow_script_timer_proxy.hpp"

// Include nodes
#include "nodes/flow_script_node_procedure.hpp"
#include "nodes/flow_script_node_text_comment.hpp"
#include "nodes/flow_script_node_return_expression_result.hpp"
#include "nodes/flow_script_node_wait_duration_fixed_seconds.hpp"
#include "nodes/flow_script_node_wait_duration_expression_result.hpp"
#include "nodes/flow_script_node_set_expression_result_to_variable_local.hpp"
#include "nodes/flow_script_node_set_expression_result_to_variable_global.hpp"
#include "nodes/flow_script_node_multi_branch_execute_parallel.hpp"
#include "nodes/flow_script_node_multi_branch_execute_sequential.hpp"
#include "nodes/flow_script_node_loop_while_expression_result_true.hpp"
#include "nodes/flow_script_node_boolean_branch_expression.hpp"

#ifdef TOOLS_ENABLED

// Include node editors
#include "editor/flow_script_node_type_db.hpp"
#include "editor/plugins/flow_script_editor_plugin.hpp"
#include "editor/nodes/flow_script_node_editor.hpp"
#include "editor/nodes/flow_script_node_editor_procedure.hpp"
#include "editor/nodes/flow_script_node_editor_text_comment.hpp"
#include "editor/nodes/flow_script_node_editor_return_expression_result.hpp"
#include "editor/nodes/flow_script_node_editor_loop_while_expression_result_true.hpp"
#include "editor/nodes/flow_script_node_editor_set_expression_result_to_variable.hpp"
#include "editor/nodes/flow_script_node_editor_boolean_branch_expression.hpp"
#include "editor/nodes/flow_script_node_editor_multi_branch_execute.hpp"
#include "editor/nodes/flow_script_node_editor_wait_duration_fixed_seconds.hpp"
#include "editor/nodes/flow_script_node_editor_wait_duration_expression_result.hpp"

#endif // TOOLS_ENABLED


void initialize_flow_script_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		// Non-instantiable core classes
		GDREGISTER_ABSTRACT_CLASS(FlowScriptNode);
		GDREGISTER_ABSTRACT_CLASS(FlowScriptNodeContext);

		// Instantiable core classes
		GDREGISTER_CLASS(FlowScript);
		GDREGISTER_CLASS(FlowScriptNodeTranslation);
		GDREGISTER_CLASS(FlowScriptBridge);
		GDREGISTER_CLASS(FlowScriptNodeCustom);

		// Extra classes
		GDREGISTER_CLASS(FlowScriptTimerProxy);

		// Register nodes
		GDREGISTER_CLASS(FlowScriptNodeProcedure);
		GDREGISTER_CLASS(FlowScriptNodeTextComment);
		GDREGISTER_CLASS(FlowScriptNodeReturnExpressionResult);
		GDREGISTER_ABSTRACT_CLASS(FlowScriptNodeWaitDuration);
		GDREGISTER_CLASS(FlowScriptNodeWaitDurationFixedSeconds);
		GDREGISTER_CLASS(FlowScriptNodeWaitDurationExpressionResult);
		GDREGISTER_ABSTRACT_CLASS(FlowScriptNodeMultiBranchExecute);
		GDREGISTER_CLASS(FlowScriptNodeMultiBranchExecuteParallel);
		GDREGISTER_CLASS(FlowScriptNodeMultiBranchExecuteSequential);
		GDREGISTER_ABSTRACT_CLASS(FlowScriptNodeSetExpressionResultToVariable);
		GDREGISTER_CLASS(FlowScriptNodeSetExpressionResultToVariableLocal);
		GDREGISTER_CLASS(FlowScriptNodeSetExpressionResultToVariableGlobal);
		GDREGISTER_CLASS(FlowScriptNodeLoopWhileExpressionResultTrue);
		GDREGISTER_CLASS(FlowScriptNodeBooleanBranchExpression);
	}

#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		// Register node editors
		GDREGISTER_CLASS(FlowScriptNodeEditor);
		GDREGISTER_CLASS(FlowScriptNodeEditorProcedure);
		GDREGISTER_CLASS(FlowScriptNodeEditorTextComment);
		GDREGISTER_CLASS(FlowScriptNodeEditorSetExpressionResultToVariable);
		GDREGISTER_CLASS(FlowScriptNodeEditorMultiBranchExecute);
		GDREGISTER_CLASS(FlowScriptNodeEditorBooleanBranchExpression);
		GDREGISTER_CLASS(FlowScriptNodeEditorWaitDurationFixedSeconds);
		GDREGISTER_CLASS(FlowScriptNodeEditorWaitDurationExpressionResult);
		GDREGISTER_CLASS(FlowScriptNodeEditorReturnExpressionResult);
		GDREGISTER_CLASS(FlowScriptNodeEditorLoopWhileExpressionResultTrue);
	}
	else if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR)
	{
		EditorPlugins::add_by_type<FlowScriptEditorPlugin>();
	}
#endif // TOOLS_ENABLED
}


void uninitialize_flow_script_module(ModuleInitializationLevel p_level)
{
}
