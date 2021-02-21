#pragma once

#include <epoxy/gl.h>

namespace gl {

struct framebuffer {
	friend void swap(framebuffer &a, framebuffer &b) {
		using std::swap;
		swap(a.fb_id_, b.fb_id_);
		swap(a.ca_id_, b.ca_id_);
	}

	framebuffer()
	: fb_id_{}, ca_id_{}, width_{}, height_{} { }

	~framebuffer() {
		glDeleteTextures(1, &ca_id_);
		glDeleteFramebuffers(1, &fb_id_);
	}

	framebuffer(const framebuffer &) = delete;
	framebuffer &operator=(const framebuffer &) = delete;

	framebuffer(framebuffer &&other)
	: framebuffer() {
		swap(*this, other);
	}

	framebuffer &operator=(framebuffer &&other) {
		swap(*this, other);
		return *this;
	}

	void generate(int width, int height) {
		glCreateFramebuffers(1, &fb_id_);
		glCreateTextures(GL_TEXTURE_2D, 1, &ca_id_);

		glTextureParameteri(ca_id_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(ca_id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(ca_id_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(ca_id_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTextureStorage2D(ca_id_, 1, GL_RGBA8, width, height);

		glNamedFramebufferTexture(fb_id_, GL_COLOR_ATTACHMENT0, ca_id_, 0);
	}

	void bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, fb_id_);
	}

	void unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void bind_ca_texture() {
		glBindTexture(GL_TEXTURE_2D, ca_id_);
	}

private:
	GLuint fb_id_;
	GLuint ca_id_;

	int width_;
	int height_;
};

} // namespace gl
