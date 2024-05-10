#ifndef FLOW_CONSTANTS_HPP
#define FLOW_CONSTANTS_HPP

#include "core/variant/variant.h"


typedef int32_t FlowNodeID;
#define TYPE_FLOW_NODE_ID Variant::INT
typedef PackedInt32Array FlowNodeIDArray;
#define TYPE_FLOW_NODE_ID_ARRAY Variant::PACKED_INT32_ARRAY

const FlowNodeID FLOW_NODE_ID_NIL = 0;
const FlowNodeID FLOW_NODE_ID_MIN = 1;
const FlowNodeID FLOW_NODE_ID_MAX = 9999;

typedef int32_t FlowFiberID;
#define TYPE_FLOW_FIBER_ID Variant::INT
typedef PackedInt32Array FlowFiberIDArray;
#define TYPE_FLOW_FIBER_ID_ARRAY Variant::PACKED_INT32_ARRAY

const FlowFiberID FLOW_FIBER_ID_NIL = 0;
const FlowFiberID FLOW_FIBER_ID_MIN = 1;
const FlowFiberID FLOW_FIBER_ID_MAX = 999;

#endif
