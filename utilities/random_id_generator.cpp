#include "random_id_generator.hpp"


static const char32_t PERMITTED_CHARACTERS[] = {
	'a',
	'b',
	'c',
	'd',
	'e',
	'f',
	'g',
	'h',
	'i',
	'j',
	'k',
	'l',
	'm',
	'n',
	'o',
	'p',
	'q',
	'r',
	's',
	't',
	'u',
	'v',
	'w',
	'x',
	'y',
	'z',
};


String RandomIDGenerator::generate_random_id(uint8_t p_length)
{
	String random_id;
	random_id.resize(p_length);

	for (uint8_t i = 0; i < p_length; i++)
	{
		int new_char_idx = rng->randi_range(0, permitted_character_count - 1);
		random_id.set(i, PERMITTED_CHARACTERS[new_char_idx]);
	}

	return random_id;
}


RandomIDGenerator::RandomIDGenerator(Ref<RandomNumberGenerator> p_rng)
{
	rng = p_rng;
	permitted_character_count = sizeof(PERMITTED_CHARACTERS) / sizeof(char32_t);
}
