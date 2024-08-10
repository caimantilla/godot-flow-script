#ifndef FLOW_SCRIPT_EDITOR_HPP
#define FLOW_SCRIPT_EDITOR_HPP


#include "flow_script_graph.hpp"
#include "scene/resources/theme.h"
#include "scene/gui/box_container.h"


class FlowScriptEditor final : public VBoxContainer
{
	GDCLASS(FlowScriptEditor, VBoxContainer);

private:
	FlowScriptGraph *graph;

	void remake_graph();

protected:
	void _notification(int p_what);

public:
	FlowScriptEditor(const Ref<Theme> &p_graph_theme);
	~FlowScriptEditor();
};


#endif // FLOW_SCRIPT_EDITOR_HPP
