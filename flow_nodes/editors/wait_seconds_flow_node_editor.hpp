#ifndef WAIT_SECONDS_FLOW_NODE_EDITOR_HPP
#define WAIT_SECONDS_FLOW_NODE_EDITOR_HPP

#include "../wait_seconds_flow_node.hpp"
#include "../../core_types/flow_node_editor.hpp"
#include "scene/gui/spin_box.h"

class WaitSecondsFlowNodeEditor final : public FlowNodeEditor
{
	GDCLASS(WaitSecondsFlowNodeEditor, FlowNodeEditor);

private:
	SpinBox *duration_spinner;

	WaitSecondsFlowNode *get_ws_flow_node() const;

	void on_duration_spinner_value_changed(float p_value);

protected:
	virtual void _initialize() override;
	virtual void _clean_up() override;
	virtual void _flow_node_updated() override;
	virtual int _get_input_slot() const override;
	virtual void _set_out_going_connection(const Ref<FlowNodeEditorOutGoingConnectionParameters> p_out_going_connection) override;
	virtual TypedArray<FlowNodeEditorOutGoingConnectionParameters> _get_out_going_connections() const override;

public:
	WaitSecondsFlowNodeEditor();
};

#endif
