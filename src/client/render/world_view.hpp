#pragma once

#include <render/chunk_view.hpp>
#include <opengl/shader.hpp>
#include <glm/vec2.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>
#include <unordered_set>
#include <deque>

namespace render {

struct world_view {
	world_view(world::world_data *data)
	: data_{data}, views_{} { }

	world::world_data *world() {
		return data_;
	}

	void render_tiles(gl::program &prog) {
		for (auto &[pos, chunk] : *data_) {
			const auto &view = views_.at(&chunk);
			prog.set_uniform("chunk_pos", pos);
			view.render_tiles();
		}
	}

	void render_light(gl::program &prog) {
		for (auto &[pos, chunk] : *data_) {
			const auto &view = views_.at(&chunk);
			prog.set_uniform("chunk_pos", pos);
			view.render_light();
		}
	}

	void add_view_for(world::chunk_data *ptr) {
		views_.emplace(ptr, chunk_view{ptr});
	}

	void remove_view_for(world::chunk_data *ptr) {
		views_.erase(ptr);
	}

	void generate_tiles_mesh_for(glm::ivec2 pos) {
		auto chunk = data_->get(pos);
		views_.at(chunk).generate_tiles_mesh();
	}

	void generate_light_mesh_for(glm::ivec2 pos, std::bitset<9> affected_set) {
		for (int i = 0; i < 9; i++) {
			if (!affected_set.test(i))
				continue;

			int tx = (i % 3) - 1;
			int ty = (i / 3) - 1;
			auto chunk = data_->get(pos + glm::ivec2{tx, ty});
			if (chunk)
				views_.at(chunk).generate_light_mesh();
		}
	}

	void queue_light_update(glm::ivec2 pos) {
		queued_light_updates_.push_back(pos);
	}

	void do_queued_light_updates() {
		while (queued_light_updates_.size()) {
			auto pos = queued_light_updates_.front();
			queued_light_updates_.pop_front();

			for (int dy = -1; dy < 2; dy++) {
				for (int dx = -1; dx < 2; dx++) {
					auto target = pos + glm::ivec2{dx, dy};
					if (visited_light_updates_.contains(target))
						continue;

					auto aff_set = data_->calculate_lighting_for(target);
					generate_light_mesh_for(target, aff_set);
					visited_light_updates_.insert(target);
				}
			}
		}

		visited_light_updates_.clear();
	}

private:
	world::world_data *data_;
	std::unordered_map<world::chunk_data *, chunk_view> views_;
	std::deque<glm::ivec2> queued_light_updates_;
	std::unordered_set<glm::ivec2> visited_light_updates_;
};

} // namespace render
