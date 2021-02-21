#pragma once

#include <render/chunk_view.hpp>
#include <opengl/shader.hpp>

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

	void generate_light_mesh_for(glm::ivec2 pos, uint32_t affected_set = 0b000010000) {
		for (int i = 0; i < 9; i++) {
			if (!(affected_set & (1 << i)))
				continue;

			int tx = (i % 3) - 1;
			int ty = (i / 3) - 1;
			auto chunk = data_->get(pos + glm::ivec2{tx, ty});
			views_.at(chunk).generate_light_mesh();
		}
	}

private:
	world::world_data *data_;
	std::unordered_map<world::chunk_data *, chunk_view> views_;
};

} // namespace render
