Visual scripting solution for Godot 4.4+.
Not for scripting game mechanics!
Instead, you write game-specific nodes for scripting levels, NPCs, dialogues, stuff like that. Simple behaviors.
You also need to set up your own class which extends FlowScriptBridge and implements the necessary methods. This is your script instance. The recommended usage is to extend it for level-specific text scripting where needed, so it functions as a general game scripting layer.
Type-casting the bridge is the expected usage.
There is also FlowScriptBuiltInNodeInterface, which must be extended for the built-in FlowScriptNodes to function.
