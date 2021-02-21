#pragma once

#include <opengl/vao.hpp>
#include <opengl/buffer.hpp>

namespace gl {

struct mesh {
	friend void swap(mesh &a, mesh &b) {
		using std::swap;
		swap(a.vbo_, b.vbo_);
		swap(a.vao_, b.vao_);
	}

	mesh()
	: vbo_{}, vao_{} {
		vbo_.generate();
		vao_.generate(vbo_);
	}

	mesh(const mesh &other) = delete;
	mesh(mesh &&other) {
		swap(*this, other);
	}

	mesh &operator=(const mesh &other) = delete;
	mesh &operator=(mesh &&other) {
		swap(*this, other);
		return *this;
	}

	void render() const {
		vao_.render(vbo_.size() / sizeof(render::vertex));
	}

	void render(size_t n_vertices) const {
		vao_.render(n_vertices);
	}

	vertex_buffer &vbo() {
		return vbo_;
	}

private:
	vertex_buffer vbo_;
	vao vao_;
};

} // namespace gl
