#pragma once

#include <stb_image.h>
#include <epoxy/gl.h>
#include <cassert>
#include <iostream>

namespace gl {

struct texture_array {
	friend void swap(texture_array &a, texture_array &b) {
		using std::swap;
		swap(a.id_, b.id_);
	}

	texture_array()
	: id_{}, width_{}, height_{}, z_{}, max_textures_{} { }

	~texture_array() {
		glDeleteTextures(1, &id_);
	}

	texture_array(const texture_array &) = delete;
	texture_array &operator=(const texture_array &) = delete;

	texture_array(texture_array &&other)
	: texture_array() {
		swap(*this, other);
	}

	texture_array &operator=(texture_array &&other) {
		swap(*this, other);
		return *this;
	}

	void generate(int width, int height, int max_textures) {
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &id_);

		glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(id_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(id_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTextureStorage3D(id_, 1, GL_RGBA8, width, height, max_textures);
		width_ = width;
		height_ = height;
		max_textures_ = max_textures;
	}

	void bind() {
		glBindTexture(GL_TEXTURE_2D_ARRAY, id_);
	}

	int add_texture(const std::string &path) {
		int w, h, n;

		if (auto d = stbi_load(path.data(), &w, &h, &n, 4); d) {
			assert(w == width_);
			assert(h == height_);
			assert(max_textures_ > z_);
			glTextureSubImage3D(id_, 0, 0, 0, z_, w, h, 1,
					GL_RGBA, GL_UNSIGNED_BYTE, d);
			stbi_image_free(d);
		} else {
			std::cerr << __func__ << ": failed to load \"" << path << "\"" << std::endl;
			assert(!"failed to load texture");
		}

		return z_++;
	}

private:
	GLuint id_;

	int width_;
	int height_;
	int z_;
	int max_textures_;
};

struct texture2d {
	friend void swap(texture2d &a, texture2d &b) {
		using std::swap;
		swap(a.id_, b.id_);
	}

	texture2d()
	: id_{}, width_{}, height_{} { }

	~texture2d() {
		glDeleteTextures(1, &id_);
	}

	texture2d(const texture_array &) = delete;
	texture2d &operator=(const texture_array &) = delete;

	texture2d(texture2d &&other)
	: texture2d() {
		swap(*this, other);
	}

	texture2d &operator=(texture2d &&other) {
		swap(*this, other);
		return *this;
	}

	void generate(int width, int height) {
		glCreateTextures(GL_TEXTURE_2D, 1, &id_);

		glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(id_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(id_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTextureStorage2D(id_, 1, GL_RGBA8, width, height);
	}

	void bind() {
		glBindTexture(GL_TEXTURE_2D, id_);
	}

	void load(const std::string &path) {
		int w, h, n;

		if (auto d = stbi_load(path.data(), &w, &h, &n, 4); d) {
			if (!id_)
				generate(w, h);

			glTextureSubImage2D(id_, 0, 0, 0, w, h,
					GL_RGBA, GL_UNSIGNED_BYTE, d);
			stbi_image_free(d);
		} else {
			std::cerr << __func__ << ": failed to load \"" << path << "\"" << std::endl;
			assert(!"failed to load texture");
		}
	}

	int width() const {
		return width_;
	}

	int height() const {
		return height_;
	}

private:
	GLuint id_;

	int width_;
	int height_;
};


} // namespace gl
