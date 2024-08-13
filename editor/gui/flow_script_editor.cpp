#include "flow_script_editor.hpp"
#include "editor/editor_node.h"
#include "editor/editor_string_names.h"
#include "scene/gui/box_container.h"


void FlowScriptEditor::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_THEME_CHANGED:
		{
			remake_graph();
		}
	}
}


void FlowScriptEditor::remake_graph()
{
}


void FlowScriptEditor::redraw_connections()
{
}


FlowScriptEditor::FlowScriptEditor(const Ref<Theme> &p_graph_theme)
{
	graph_theme = p_graph_theme;
}


FlowScriptEditor::~FlowScriptEditor()
{
}
