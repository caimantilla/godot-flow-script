#ifndef FLOW_NODE_FACTORY_HPP
#define FLOW_NODE_FACTORY_HPP

#include "core/object/object.h"
#include "../core_types/flow_type.hpp"
#include "../core_types/flow_node.hpp"
#include "../core_types/flow_node_editor.hpp"

class FlowFactory final : public Object
{
	GDCLASS(FlowFactory, Object);

private:
	static FlowFactory *singleton;

public:
	static FlowFactory *get_singleton();

	FlowNode *create_flow_node_of_flow_type(const String &p_type_id);
	FlowNodeEditor *create_flow_node_editor_for_flow_type(const String &p_type_id);

	FlowFactory();
	~FlowFactory();
};

#endif
