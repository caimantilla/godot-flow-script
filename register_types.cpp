// Godot includes
#include "register_types.h"
#include "core/object/class_db.h"
#include "core/string/ustring.h"
#include "editor/editor_plugin.h"


// Core includes
#include "core_types/flow_type.hpp"
#include "core_types/flow_node.hpp"
#include "core_types/flow_node_execution_state.hpp"
#include "core_types/flow_node_editor_out_going_connection_parameters.hpp"
#include "core_types/flow_node_editor.hpp"
#include "core_types/flow_controller.hpp"
#include "core_types/flow_bridge.hpp"
#include "core_types/flow_fiber.hpp"
#include "core_types/flow_script.hpp"

// Singleton includes
#include "singletons/flow_config_manager.hpp"
#include "singletons/flow_type_db.hpp"
#include "singletons/flow_factory.hpp"

// FlowNode includes
#include "flow_nodes/assign_expression_to_local_variable_flow_node.hpp"
#include "flow_nodes/assign_expression_to_global_variable_flow_node.hpp"
#include "flow_nodes/assign_expression_to_variable_flow_node.hpp"
#include "flow_nodes/comment_flow_node.hpp"
#include "flow_nodes/execute_expression_flow_node.hpp"
#include "flow_nodes/expression_branch_flow_node.hpp"
#include "flow_nodes/multi_branch_execution_flow_node.hpp"
#include "flow_nodes/print_to_console_flow_node.hpp"
#include "flow_nodes/procedure_flow_node.hpp"
#include "flow_nodes/return_expression_flow_node.hpp"
#include "flow_nodes/sequential_branch_execution_flow_node.hpp"
#include "flow_nodes/simultaneous_branch_execution_flow_node.hpp"
#include "flow_nodes/wait_seconds_flow_node.hpp"
#include "flow_nodes/while_expression_true_loop_flow_node.hpp"

// FlowNodeEditor includes
#include "flow_nodes/editors/assign_expression_to_variable_flow_node_editor.hpp"
#include "flow_nodes/editors/comment_flow_node_editor.hpp"
#include "flow_nodes/editors/execute_expression_flow_node_editor.hpp"
#include "flow_nodes/editors/expression_branch_flow_node_editor.hpp"
#include "flow_nodes/editors/multi_branch_execution_flow_node_editor.hpp"
#include "flow_nodes/editors/print_to_console_flow_node_editor.hpp"
#include "flow_nodes/editors/procedure_flow_node_editor.hpp"
#include "flow_nodes/editors/return_expression_flow_node_editor.hpp"
#include "flow_nodes/editors/wait_seconds_flow_node_editor.hpp"
#include "flow_nodes/editors/while_expression_true_loop_flow_node_editor.hpp"

#ifdef TOOLS_ENABLED

// Editor includes
#include "editor/flow_script_editor_plugin.hpp"
#include "editor/editor_inspector_plugin_flow_node.hpp"

#endif

// Misc includes
#include "built_in_flow_types.hpp"


static void _register_singletons()
{
	memnew(BuiltInFlowTypes);
	memnew(FlowConfigManager);
	memnew(FlowTypeDB);
	memnew(FlowFactory);

	Engine::get_singleton()->add_singleton(Engine::Singleton("FlowConfigManager", FlowConfigManager::get_singleton(), "FlowConfigManager"));
	Engine::get_singleton()->add_singleton(Engine::Singleton("FlowTypeDB", FlowTypeDB::get_singleton(), "FlowTypeDB"));
	Engine::get_singleton()->add_singleton(Engine::Singleton("FlowFactory", FlowFactory::get_singleton(), "FlowFactory"));
}


static void _unregister_singletons()
{
	Engine::get_singleton()->remove_singleton("FlowConfigManager");
	Engine::get_singleton()->remove_singleton("FlowTypeDB");
	Engine::get_singleton()->remove_singleton("FlowFactory");

	memdelete(BuiltInFlowTypes::get_singleton());
	memdelete(FlowConfigManager::get_singleton());
	memdelete(FlowTypeDB::get_singleton());
	memdelete(FlowFactory::get_singleton());
}


