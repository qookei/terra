#include <world/world.hpp>
#include <iostream>
#include <glm/gtx/io.hpp>
#include <algorithm>
#include <deque>
#include <limits>
#include <unordered_set>
#include <random>

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

	struct light_prop {
		glm::ivec2 pos;
		glm::vec4 color;
		float level;
	};

	auto propagate_light = [&add_light_at, this] (light_prop p) {
		std::deque<light_prop> propagation_queue;
		std::unordered_set<glm::ivec2> visited;

		propagation_queue.push_back(p);

		constexpr auto epsilon = std::numeric_limits<float>::epsilon();

		while (propagation_queue.size()) {
			auto p = propagation_queue.front();
			propagation_queue.pop_front();

			float v = p.level;
			add_light_at(p.pos.x, p.pos.y, p.color * glm::vec4{v, v, v, 1.f});
			visited.insert(p.pos);
			v -= 0.12f;

			if (v < epsilon)
				continue;

			constexpr glm::ivec2 dirs[] = {
				{1, 0}, {-1, 0},
				{0, 1}, {0, -1}
			};

			for (auto dir : dirs) {
				if (auto pos = p.pos + dir; !visited.count(pos)) {
					propagation_queue.push_back({pos, p.color, v});
					visited.insert(pos);
				}
			}
		}
	};

	std::deque<light_prop> light_queue;

	for (int y = 0; y < chunk_data::height; y++) {
		for (int x = 0; x < chunk_data::width; x++) {
			// TODO: have world tile registry
			auto idx = x + y * chunk_data::width;
			auto tile = chunk->front[idx];
			if (tile != tile_id::torch && tile != tile_id::lava)
				continue;

			auto color = tile == tile_id::torch
				? glm::vec4{1.f, .64f, 0.f, 1.f}
				: glm::vec4{1.f, .2f, 0.f, 1.f};

			float v = tile == tile_id::torch ? 1.f : 0.75f;

			light_queue.push_back({{x, y}, color, v});
		}
	}

	while (light_queue.size()) {
		auto item = light_queue.front();
		light_queue.pop_front();

		propagate_light(item);
	}

	return affected_set;
}

} // namespace world
