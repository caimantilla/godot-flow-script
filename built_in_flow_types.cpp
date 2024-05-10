#include "built_in_flow_types.hpp"


BuiltInFlowTypes *BuiltInFlowTypes::singleton = nullptr;


BuiltInFlowTypes *BuiltInFlowTypes::get_singleton()
{
	return singleton;
}


void BuiltInFlowTypes::init_types()
{
	if (have_types_initialized)
	{
		return;
	}

	have_types_initialized = true;
	_init_types_internal();
}


List<Ref<FlowType>> *BuiltInFlowTypes::get_built_in_types()
{
	return &built_in_types;
}


int BuiltInFlowTypes::get_type_count() const
{
	return built_in_types.size();
}


Ref<FlowType> BuiltInFlowTypes::get_type_at(const int p_idx) const
{
	return built_in_types.get(p_idx);
}


void BuiltInFlowTypes::add_type(Ref<FlowType> p_type)
{
	built_in_types.push_back(p_type);
}


BuiltInFlowTypes::BuiltInFlowTypes()
{
	CRASH_COND(singleton != nullptr);
	singleton = this;

	init_types();
}


BuiltInFlowTypes::~BuiltInFlowTypes()
{
	singleton = nullptr;
}


void BuiltInFlowTypes::_init_types_internal()
{
	add_type(FlowType::create_native_type(
		"procedure",
		"Procedure",
		"Flow",
		"Marks the beginning of a Flow procedure.\nNodes of this type can be renamed.",
		"",
		"ProcedureFlowNode",
		"ProcedureFlowNodeEditor",
		true
	));
	add_type(FlowType::create_native_type(
		"return_expression",
		"Return",
		"Flow",
		"Returns the result of an expression to the caller.",
		"",
		"ReturnExpressionFlowNode",
		"ReturnExpressionFlowNodeEditor",
		false
	));
	add_type(FlowType::create_native_type(
		"floating_comment",
		"Comment",
		"Debug",
		"A disconnected text box.\nWrite whatever you want.",
		"",
		"CommentFlowNode",
		"CommentFlowNodeEditor",
		false
	));
	add_type(FlowType::create_native_type(
		"wait_seconds",
		"Wait Fixed Duration",
		"",
		"Pauses script execution for a defined duration.",
		"",
		"WaitSecondsFlowNode",
		"WaitSecondsFlowNodeEditor",
		false
	));
	add_type(FlowType::create_native_type(
		"print_to_console",
		"Print Message to Console",
		"Debug",
		"Prints a message to the console.\nFormat strings are supported, with the values sourced from the expression box. Use the {0}, {1}, etc. style syntax.",
		"",
		"PrintToConsoleFlowNode",
		"PrintToConsoleFlowNodeEditor",
		false
	));
	add_type(FlowType::create_native_type(
		"execute_expression",
		"Execute Expressions",
		"Flow",
		"Executes a series of expressions.\nNo result is returned.",
		"",
		"ExecuteExpressionFlowNode",
		"ExecuteExpressionFlowNodeEditor",
		false
	));
	add_type(FlowType::create_native_type(
		"expression_branch",
		"Conditional Branch",
		"Flow",
		"Branches depending on the result of an evaluated expression.",
		"",
		"ExpressionBranchFlowNode",
		"ExpressionBranchFlowNodeEditor",
		false
	));
	add_type(FlowType::create_native_type(
		"execute_external_flow_script",
		"Execute External Procedure",
		"Flow",
		"Executes a procedure from another FlowScript.",
		"",
		"ExecuteExternalFlowScriptFlowNode",
		"ExecuteExternalFlowScriptFlowNodeEditor",
		false
	));
	add_type(FlowType::create_native_type(
		"while_expression_true_loop",
		"While Loop",
		"Flow",
		"Repeats a section of code as long as the expressions written evaluate to true.",
		"",
		"WhileExpressionTrueLoopFlowNode",
		"WhileExpressionTrueLoopFlowNodeEditor",
		false
	));
	add_type(FlowType::create_native_type(
		"sequential_branch_execution",
		"Execute Branches Sequentially",
		"Flow",
		"Executes a list of branches one after another, waiting for each to complete before progressing to the next.",
		"",
		"SequentialBranchExecutionFlowNode",
		"SequentialBranchExecutionFlowNodeEditor",
		false
	));
	add_type(FlowType::create_native_type(
		"simultaneous_branch_execution",
		"Execute Branches Simultaneously",
		"Flow",
		"Executes a list of branches simultaneously, waiting for all of them to complete before progressing.",
		"",
		"SimultaneousBranchExecutionFlowNode",
		"SimultaneousBranchExecutionFlowNodeEditor",
		false
	));
	add_type(FlowType::create_native_type(
		"assign_expression_to_local_variable",
		"Set Local Variable",
		"Variable",
		"Assigns a local variable to the result of some expression.",
		"",
		"AssignExpressionToLocalVariableFlowNode",
		"AssignExpressionToVariableFlowNodeEditor",
		false
	));
	add_type(FlowType::create_native_type(
		"assign_expression_to_global_variable",
		"Set Global Variable",
		"Variable",
		"Assigns a global variable to the result of some expression.",
		"",
		"AssignExpressionToGlobalVariableFlowNode",
		"AssignExpressionToVariableFlowNodeEditor",
		false
	));
}
