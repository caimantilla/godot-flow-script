#include "flow_script_editor_expression_display_box.hpp"
#include "scene/gui/label.h"
#include "scene/gui/rich_text_label.h"


Size2 FlowScriptEditorExpressionDisplayBox::get_minimum_size() const
{
	switch (current_visible_mode)
	{
		case VISIBILITY_MODE_UNDEFINED:
			return undefined_label->get_minimum_size();
			break;
		case VISIBILITY_MODE_CONTENT:
			return content_label->get_minimum_size();
			break;
	}
}


void FlowScriptEditorExpressionDisplayBox::set_expression(const String &p_content)
{
	if (p_content == expression)
	{
		return;
	}
	expression = p_content;
	String used_content = expression.strip_edges();
	VisibilityMode new_visible_mode;
	if (expression.is_empty())
	{
		new_visible_mode = VISIBILITY_MODE_UNDEFINED;
		content_label->hide();
		content_label->set_text("");
		undefined_label->show();
	}
	else
	{
		new_visible_mode = VISIBILITY_MODE_CONTENT;
		undefined_label->hide();
		if (used_content.contains("\n"))
		{
			content_label->push_list(0, RichTextLabel::LIST_DOTS, false);
			content_label->add_text(used_content);
			content_label->pop();
		}
		else
		{
			content_label->set_text(used_content);
		}
		content_label->show();
	}
	if (current_visible_mode != new_visible_mode)
	{
		current_visible_mode = new_visible_mode;
		update_minimum_size();
	}
}


String FlowScriptEditorExpressionDisplayBox::get_expression() const
{
	return expression;
}


void FlowScriptEditorExpressionDisplayBox::set_placeholder(const String &p_text)
{
	undefined_label->set_text(p_text);
}


String FlowScriptEditorExpressionDisplayBox::get_placeholder() const
{
	return undefined_label->get_text();
}


FlowScriptEditorExpressionDisplayBox::FlowScriptEditorExpressionDisplayBox()
{
	set_mouse_filter(MOUSE_FILTER_PASS);

	undefined_label = memnew(Label);
	undefined_label->set_text(TTR("Undefined expression."));
	undefined_label->set_modulate(Color::named("red"));
	undefined_label->set_anchors_and_offsets_preset(PRESET_FULL_RECT);
	undefined_label->set_mouse_filter(MOUSE_FILTER_IGNORE);
	undefined_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	undefined_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	add_child(undefined_label);

	content_label = memnew(RichTextLabel);
	content_label->hide();
	content_label->set_auto_translate(false);
	content_label->set_anchors_and_offsets_preset(PRESET_FULL_RECT);
	content_label->set_mouse_filter(MOUSE_FILTER_IGNORE);
	content_label->set_fit_content(true);
	content_label->set_scroll_active(false);
	add_child(content_label);
}
