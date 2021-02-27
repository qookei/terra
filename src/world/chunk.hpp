#pragma once

#include <glm/vec4.hpp>
#include <world/tile_ids.hpp>
#include <array>
#include <bitset>

namespace world {

struct light_data {
	float level;
	glm::vec4 color;
};

struct chunk_data {
	static constexpr int width = 32;
	static constexpr int height = 32;

	std::array<tile_id, width * height> front{};
	std::array<tile_id, width * height> back{};

	std::array<light_data, width * height * 9> lights{};
	std::bitset<9> light_affected_set{};

	light_data &light_at(int neigh, int x, int y) {
		return lights[x * 9 + neigh + y * width * 9];
	}
};

} // namespace world
