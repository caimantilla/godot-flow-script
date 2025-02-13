#ifndef FLOW_SCRIPT_TYPEDEFS_HPP
#define FLOW_SCRIPT_TYPEDEFS_HPP


#include "core/typedefs.h"
#include "core/templates/vector.h"


typedef int32_t FlowScriptIncludeID;
typedef int32_t FlowScriptNodeID;
typedef int32_t FlowScriptExecutionFiberID;
typedef uint32_t FlowScriptNodeConnectionListNo;
typedef uint32_t FlowScriptNodeConnectionListSlotNo;
typedef FlowScriptNodeConnectionListSlotNo FlowScriptNodeConnectionListLength;

typedef Vector<FlowScriptIncludeID> PackedFlowScriptIncludeIDArray;
typedef Vector<FlowScriptNodeID> PackedFlowScriptNodeIDArray;
typedef Vector<FlowScriptExecutionFiberID> PackedFlowScriptExecutionFiberIDArray;


#endif // FLOW_SCRIPT_TYPEDEFS_HPP
