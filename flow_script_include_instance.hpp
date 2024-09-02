#ifndef FLOW_SCRIPT_INCLUDE_INSTANCE_HPP
#define FLOW_SCRIPT_INCLUDE_INSTANCE_HPP


#include "core/object/ref_counted.h"


class FlowScript;


class FlowScriptIncludeInstance final
{
public:
	Ref<FlowScript> flow_script;
	Point2i position;

	bool is_valid() const;
};


#endif // FLOW_SCRIPT_INCLUDE_INSTANCE_HPP
