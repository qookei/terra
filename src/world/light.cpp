#include <world/world.hpp>
#include <algorithm>
#include <deque>
#include <limits>
#include <glm/gtx/hash.hpp>

namespace {
	inline glm::ivec2 tile_to_neigh_offset(glm::ivec2 pos) {
		int tx = 0, ty = 0;
		if (pos.x < 0) tx = -1;
		else if (pos.x >= world::chunk_data::width) tx = 1;
		if (pos.y < 0) ty = -1;
		else if (pos.y >= world::chunk_data::height) ty = 1;

		return {tx, ty};
	}

	inline int tile_to_neigh_idx(glm::ivec2 pos) {
		auto off = tile_to_neigh_offset(pos);

		return (off.x + 1) + (off.y + 1) * 3;
	}

	// TODO: further optimization could be done by checking neighboring chunks
	inline bool is_not_surrounded(world::chunk_data *chunk, glm::ivec2 pos, world::tile_id id) {
		if (pos.x == 0 || pos.x == world::chunk_data::width - 1) return true;
		if (pos.y == 0 || pos.y == world::chunk_data::height - 1) return true;

		return chunk->front[(pos.x - 1) + pos.y * world::chunk_data::width] != id
			|| chunk->front[(pos.x + 1) + pos.y * world::chunk_data::width] != id
			|| chunk->front[pos.x + (pos.y - 1) * world::chunk_data::width] != id
			|| chunk->front[pos.x + (pos.y + 1) * world::chunk_data::width] != id;
	}
} // namespace anonymous

namespace world {

std::bitset<9> world_data::calculate_lighting_for(glm::ivec2 pos) {
	auto chunk = get(pos);
	if (!chunk)
		return 0;

	auto old_aff_set = chunk->light_affected_set;
	old_aff_set.set(4); // This chunk is always affected
	for (int i = 0; i < 9; i++) {
		if (!old_aff_set.test(i))
			continue;

		int tx = (i % 3) - 1;
		int ty = (i / 3) - 1;
		auto other = get(pos + glm::ivec2{tx, ty});
		if (!other)
			continue;

		for (int j = 0; j < chunk_data::width * chunk_data::height; j++)
			other->lights[i + j * 9] = {};
	}

	std::bitset<9> affected_set = 0;

	auto add_light_at = [this, &affected_set, pos] (glm::ivec2 put_pos,
			glm::vec4 color, float level) {
		auto off = tile_to_neigh_offset(put_pos);
		auto aff_idx = tile_to_neigh_idx(put_pos);

		auto tile_pos = tile_at(put_pos);
		auto chunk = get(pos + off);
		if (!chunk)
			return;

		chunk->light_at(aff_idx, tile_pos.x, tile_pos.y).color += color * level;
		chunk->light_at(aff_idx, tile_pos.x, tile_pos.y).level += level;
		affected_set.set(aff_idx);
	};

	struct light_prop {
		glm::ivec2 origin;
		tile_id origin_tile;
		glm::ivec2 pos;
		glm::vec4 color;
		float level;
		float dropoff_amp;
	};

	std::array<
		std::bitset<
			chunk_data::width * chunk_data::height
		>,
		chunk_data::width * 3 * chunk_data::height * 3
	> visited{};
	constexpr auto epsilon = std::numeric_limits<float>::epsilon();
	std::deque<light_prop> propagation_queue;

	for (int y = 0; y < chunk_data::height; y++) {
		for (int x = 0; x < chunk_data::width; x++) {
			auto idx = x + y * chunk_data::width;
			auto tile = chunk->front[idx];

			glm::vec4 color{};
			float level = 0;
			float dropoff_amp = 1.f;

			// TODO: have world tile registry
			if (tile == tile_id::torch) {
				color = glm::vec4{1.f, .64f, 0.f, 1.f};
				level = 1.f;
			} else if (tile == tile_id::lava) {
				color = glm::vec4{1.f, .2f, 0.f, 1.f};
				level = 0.75f;
			} else if (tile == tile_id::air && pos.y < 1) {
				color = glm::vec4{1.f, 1.f, 1.f, 1.f};
				level = local_time();
				dropoff_amp = 1.5f;
			} else {
				continue;
			}

			if (is_not_surrounded(chunk, {x, y}, tile)) {
				propagation_queue.push_back({{x, y}, tile,
						{x, y}, color, level,
						dropoff_amp});
			} else {
				add_light_at({x, y}, color, level);
			}

			auto visited_idx = (chunk_data::width + x)
				+ (chunk_data::height + y) * chunk_data::width * 3;
			visited[visited_idx].set(idx);
		}
	}

	while (propagation_queue.size()) {
		auto item = propagation_queue.front();
		propagation_queue.pop_front();

		auto off = tile_to_neigh_offset(item.pos);
		auto tile_pos = tile_at(item.pos);
		auto chunk = get(pos + off);
		if (!chunk)
			continue;

		auto tile = chunk->front[tile_pos.x + tile_pos.y * chunk_data::width];

		// Don't propagate light from air blocks onto other air blocks
		if (item.pos != item.origin && item.origin_tile == tile_id::air
				&& tile == item.origin_tile)
			continue;

		float dropoff = 0.12f;
		if (tile == tile_id::air && (pos + off).y >= 1)
			dropoff = 0.05f;

		add_light_at(item.pos, item.color, item.level);
		item.level -= dropoff * item.dropoff_amp;

		if (item.level < epsilon)
			continue;

		constexpr glm::ivec2 dirs[] = {
			{1, 0}, {-1, 0},
			{0, 1}, {0, -1}
		};

		for (auto dir : dirs) {
			auto pos = item.pos + dir;
			auto origin_idx = item.origin.x + item.origin.y * chunk_data::width;
			auto item_idx = (chunk_data::width + pos.x)
				+ (chunk_data::height + pos.y) * chunk_data::width * 3;

			if (!visited[item_idx].test(origin_idx)) {
				propagation_queue.push_back({item.origin,
						item.origin_tile, pos, item.color,
						item.level, item.dropoff_amp});
				visited[item_idx].set(origin_idx);
			}
		}
	}

	chunk->light_affected_set = affected_set;
	return affected_set | old_aff_set;
}

} // namespace world
