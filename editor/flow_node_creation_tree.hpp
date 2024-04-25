#ifndef FLOW_NODE_CREATION_TREE_HPP
#define FLOW_NODE_CREATION_TREE_HPP

#include "../singletons/flow_type_db.hpp"
#include "scene/gui/tree.h"


class FlowNodeCreationTree final : public Tree
{
	GDCLASS(FlowNodeCreationTree, Tree);

private:
	HashMap<String, TreeItem *> category_item_map;
	HashMap<String, TreeItem *> type_item_map;

	void init_signals();

	void create_category_if_necessary(const String &p_category_path);
	void clear_types();
	void reload_types();

	void on_item_selected();
	void on_item_activated();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	String get_selected_flow_node_type_id() const;

	FlowNodeCreationTree();
};


#endif
