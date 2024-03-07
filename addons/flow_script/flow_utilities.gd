@tool
class_name FlowUtilities
extends RefCounted



const ID_LENGTH_DEFAULT: int = 6



enum CharacterType {
	BAN = 0,
	ALPHA_UPPER = 1,
	ALPHA_LOWER = 2,
	NUMERIC = 3,
	SYMBOL = 4,
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
	'A': CharacterType.ALPHA_UPPER,
	'B': CharacterType.ALPHA_UPPER,
	'C': CharacterType.ALPHA_UPPER,
	'D': CharacterType.ALPHA_UPPER,
	'E': CharacterType.ALPHA_UPPER,
	'F': CharacterType.ALPHA_UPPER,
	'G': CharacterType.ALPHA_UPPER,
	'H': CharacterType.ALPHA_UPPER,
	'I': CharacterType.ALPHA_UPPER,
	'J': CharacterType.ALPHA_UPPER,
	'K': CharacterType.ALPHA_UPPER,
	'L': CharacterType.ALPHA_UPPER,
	'M': CharacterType.ALPHA_UPPER,
	'N': CharacterType.ALPHA_UPPER,
	'O': CharacterType.ALPHA_UPPER,
	'P': CharacterType.ALPHA_UPPER,
	'Q': CharacterType.ALPHA_UPPER,
	'R': CharacterType.ALPHA_UPPER,
	'S': CharacterType.ALPHA_UPPER,
	'T': CharacterType.ALPHA_UPPER,
	'U': CharacterType.ALPHA_UPPER,
	'V': CharacterType.ALPHA_UPPER,
	'W': CharacterType.ALPHA_UPPER,
	'X': CharacterType.ALPHA_UPPER,
	'Y': CharacterType.ALPHA_UPPER,
	'Z': CharacterType.ALPHA_UPPER,
	
	'a': CharacterType.ALPHA_LOWER,
	'b': CharacterType.ALPHA_LOWER,
	'c': CharacterType.ALPHA_LOWER,
	'd': CharacterType.ALPHA_LOWER,
	'e': CharacterType.ALPHA_LOWER,
	'f': CharacterType.ALPHA_LOWER,
	'g': CharacterType.ALPHA_LOWER,
	'h': CharacterType.ALPHA_LOWER,
	'i': CharacterType.ALPHA_LOWER,
	'j': CharacterType.ALPHA_LOWER,
	'k': CharacterType.ALPHA_LOWER,
	'l': CharacterType.ALPHA_LOWER,
	'm': CharacterType.ALPHA_LOWER,
	'n': CharacterType.ALPHA_LOWER,
	'o': CharacterType.ALPHA_LOWER,
	'p': CharacterType.ALPHA_LOWER,
	'q': CharacterType.ALPHA_LOWER,
	'r': CharacterType.ALPHA_LOWER,
	's': CharacterType.ALPHA_LOWER,
	't': CharacterType.ALPHA_LOWER,
	'u': CharacterType.ALPHA_LOWER,
	'v': CharacterType.ALPHA_LOWER,
	'w': CharacterType.ALPHA_LOWER,
	'x': CharacterType.ALPHA_LOWER,
	'y': CharacterType.ALPHA_LOWER,
	'z': CharacterType.ALPHA_LOWER,
	
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
	
	'_': CharacterType.SYMBOL,
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
		while _ID_PERMITTED_CHARACTER_SET[new_character] != CharacterType.ALPHA_LOWER:
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
		
		var character_type: CharacterType = _ID_PERMITTED_CHARACTER_SET[character]
		if i == 0:
			if character_type != CharacterType.ALPHA_UPPER and character_type != CharacterType.ALPHA_LOWER:
				return false
		
		if character_type == CharacterType.BAN:
			return false
	
	return true
