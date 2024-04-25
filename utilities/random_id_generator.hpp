#ifndef RANDOM_ID_GENERATOR_HPP
#define RANDOM_ID_GENERATOR_HPP

#include "core/string/ustring.h"
#include "core/math/random_number_generator.h"

class RandomIDGenerator
{
private:
	int permitted_character_count;
	Ref<RandomNumberGenerator> rng;

public:
	String generate_random_id(uint8_t p_length);

	RandomIDGenerator(Ref<RandomNumberGenerator> p_rng);
};

#endif
