#include <iostream>
#include <render/window.hpp>

#include <world/tile_ids.hpp>
#include <world/client_tile_registry.hpp>
#include <world/world.hpp>

#include <render/chunk_view.hpp>
#include <render/world_view.hpp>
#include <opengl/shader.hpp>
#include <opengl/texture.hpp>
#include <opengl/framebuffer.hpp>
#include <opengl/init.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/transform.hpp>

#include <random>

void add_random_chunk(glm::ivec2 pos, render::world_view &view, auto &mt) {
	std::uniform_int_distribution<int> dist{0, 15};
	std::uniform_int_distribution<int> dist3{0, 11};
	std::uniform_int_distribution<int> dist2{0, 99};
	world::chunk_data cd;

	for (size_t i = 0; i < world::chunk_data::width * world::chunk_data::height; i++) {
		cd.front[i] = static_cast<world::tile_id>(dist(mt));

		if ((cd.front[i] == world::tile_id::torch || cd.front[i] == world::tile_id::lava) && dist2(mt) > 2)
			cd.front[i] = static_cast<world::tile_id>(dist3(mt));

		cd.back[i] = static_cast<world::tile_id>(dist(mt));
	}

	view.world()->add(pos, std::move(cd));
	view.add_view_for(view.world()->get(pos));
	view.generate_tiles_mesh_for(pos);
}

int main() {
	window wnd_;

	gl::init();

	auto chunk_prog = gl::program(
			gl::shader(GL_VERTEX_SHADER, "res/shaders/generic-vertex.glsl"),
			gl::shader(GL_FRAGMENT_SHADER, "res/shaders/chunk-fragment.glsl")
		);

	auto light_render_prog = gl::program(
			gl::shader(GL_VERTEX_SHADER, "res/shaders/light-render-vertex.glsl"),
			gl::shader(GL_FRAGMENT_SHADER, "res/shaders/light-render-fragment.glsl")
		);

	auto light_apply_prog = gl::program(
			gl::shader(GL_VERTEX_SHADER, "res/shaders/light-apply-vertex.glsl"),
			gl::shader(GL_FRAGMENT_SHADER, "res/shaders/light-apply-fragment.glsl")
		);

	world::world_data world;
	render::world_view world_view{&world};

	std::random_device rd;
	std::mt19937 mt{rd()};

	client::world::load_default_tiles();

	add_random_chunk({0, 0}, world_view, mt);
	add_random_chunk({1, 0}, world_view, mt);
	add_random_chunk({2, 0}, world_view, mt);

	add_random_chunk({0, 1}, world_view, mt);
	add_random_chunk({1, 1}, world_view, mt);
	add_random_chunk({2, 1}, world_view, mt);

	for (auto &[pos, _] : world) {
		auto aff_set = world.calculate_lighting_for(pos);
		world_view.generate_light_mesh_for(pos, aff_set);
	}

	glm::mat4 ortho = glm::ortho(0.f, 1280.f, 720.f, 0.f);

	gl::framebuffer light_fb;
	light_fb.generate(1280, 720);

	gl::mesh light_mesh;
	render::vertex vertices[] = {
		{{-1, -1}, {0, 0, 0}, {}},
		{{ 1,  1}, {1, 1, 0}, {}},
		{{-1,  1}, {0, 1, 0}, {}},
		{{-1, -1}, {0, 0, 0}, {}},
		{{ 1, -1}, {1, 0, 0}, {}},
		{{ 1,  1}, {1, 1, 0}, {}},
	};
	light_mesh.vbo().store_regenerate(vertices, sizeof(render::vertex) * 6, GL_STATIC_DRAW);

	auto &tr = client::world::tile_registry::get();

	bool loop = true;
	while(loop) {
		SDL_Event ev;
		while(SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT)
				loop = false;
		}

		glBlendFunc(GL_ONE, GL_ONE);

		light_fb.bind();
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		light_render_prog.use();
		light_render_prog.set_uniform("ortho", ortho);
		world_view.render_light(light_render_prog);
		light_fb.unbind();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(0.364f, 0.737f, 0.823f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		chunk_prog.use();
		chunk_prog.set_uniform("ortho", ortho);
		tr.associated_texture().bind();
		world_view.render_tiles(chunk_prog);

		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		light_apply_prog.use();
		light_fb.bind_ca_texture();
		light_mesh.render(6);

		wnd_.swap();
	}

	return 0;
}
