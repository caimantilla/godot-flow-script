Visual scripting solution for Godot 4.3+.
Not for scripting game mechanics!
Instead, you write game-specific nodes for scripting levels, NPCs, dialogues, stuff like that. Simple behaviors.
You also need to set up your own class which extends FlowBridge and implements the necessary methods. This is your script instance. The recommended usage is to extend it for level-specific text scripting where needed, so it functions as a general game scripting layer.
