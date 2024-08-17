#ifndef FLOW_SCRIPT_EDITOR_EXPRESSION_DISPLAY_BOX_HPP
#define FLOW_SCRIPT_EDITOR_EXPRESSION_DISPLAY_BOX_HPP


#include "scene/gui/control.h"


class Label;
class RichTextLabel;


class FlowScriptEditorExpressionDisplayBox : public Control
{
	GDCLASS(FlowScriptEditorExpressionDisplayBox, Control);

private:
	enum VisibilityMode
	{
		VISIBILITY_MODE_UNDEFINED = 0,
		VISIBILITY_MODE_CONTENT = 1,
	};

private:
	String expression;
	VisibilityMode current_visible_mode = VISIBILITY_MODE_UNDEFINED;

	Label *undefined_label;
	RichTextLabel *content_label;

public:
	void set_expression(const String &p_content);
	String get_expression() const;
	void set_placeholder(const String &p_text);
	String get_placeholder() const;

	virtual Size2 get_minimum_size() const override;

	FlowScriptEditorExpressionDisplayBox();
};


#endif // FLOW_SCRIPT_EDITOR_EXPRESSION_DISPLAY_BOX_HPP