static void _register_types()
{
	// Register singleton types
	GDREGISTER_CLASS(FlowConfigManager);
	GDREGISTER_CLASS(FlowTypeDB);
	GDREGISTER_CLASS(FlowFactory);

	// Register core types
	GDREGISTER_CLASS(FlowType);
	GDREGISTER_CLASS(FlowNode);
	GDREGISTER_CLASS(FlowNodeExecutionState);
	GDREGISTER_CLASS(FlowNodeEditorOutGoingConnectionParameters);
	GDREGISTER_CLASS(FlowNodeEditor);
	GDREGISTER_CLASS(FlowController);
	GDREGISTER_CLASS(FlowBridge);
	GDREGISTER_CLASS(FlowFiber);
	GDREGISTER_CLASS(FlowScript);

	// Register FlowNodes
	GDREGISTER_CLASS(ProcedureFlowNode);
	GDREGISTER_CLASS(ReturnExpressionFlowNode);
	GDREGISTER_VIRTUAL_CLASS(AssignExpressionToVariableFlowNode);
	GDREGISTER_CLASS(AssignExpressionToLocalVariableFlowNode);
	GDREGISTER_CLASS(AssignExpressionToGlobalVariableFlowNode);
	GDREGISTER_CLASS(CommentFlowNode);
	GDREGISTER_CLASS(ExecuteExpressionFlowNode);
	GDREGISTER_CLASS(ExpressionBranchFlowNode);
	GDREGISTER_VIRTUAL_CLASS(MultiBranchExecutionFlowNode);
	GDREGISTER_CLASS(SequentialBranchExecutionFlowNode);
	GDREGISTER_CLASS(SimultaneousBranchExecutionFlowNode);
	GDREGISTER_CLASS(WhileExpressionTrueLoopFlowNode);
	GDREGISTER_CLASS(PrintToConsoleFlowNode);
	GDREGISTER_CLASS(WaitSecondsFlowNode);

	// Register FlowNode editors
	GDREGISTER_CLASS(ProcedureFlowNodeEditor);
	GDREGISTER_CLASS(ReturnExpressionFlowNodeEditor);
	GDREGISTER_CLASS(AssignExpressionToVariableFlowNodeEditor);
	GDREGISTER_CLASS(CommentFlowNodeEditor);
	GDREGISTER_CLASS(ExecuteExpressionFlowNodeEditor);
	GDREGISTER_CLASS(ExpressionBranchFlowNodeEditor);
	GDREGISTER_CLASS(MultiBranchExecutionFlowNodeEditor);
	GDREGISTER_CLASS(PrintToConsoleFlowNodeEditor);
	GDREGISTER_CLASS(WaitSecondsFlowNodeEditor);
	GDREGISTER_CLASS(WhileExpressionTrueLoopFlowNodeEditor);
}


static void _unregister_types()
{
}


static void _register_flow_nodes()
{
	for (int i = 0; i < BuiltInFlowTypes::get_singleton()->get_type_count(); i++)
	{
		Ref<FlowType> type = BuiltInFlowTypes::get_singleton()->get_type_at(i);
		FlowTypeDB::get_singleton()->register_native_type(type);
	}
}


static void _unregister_flow_nodes()
{
	for (int i = 0; i < BuiltInFlowTypes::get_singleton()->get_type_count(); i++)
	{
		Ref<FlowType> type = BuiltInFlowTypes::get_singleton()->get_type_at(i);
		FlowTypeDB::get_singleton()->unregister_native_type(type);
	}
}


static void _register_editor()
{
#ifdef TOOLS_ENABLED

	if (Engine::get_singleton()->is_editor_hint())
	{
		EditorPlugins::add_by_type<FlowScriptEditorPlugin>();
	}

#endif
}


static void _unregister_editor()
{
#ifdef TOOLS_ENABLED
#endif
}


void initialize_flow_script_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
	{
		_register_singletons();
	}
	else if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		FlowConfigManager::get_singleton()->initialize_project_settings();

		_register_types();
		_register_flow_nodes();

		FlowTypeDB::get_singleton()->refresh();
	}
	else if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR)
	{
		_register_editor();
	}
}


void uninitialize_flow_script_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
	{
		_unregister_singletons();
	}
	else if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		_unregister_flow_nodes();
		_unregister_types();
	}
	else if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR)
	{
		_unregister_editor();
	}
}
