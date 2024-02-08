Visual scripting solution for Godot 4.
Not for scripting game mechanics!
Instead, you write game-specific nodes for scripting levels, NPCs, stuff like that. Simple behaviors.
Each script has pages which contain nodes. Use pages for organization.
Bear in mind that the pages themselves are only for organization, node names must be unique to the script.
You also need to set up your own class which extends FlowObject and implements the necessary methods (check the flow_object.gd script for details)
