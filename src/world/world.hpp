#pragma once

#include <world/chunk.hpp>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <glm/gtx/hash.hpp>
#include <bitset>

namespace world {

struct world_data {
	world_data() = default;

	bool is_loaded(glm::ivec2 pos) const {
		return chunks.contains(pos);
	}

	void remove(glm::ivec2 pos) {
		chunks.erase(pos);
	}

	chunk_data *get(glm::ivec2 pos) {
		if (is_loaded(pos))
			return &chunks[pos];

		return nullptr;
	}

	void add(glm::ivec2 pos, chunk_data &&data) {
		chunks.emplace(pos, std::move(data));
	}

	glm::ivec2 chunk_at(glm::ivec2 world_pos) const {
		return {static_cast<unsigned int>(world_pos.x) % chunk_data::width,
			static_cast<unsigned int>(world_pos.y) % chunk_data::height};
	}

	glm::ivec2 tile_at(glm::ivec2 world_pos) const {
		return {static_cast<unsigned int>(world_pos.x) % chunk_data::width,
			static_cast<unsigned int>(world_pos.y) % chunk_data::height};
	}

	// Returns bit set of affected chunks
	std::bitset<9> calculate_lighting_for(glm::ivec2 pos);

	auto begin() const {
		return chunks.begin();
	}

	auto begin() {
		return chunks.begin();
	}

	auto end() const {
		return chunks.end();
	}

	auto end() {
		return chunks.end();
	}

private:
	std::unordered_map<glm::ivec2, chunk_data> chunks;
};

} // namespace world
