#pragma once

#include <map>
#include <glm/vec4.hpp>
#include <world/tile_ids.hpp>
#include <opengl/texture.hpp>

namespace client::world {

using tile_id = ::world::tile_id;

struct client_tile_info {
	std::string bg_path = "";
	std::string fg_path = "";

	glm::vec4 fg_color = {1, 1, 1, 1};
	glm::vec4 bg_color = {1, 1, 1, 1};

	int bg_idx = 0;
	int fg_idx = 0;

	bool emits_light = false;
	glm::vec4 light_color = {1, 1, 1, 1};
};

struct tile_registry {
	static tile_registry &get() {
		static tile_registry inst;
		return inst;
	}

	const client_tile_info &get(tile_id id) const {
		return tiles_.at(id);
	}

	void register_tile(tile_id id, client_tile_info &&ti) {
		if (ti.bg_path.size())
			n_textures_++;
		if (ti.fg_path.size())
			n_textures_++;

		tiles_.emplace(id, std::move(ti));
	}

	void finalize() {
		tex_array_.generate(16, 16, n_textures_);

		for (auto &[_, ti] : tiles_) {
			if (ti.bg_path.size())
				ti.bg_idx = tex_array_.add_texture(ti.bg_path) + 1;
			if (ti.fg_path.size())
				ti.fg_idx = tex_array_.add_texture(ti.fg_path) + 1;
		}
	}

	gl::texture_array &associated_texture() {
		return tex_array_;
	}

private:
	tile_registry()
	: tiles_{}, tex_array_{}, n_textures_{} { }

	std::map<tile_id, client_tile_info> tiles_;
	gl::texture_array tex_array_;

	int n_textures_;
};

inline void load_default_tiles() {
	auto &tr = tile_registry::get();

	tr.register_tile(world::tile_id::air, {});
	tr.register_tile(world::tile_id::grass,
			{.bg_path = "res/tiles/dirt.png", .fg_path = "res/tiles/grass_top.png", .fg_color = {0.063f, 0.645f, 0.011f, 1}});
	tr.register_tile(world::tile_id::dirt,
			{.bg_path = "res/tiles/dirt.png"});
	tr.register_tile(world::tile_id::stone,
			{.bg_path = "res/tiles/stone.png"});
	tr.register_tile(world::tile_id::cobble,
			{.bg_path = "res/tiles/cobble.png"});
	tr.register_tile(world::tile_id::iron_ore,
			{.bg_path = "res/tiles/stone.png", .fg_path = "res/tiles/iron_ore.png"});
	tr.register_tile(world::tile_id::coal_ore,
			{.bg_path = "res/tiles/stone.png", .fg_path = "res/tiles/coal_ore.png"});
	tr.register_tile(world::tile_id::log,
			{.bg_path = "res/tiles/log.png"});
	tr.register_tile(world::tile_id::leaves,
			{.bg_path = "res/tiles/leaves.png", .fg_color = {0.063f, 0.645f, 0.011f, 1}});
	tr.register_tile(world::tile_id::planks, {.bg_path = "res/tiles/planks.png"});
	tr.register_tile(world::tile_id::water, {.bg_path = "res/tiles/water.png"});
	tr.register_tile(world::tile_id::ice, {.bg_path = "res/tiles/ice.png"});
	tr.register_tile(world::tile_id::torch, {.bg_path = "res/tiles/torch.png", .emits_light = true, .light_color = {1.f, 0.5f, 0.f, 1.f}});
	tr.register_tile(world::tile_id::lava, {.bg_path = "res/tiles/lava.png", .emits_light = true, .light_color = {1.f, 0.2f, 0.f, 1.f}});
	tr.register_tile(world::tile_id::sand, {.bg_path = "res/tiles/sand.png"});
	tr.register_tile(world::tile_id::cactus, {.bg_path = "res/tiles/cactus.png"});

	tr.finalize();
}

} // namespace client::world
