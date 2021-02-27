#include <render/chunk_view.hpp>
#include <world/client_tile_registry.hpp>

namespace render {

void chunk_view::generate_tiles_mesh() {
	gl::buffer_mapping<vertex, gl::vertex_buffer> mapping{tile_mesh_.vbo()};
	auto vtx = mapping.get();

	used_vertices_ = 0;

	auto emit_tile = [this](vertex *&vtx, int x, int y, world::tile_id id, bool back) {
		auto &info = client::world::tile_registry::get().get(id);

		constexpr float w = 16, h = 16;
		float tx = x * w, ty = y * h;

		constexpr glm::vec4 back_dim = {0.7f, 0.7f, 0.7f, 1.f};
		constexpr glm::vec4 no_dim = {1.f, 1.f, 1.f, 1.f};
		auto bg_rgba = info.bg_color * (back ? back_dim : no_dim);
		auto fg_rgba = info.fg_color * (back ? back_dim : no_dim);
		int bi = info.bg_idx - 1, fi = info.fg_idx - 1;

		if (info.bg_idx) {
			*vtx++ = vertex{{tx, ty}, {0, 0, bi}, bg_rgba};
			*vtx++ = vertex{{tx + w, ty + h}, {1, 1, bi}, bg_rgba};
			*vtx++ = vertex{{tx, ty + h}, {0, 1, bi}, bg_rgba};

			*vtx++ = vertex{{tx, ty}, {0, 0, bi}, bg_rgba};
			*vtx++ = vertex{{tx + w, ty}, {1, 0, bi}, bg_rgba};
			*vtx++ = vertex{{tx + w, ty + h}, {1, 1, bi}, bg_rgba};
			used_vertices_ += 6;
		}

		if (info.fg_idx) {
			*vtx++ = vertex{{tx, ty}, {0, 0, fi}, fg_rgba};
			*vtx++ = vertex{{tx + w, ty + h}, {1, 1, fi}, fg_rgba};
			*vtx++ = vertex{{tx, ty + h}, {0, 1, fi}, fg_rgba};

			*vtx++ = vertex{{tx, ty}, {0, 0, fi}, fg_rgba};
			*vtx++ = vertex{{tx + w, ty}, {1, 0, fi}, fg_rgba};
			*vtx++ = vertex{{tx + w, ty + h}, {1, 1, fi}, fg_rgba};
			used_vertices_ += 6;
		}
	};

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (auto i = data_->back[x + y * width]; i != world::tile_id::air)
				emit_tile(vtx, x, y, i, true);

			if (auto i = data_->front[x + y * width]; i != world::tile_id::air)
				emit_tile(vtx, x, y, i, false);
		}
	}
}

void chunk_view::generate_light_mesh() {
	gl::buffer_mapping<vertex, gl::vertex_buffer> mapping{light_mesh_.vbo()};
	auto vtx = mapping.get();

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			constexpr auto w = 16, h = 16;
			auto tx = x * w + w / 2, ty = y * h + h / 2;

			glm::vec4 rgba{};
			for (int i = 0; i < 9; i++)
				rgba += data_->light_at(i, x, y).color;

			*vtx++ = vertex{{tx, ty}, {}, rgba};
		}
	}
}


} // namespace render
