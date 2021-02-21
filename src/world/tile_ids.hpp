#pragma once

#include <stdint.h>

namespace world {

enum class tile_id : uint32_t {
	air = 0,
	grass,
	dirt,
	stone,
	cobble,
	iron_ore,
	coal_ore,
	log,
	leaves,
	planks,
	water,
	ice,
	torch,
	lava,
	sand,
	cactus
};

} // namespace world
