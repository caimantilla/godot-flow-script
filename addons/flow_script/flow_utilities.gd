@tool
class_name FlowUtilities
extends RefCounted



const ID_LENGTH_DEFAULT: int = 6



enum CharacterType {
	BAN = 0,
	ALPHA = 1,
	NUMERIC = 2,
}


enum InterfaceLocation {
	BOTTOM_PANEL = 0,
	MAIN_SCREEN = 1,
}

const InterfaceLocation_HINT := PROPERTY_HINT_ENUM
const InterfaceLocation_HINT_STRING: String = "Bottom Panel:0,Main Screen:1"


const _SERIALIZABLE_TYPES_SET: Dictionary = {
	TYPE_STRING: true,
	TYPE_STRING_NAME: true,
	TYPE_INT: true,
	TYPE_FLOAT: true,
	TYPE_ARRAY: true,
	TYPE_DICTIONARY: true,
	TYPE_BOOL: true,
	TYPE_NIL: true,
	TYPE_PACKED_BYTE_ARRAY: true,
	TYPE_PACKED_STRING_ARRAY: true,
	TYPE_PACKED_FLOAT32_ARRAY: true,
	TYPE_PACKED_FLOAT64_ARRAY: true,
	TYPE_PACKED_INT32_ARRAY: true,
	TYPE_PACKED_INT64_ARRAY: true,
}


const _ID_PERMITTED_CHARACTER_SET: Dictionary = {
	'A': CharacterType.ALPHA,
	'B': CharacterType.ALPHA,
	'C': CharacterType.ALPHA,
	'D': CharacterType.ALPHA,
	'E': CharacterType.ALPHA,
	'F': CharacterType.ALPHA,
	'G': CharacterType.ALPHA,
	'H': CharacterType.ALPHA,
	'I': CharacterType.ALPHA,
	'J': CharacterType.ALPHA,
	'K': CharacterType.ALPHA,
	'L': CharacterType.ALPHA,
	'M': CharacterType.ALPHA,
	'N': CharacterType.ALPHA,
	'O': CharacterType.ALPHA,
	'P': CharacterType.ALPHA,
	'Q': CharacterType.ALPHA,
	'R': CharacterType.ALPHA,
	'S': CharacterType.ALPHA,
	'T': CharacterType.ALPHA,
	'U': CharacterType.ALPHA,
	'V': CharacterType.ALPHA,
	'W': CharacterType.ALPHA,
	'X': CharacterType.ALPHA,
	'Y': CharacterType.ALPHA,
	'Z': CharacterType.ALPHA,
	
	'a': CharacterType.ALPHA,
	'b': CharacterType.ALPHA,
	'c': CharacterType.ALPHA,
	'd': CharacterType.ALPHA,
	'e': CharacterType.ALPHA,
	'f': CharacterType.ALPHA,
	'g': CharacterType.ALPHA,
	'h': CharacterType.ALPHA,
	'i': CharacterType.ALPHA,
	'j': CharacterType.ALPHA,
	'k': CharacterType.ALPHA,
	'l': CharacterType.ALPHA,
	'm': CharacterType.ALPHA,
	'n': CharacterType.ALPHA,
	'o': CharacterType.ALPHA,
	'p': CharacterType.ALPHA,
	'q': CharacterType.ALPHA,
	'r': CharacterType.ALPHA,
	's': CharacterType.ALPHA,
	't': CharacterType.ALPHA,
	'u': CharacterType.ALPHA,
	'v': CharacterType.ALPHA,
	'w': CharacterType.ALPHA,
	'x': CharacterType.ALPHA,
	'y': CharacterType.ALPHA,
	'z': CharacterType.ALPHA,
	
	'1': CharacterType.NUMERIC,
	'2': CharacterType.NUMERIC,
	'3': CharacterType.NUMERIC,
	'4': CharacterType.NUMERIC,
	'5': CharacterType.NUMERIC,
	'6': CharacterType.NUMERIC,
	'7': CharacterType.NUMERIC,
	'8': CharacterType.NUMERIC,
	'9': CharacterType.NUMERIC,
	'0': CharacterType.NUMERIC,
}



static func is_type_serializable(p_type: Variant.Type) -> bool:
	return _SERIALIZABLE_TYPES_SET.has(p_type)


static func get_character_type(p_char: String) -> CharacterType:
	if _ID_PERMITTED_CHARACTER_SET.has(p_char):
		return _ID_PERMITTED_CHARACTER_SET[p_char] as CharacterType
	else:
		return CharacterType.BAN


static func generate_random_alphanumeric_id(p_length: int = ID_LENGTH_DEFAULT) -> String:
	var id: String = ""
	
	var character_set: Array = _ID_PERMITTED_CHARACTER_SET.keys()
	
	for i in p_length:
		var new_character: String = character_set.pick_random()
		
		if i == 0:
			while _ID_PERMITTED_CHARACTER_SET[new_character] == CharacterType.NUMERIC:
				new_character = character_set.pick_random()
		
		id += new_character
	
	return id


static func is_alphanumeric_id_valid(p_id: String) -> bool:
	if p_id.is_empty():
		return false
	
	for i in p_id.length():
		var character: String = p_id[i]
		
		if not _ID_PERMITTED_CHARACTER_SET.has(character):
			return false
		
		if i == 0 and _ID_PERMITTED_CHARACTER_SET[character] == CharacterType.NUMERIC:
			return false
	
	return true
