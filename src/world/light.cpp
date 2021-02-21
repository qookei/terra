#include <world/world.hpp>

#include <iostream>

#include <algorithm>

namespace {
} // namespace anonymous

namespace world {

uint32_t world_data::calculate_lighting_for(glm::ivec2 pos) {
	auto chunk = get(pos);
	if (!chunk)
		return 0;

	uint32_t affected_set = 0;

	auto add_light_at = [this, &affected_set, pos] (int x, int y, glm::vec4 v) {
		int tx = 0, ty = 0;
		if (x < 0) tx = -1;
		else if (x >= chunk_data::width) tx = 1;
		if (y < 0) ty = -1;
		else if (y >= chunk_data::height) ty = 1;

		auto tile_pos = tile_at({x, y});
		auto chunk = get(pos + glm::ivec2{tx, ty});
		if (!chunk)
			return;

		auto idx = tile_pos.x + tile_pos.y * chunk_data::width;
		auto aff_idx = (tx + 1) + (ty + 1) * 3;
		chunk->lights[aff_idx][idx].color += v;
		affected_set |= (1 << aff_idx);
	};

	for (int y = 0; y < chunk_data::height; y++) {
		for (int x = 0; x < chunk_data::width; x++) {
			// TODO: have world tile registry
			auto idx = x + y * chunk_data::width;
			auto tile = chunk->front[idx];
			if (tile != tile_id::torch && tile != tile_id::lava)
				continue;

			constexpr int radius = 10;
 
			for (int xx = -radius; xx <= radius; xx++) {
				for (int yy = -radius; yy <= radius; yy++) {
					int real_x = xx + x;
					int real_y = yy + y;

					constexpr float max = (radius - 1) * (radius - 1) * 2 - 0.04f;
					float dist = xx * xx + yy * yy + 10;

					float v = std::clamp((max - dist) / max, 0.f, 1.f);

					add_light_at(real_x, real_y, {v, v, v, 1.f});
				}
			}
		}
	}

	return affected_set;
}

} // namespace world
