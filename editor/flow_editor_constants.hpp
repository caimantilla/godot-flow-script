#ifndef FLOW_EDITOR_CONSTANTS_HPP
#define FLOW_EDITOR_CONSTANTS_HPP

class FlowEditorConstants
{
public:
	enum FlowGUIEditorLocation
	{
		FLOW_EDITOR_GUI_LOCATION_MAIN_SCREEN = 0,
		FLOW_EDITOR_GUI_LOCATION_BOTTOM_PANEL = 1,
	};

	enum FlowNodeCreationDialogAppearPosition
	{
		FLOW_NODE_CREATION_DIALOG_APPEAR_POSITION_SCREEN_CENTER = 0,
		FLOW_NODE_CREATION_DIALOG_APPEAR_POSITION_UNDER_MOUSE = 1,
	};
};

#endif