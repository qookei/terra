#pragma once

#include <vector>
#include <concepts>
#include <fstream>

#include <cassert>
#include <iostream>

#include <epoxy/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace gl {

struct shader {
	friend void swap(shader &a, shader &b) {
		using std::swap;
		swap(a.id_, b.id_);
		swap(a.type_, b.type_);
	}

	shader()
	: id_{}, type_{} { }

	shader(GLenum type, const std::string &path)
	: id_{glCreateShader(type)}, type_{type} {
		std::ifstream ifs{path};
		if (!ifs) {
			std::cerr << __func__ << ": failed to load \"" << path << "\"" << std::endl;
			assert(!"failed to load shader");
		}

		std::string content{std::istreambuf_iterator<char>{ifs},
					std::istreambuf_iterator<char>{}};

		const char *c_str = content.c_str();

		glShaderSource(id_, 1, &c_str, nullptr);
		glCompileShader(id_);
	}

	~shader() {
		glDeleteShader(id_);
	}

	shader(const shader &) = delete;
	shader &operator=(const shader &) = delete;

	shader(shader &&other)
	: shader() {
		swap(*this, other);
	}

	shader &operator=(shader &&other) {
		swap(*this, other);
		return *this;
	}

	GLuint id() const {
		return id_;
	}

	GLenum type() const {
		return type_;
	}

private:
	GLuint id_;
	GLenum type_;
};

struct program {
	friend void swap(program &a, program &b) {
		using std::swap;
		swap(a.id_, b.id_);
	}

	program()
	: id_{0} { }

	template <typename ...Ts> requires (std::same_as<Ts, shader> && ...)
	program(Ts ...shaders)
	: id_{glCreateProgram()} {
		(glAttachShader(id_, shaders.id()), ...);
		glLinkProgram(id_);
		glValidateProgram(id_);
		(glDetachShader(id_, shaders.id()), ...);
	}

	program(const program &) = delete;
	program &operator=(const program &) = delete;

	program(program &&other)
	: program() {
		swap(*this, other);
	}

	program &operator=(program &&other) {
		swap(*this, other);
		return *this;
	}

	~program() {
		glDeleteProgram(id_);
	}

	void use() {
		glUseProgram(id_);
	}

	GLuint id() const {
		return id_;
	}

	GLint attribute_location(const std::string &name) {
		return glGetAttribLocation(id_, name.c_str());
	}

	void set_uniform(const std::string &name, glm::mat4 val) {
		auto loc = glGetUniformLocation(id_, name.c_str());
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
	}

	void set_uniform(const std::string &name, glm::ivec2 val) {
		auto loc = glGetUniformLocation(id_, name.c_str());
		glUniform2iv(loc, 1, glm::value_ptr(val));
	}

	void set_uniform(const std::string &name, int val) {
		auto loc = glGetUniformLocation(id_, name.c_str());
		glUniform1i(loc, val);
	}

private:
	GLuint id_;
};


} // namespace gl
