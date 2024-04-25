#ifndef FLOW_SCRIPT_EDITOR_MENU_HPP
#define FLOW_SCRIPT_EDITOR_MENU_HPP

#include "scene/gui/margin_container.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"


class FlowScriptEditorMenu final : public MarginContainer
{
	GDCLASS(FlowScriptEditorMenu, MarginContainer);

private:
	MarginContainer *inner_container;
	HBoxContainer *bottom_spread;
	Button *btn_create_node;

	void update_style();
	int get_inner_container_margin() const;
	void request_node_creation();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	FlowScriptEditorMenu();
};


#endif
