#pragma once

#include <epoxy/gl.h>
#include <opengl/buffer.hpp>
#include <render/vertex.hpp>

namespace gl {

struct vao {
	friend void swap(vao &a, vao &b) {
		using std::swap;
		swap(a.id_, b.id_);
	}

	vao()
	: id_{} { }

	~vao() {
		glDeleteVertexArrays(1, &id_);
	}

	vao(const vao &other) = delete;
	vao(vao &&other) {
		swap(*this, other);
	}

	vao &operator=(const vao &other) = delete;
	vao &operator=(vao &&other) {
		swap(*this, other);
		return *this;
	}

	void generate(vertex_buffer &vbo) {
		glCreateVertexArrays(1, &id_);

		using render::vertex;

		glVertexArrayVertexBuffer(id_, 0, vbo.id(), 0, sizeof(vertex));

		glEnableVertexArrayAttrib(id_, 0);
		glVertexArrayAttribFormat(id_, 0, 2, GL_FLOAT, GL_FALSE, offsetof(vertex, pos));
		glVertexArrayAttribBinding(id_, 0, 0);

		glEnableVertexArrayAttrib(id_, 1);
		glVertexArrayAttribFormat(id_, 1, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, tex));
		glVertexArrayAttribBinding(id_, 1, 0);

		glEnableVertexArrayAttrib(id_, 2);
		glVertexArrayAttribFormat(id_, 2, 4, GL_FLOAT, GL_FALSE, offsetof(vertex, color));
		glVertexArrayAttribBinding(id_, 2, 0);
	}

	void bind() const {
		glBindVertexArray(id_);
	}

	void unbind() const {
		glBindVertexArray(0);
	}

	template <GLenum Mode = GL_TRIANGLES>
	void render(size_t n_vertices) const {
		bind();

		glDrawArrays(Mode, 0, n_vertices);
	}

	GLuint id() const {
		return id_;
	}

private:
	GLuint id_;
};

} // namespace gl
