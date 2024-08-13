#ifndef FLOW_SCRIPT_EDITOR_HPP
#define FLOW_SCRIPT_EDITOR_HPP


#include "flow_script.hpp"
#include "flow_script_graph.hpp"
#include "scene/resources/theme.h"
#include "scene/gui/box_container.h"


class FlowScriptEditor final : public VBoxContainer
{
	GDCLASS(FlowScriptEditor, VBoxContainer);

private:
	struct EditedFlowScript
	{
		FlowScriptGraph *graph;
		Ref<FlowScript> flow_script;
		String visible_identifier;
	};

private:
	Ref<Theme> graph_theme;
	FlowScriptGraph *graph;

	void remake_graph();
	void redraw_connections();

protected:
	void _notification(int p_what);

public:
	FlowScriptEditor(const Ref<Theme> &p_graph_theme);
	~FlowScriptEditor();
};


#endif // FLOW_SCRIPT_EDITOR_HPP
