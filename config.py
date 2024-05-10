# config.py


def can_build(env, platform):
	return True


def configure(env):
	pass


def get_doc_path():
	return "doc_classes"


def get_doc_classes():
	names = [
		"FlowConfigManager",
		"FlowFactory",
		"FlowTypeDB",

		"FlowScript",
		"FlowType",
		"FlowNode",
		"FlowNodeExecutionState",
		"FlowFiber",
		"FlowController",
		"FlowBridge",
		"FlowNodeEditor",
		"FlowNodeEditorOutGoingConnectionParameters",

		"AssignExpressionToVariableFlowNode",
		"AssignExpressionToLocalVariableFlowNode",
		"AssignExpressionToGlobalVariableFlowNode",
		"CommentFlowNode",
		"ExecuteExpressionFlowNode",
		"ExecuteExternalFlowScriptFlowNode",
		"ExpressionBranchFlowNode",
		"MultiBranchExecutionFlowNode",
		"PrintToConsoleFlowNode",
		"ProcedureFlowNode",
		"ReturnExpressionFlowNode",
		"SequentialBranchExecutionFlowNode",
		"SimultaneousBranchExecutionFlowNode",
		"WaitSecondsFlowNode",
		"WhileExpressionTrueLoopFlowNode",

		"AssignExpressionToVariableFlowNodeEditor",
		"CommentFlowNodeEditor",
		"ExecuteExpressionFlowNodeEditor",
		"ExecuteExternalFlowScriptFlowNodeEditor",
		"ExpressionBranchFlowNodeEditor",
		"MultiBranchExecutionFlowNodeEditor",
		"PrintToConsoleFlowNodeEditor",
		"ProcedureFlowNodeEditor",
		"ReturnExpressionFlowNodeEditor",
		"SequentialBranchExecutionFlowNodeEditor",
		"SimultaneousBranchExecutionFlowNodeEditor",
		"WaitSecondsFlowNodeEditor",
		"WhileExpressionTrueLoopFlowNodeEditor",
	]

	return names
