#pragma once

#include <opengl/mesh.hpp>
#include <opengl/shader.hpp>

#include <world/chunk.hpp>

namespace render {

struct chunk_view {
	static constexpr int width = world::chunk_data::width;
	static constexpr int height = world::chunk_data::height;
	static constexpr int layers = 2;
	static constexpr int tile_vertices = width * height * layers * 6 * 2;
	static constexpr int light_vertices = width * height * 6;

	chunk_view(world::chunk_data *data)
	: tile_mesh_{}, light_mesh_{}, data_{data}, used_vertices_{} {
		tile_mesh_.vbo().store_regenerate(nullptr, sizeof(vertex) * tile_vertices, GL_DYNAMIC_DRAW);
		light_mesh_.vbo().store_regenerate(nullptr, sizeof(vertex) * light_vertices, GL_DYNAMIC_DRAW);
	}

	void generate_light_mesh();
	void generate_tiles_mesh();

	void render_tiles() const {
		tile_mesh_.render(used_vertices_);
	}

	void render_light() const {
		light_mesh_.render(light_vertices);
	}

private:
	gl::mesh tile_mesh_;
	gl::mesh light_mesh_;

	world::chunk_data *data_;

	size_t used_vertices_;
};

} // namespace render
