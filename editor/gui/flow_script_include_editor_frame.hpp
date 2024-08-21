#ifndef FLOW_SCRIPT_INCLUDE_EDITOR_FRAME_HPP
#define FLOW_SCRIPT_INCLUDE_EDITOR_FRAME_HPP


#include "../../flow_script.hpp"
#include "scene/gui/graph_frame.h"
#include "scene/gui/button.h"


class FlowScriptIncludeEditorFrame : public GraphFrame
{
	GDCLASS(FlowScriptIncludeEditorFrame, GraphFrame);

private:
	Button *remove_script_button;

	void request_removal();
	void update_title();

protected:
	void _notification(int p_what);

public:
	FlowScriptIncludeID include_id = FlowScript::INCLUDE_FLOW_SCRIPT_ID_INVALID;
	Ref<FlowScript> parent_flow_script;
	Ref<FlowScript> edited_flow_script;

	FlowScriptIncludeEditorFrame();
};


#endif // FLOW_SCRIPT_INCLUDE_EDITOR_FRAME_HPP
