#ifndef FLOW_SCRIPT_INCLUDE_EDITOR_FRAME_HPP
#define FLOW_SCRIPT_INCLUDE_EDITOR_FRAME_HPP


#include "flow_script.hpp"
#include "scene/gui/graph_frame.h"


class FlowScriptIncludeEditorFrame : public GraphFrame
{
	GDCLASS(FlowScriptIncludeEditorFrame, GraphFrame);

public:
	Ref<FlowScript> parent_flow_script;
	FlowScriptIncludeID include_id = FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID;

	FlowScriptIncludeEditorFrame();
};


#endif // FLOW_SCRIPT_INCLUDE_EDITOR_FRAME_HPP
