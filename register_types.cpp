#include "register_types.h"

#include <array>

// Core classes
#include "flow_script_constants.hpp"
#include "flow_script.hpp"
#include "flow_script_node.hpp"
#include "flow_script_node_custom.hpp"
#include "flow_script_bridge.hpp"
#include "flow_script_node_context.hpp"

// Built-in interface classes
#include "flow_script_built_in_node_interface.hpp"
#include "flow_script_built_in_timer_proxy.hpp"

// Built-in node classes
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

// Editor classes
#include "editor/flow_script_node_type_info.hpp"
#include "editor/flow_script_node_type_db.hpp"
#include "editor/plugins/flow_script_editor_plugin.hpp"
#include "editor/gui/flow_script_editor_expression_display_box.hpp"

// Built-in node editor classes
#include "editor/nodes/flow_script_node_editor.hpp"
#include "editor/nodes/flow_script_node_editor_placeholder.hpp"
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


#ifdef TOOLS_ENABLED

static const std::array BUILT_IN_FLOW_SCRIPT_NODE_TYPE_LIST = {
	FlowScriptNodeTypeInfo {
		.enabled = true,
		.type_name = "Procedure",
		.type_category = "Built-In",
		.type_description = "A named entrypoint into the FlowScript.",
		.editable_name = true,
		.node_native_class_name = "FlowScriptNodeProcedure",
		.editor_native_class_name = "FlowScriptNodeEditorProcedure",
	},
	FlowScriptNodeTypeInfo {
		.enabled = true,
		.type_name = "Comment",
		.type_category = "Built-In",
		.type_description = "A box to take notes in.",
		.editable_size = true,
		.node_native_class_name = "FlowScriptNodeTextComment",
		.editor_native_class_name = "FlowScriptNodeEditorTextComment",
	},
	FlowScriptNodeTypeInfo {
		.enabled = true,
		.type_name = "Evaluate and Return Expression",
		.type_category = "Built-In/Logic",
		.type_description = "Returns the result of an expression evaluation to the caller.",
		.node_native_class_name = "FlowScriptNodeReturnExpressionResult",
		.editor_native_class_name = "FlowScriptNodeEditorReturnExpressionResult",
	},
	FlowScriptNodeTypeInfo {
		.enabled = true,
		.type_name = "While Loop Expression",
		.type_category = "Built-In/Logic/Looping",
		.type_description = "Loops while an expression result is true.",
		.node_native_class_name = "FlowScriptNodeLoopWhileExpressionResultTrue",
		.editor_native_class_name = "FlowScriptNodeEditorLoopWhileExpressionResultTrue",
	},
	FlowScriptNodeTypeInfo {
		.enabled = true,
		.type_name = "Branch Expression",
		.type_category = "Built-In/Logic/Branching",
		.type_description = "Branches based on the result of a list of expression evaluations.",
		.node_native_class_name = "FlowScriptNodeBooleanBranchExpression",
		.editor_native_class_name = "FlowScriptNodeEditorBooleanBranchExpression",
	},
	FlowScriptNodeTypeInfo {
		.enabled = true,
		.type_name = "Execute Sequential Branches",
		.type_category = "Built-In/Concurrency",
		.type_description = "Executes a list of branches in order, one after the other, and then advances.",
		.node_native_class_name = "FlowScriptNodeMultiBranchExecuteSequential",
		.editor_native_class_name = "FlowScriptNodeEditorMultiBranchExecute",
	},
	FlowScriptNodeTypeInfo {
		.enabled = true,
		.type_name = "Execute Parallel Branches",
		.type_category = "Built-In/Concurrency",
		.type_description = "Triggers execution of a list of branches all at once, then advances once all the branches have finished execution.",
		.node_native_class_name = "FlowScriptNodeMultiBranchExecuteParallel",
		.editor_native_class_name = "FlowScriptNodeEditorMultiBranchExecute",
	},
	FlowScriptNodeTypeInfo {
		.enabled = true,
		.type_name = "Assign Local Variable",
		.type_category = "Built-In/Variables",
		.type_description = "Evaluates an expression, then assigns the result to a local variable.",
		.node_native_class_name = "FlowScriptNodeSetExpressionResultToVariableLocal",
		.editor_native_class_name = "FlowScriptNodeEditorSetExpressionResultToVariable",
	},
	FlowScriptNodeTypeInfo {
		.enabled = true,
		.type_name = "Assign Global Variable",
		.type_category = "Built-In/Variables",
		.type_description = "Evaluates an expression, then assigns the result to a global variable.",
		.node_native_class_name = "FlowScriptNodeSetExpressionResultToVariableGlobal",
		.editor_native_class_name = "FlowScriptNodeEditorSetExpressionResultToVariable",
	},
	FlowScriptNodeTypeInfo {
		.enabled = true,
		.type_name = "Wait Fixed Seconds",
		.type_category = "Built-In/Timing",
		.type_description = "Waits a defined amount of time, in seconds.",
		.node_native_class_name = "FlowScriptNodeWaitDurationFixedSeconds",
		.editor_native_class_name = "FlowScriptNodeEditorWaitDurationFixedSeconds",
	},
	FlowScriptNodeTypeInfo {
		.enabled = true,
		.type_name = "Wait Expression",
		.type_category = "Built-In/Timing",
		.type_description = "Waits the number of seconds evaluated from an expression.",
		.node_native_class_name = "FlowScriptNodeWaitDurationExpressionResult",
		.editor_native_class_name = "FlowScriptNodeEditorWaitDurationExpressionResult",
	},
};

#endif // TOOLS_ENABLED


void initialize_flow_script_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		// Non-instantiable core classes
		GDREGISTER_ABSTRACT_CLASS(FlowScriptConstants);
		GDREGISTER_ABSTRACT_CLASS(FlowScriptNode);
		GDREGISTER_ABSTRACT_CLASS(FlowScriptNodeContext);

		// Instantiable core classes
		GDREGISTER_CLASS(FlowScript);
		GDREGISTER_CLASS(FlowScriptBridge);
		GDREGISTER_CLASS(FlowScriptNodeCustom);

		// Extra classes
		GDREGISTER_CLASS(FlowScriptBuiltInNodeInterface);
		GDREGISTER_CLASS(FlowScriptBuiltInTimerProxy);

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
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
	{
		// Initialize singleton
		memnew(FlowScriptNodeTypeDB);
	}
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
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR)
	{
		// Register built-in node types
		for (const FlowScriptNodeTypeInfo &curr_type : BUILT_IN_FLOW_SCRIPT_NODE_TYPE_LIST)
		{
			FlowScriptNodeTypeDB::get_singleton()->add_type(curr_type);
		}
		// And of course create the plugin
		EditorPlugins::add_by_type<FlowScriptEditorPlugin>();
	}
#endif // TOOLS_ENABLED
}


void uninitialize_flow_script_module(ModuleInitializationLevel p_level)
{
#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
	{
		memdelete(FlowScriptNodeTypeDB::get_singleton());
	}
#endif // TOOLS_ENABLED
}
