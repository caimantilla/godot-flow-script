#ifndef FLOW_TYPE_DEFINITION_DIALOG_HPP
#define FLOW_TYPE_DEFINITION_DIALOG_HPP

#include "../core_types/flow_type.hpp"
#include "scene/gui/popup.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/text_edit.h"


// TODO: Write this GUI!
class FlowTypeDefinitionDialog final : public PopupPanel
{
	GDCLASS(FlowTypeDefinitionDialog, PopupPanel);

private:
	Ref<FlowType> edited_flow_type;

	LineEdit *type_id_line;
	LineEdit *type_name_line;
	TextEdit *type_description_box;

	void reset_edited_flow_type();

protected:
	static void _bind_methods();

public:
	void load_flow_type(const Ref<FlowType> p_flow_type);

	FlowTypeDefinitionDialog();
};


#endif
