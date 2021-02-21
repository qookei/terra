#pragma once

#include <epoxy/gl.h>
#include <cassert>
#include <utility>
#include <stddef.h>

namespace gl {

template <GLenum Type>
struct buffer {
	friend void swap(buffer &a, buffer &b) {
		using std::swap;

		swap(a.id_, b.id_);
		swap(a.size_, b.size_);
		swap(a.buffer_ptr_, b.buffer_ptr_);
		swap(a.usage_, b.usage_);
	}

	buffer()
	:id_{0}, size_{0}, buffer_ptr_{nullptr}, usage_{0} {}

	~buffer() {
		if (buffer_ptr_)
			unmap();

		glDeleteBuffers(1, &id_);
	}

	buffer(const buffer &other) = delete;
	buffer(buffer &&other) {
		swap(*this, other);
	}

	buffer &operator=(const buffer &other) = delete;
	buffer &operator=(buffer &&other) {
		swap(*this, other);
		return *this;
	}

	void bind() {
		assert(id_);
		glBindBuffer(Type, id_);
	}

	void unbind() {
		glBindBuffer(Type, 0);
	}

	void generate() {
		glGenBuffers(1, &id_);
	}

	void *map() {
		buffer_ptr_ = glMapNamedBuffer(id_, GL_READ_WRITE);

		assert(buffer_ptr_);
		return buffer_ptr_;
	}

	void store(const void *data, size_t offset, size_t size) {
		if (!data)
			return;

		assert(size_ >= (size + offset) && "buffer overflow");

		glNamedBufferSubData(id_, offset, size, data);
	}

	void unmap() {
		glUnmapNamedBuffer(id_);

		buffer_ptr_ = nullptr;
	}

	void store_regenerate(const void *data, size_t size, GLenum usage) {
		if (size_ == size && usage_ == usage) {
			store(data, 0, size);
			return;
		}

		size_ = size;
		usage_ = usage;

		glNamedBufferData(id_, size_, data, usage);
	}

	size_t size() const {
		return size_;
	}

	void *data() const {
		return buffer_ptr_;
	}

	GLenum usage() const {
		return usage_;
	}

	GLuint id() const {
		return id_;
	}

private:
	GLuint id_;
	size_t size_;
	void *buffer_ptr_;
	GLenum usage_;
};

using vertex_buffer = buffer<GL_ARRAY_BUFFER>;

// RAII wrapper for mapping/unmapping buffers
template <typename T, typename Buffer>
struct buffer_mapping {
	friend void swap(buffer_mapping &a, buffer_mapping &b) {
		using std::swap;
		swap(a.ptr_, b.ptr_);
		swap(a.buffer_, b.buffer_);
	}

	buffer_mapping()
	: ptr_{}, buffer_{} { }

	buffer_mapping(Buffer &buffer)
	: ptr_{reinterpret_cast<T *>(buffer.map())}, buffer_{&buffer} { }

	~buffer_mapping() {
		if (buffer_) buffer_->unmap();
	}

	buffer_mapping(const buffer_mapping &) = delete;
	buffer_mapping &operator=(const buffer_mapping &) = delete;

	buffer_mapping(buffer_mapping &&other)
	: buffer_mapping() {
		swap(*this, other);
	}

	buffer_mapping &operator=(buffer_mapping &&other) {
		swap(*this, other);
		return *this;
	}

	T *get() const {
		return ptr_;
	}

	Buffer &associated_buffer() const {
		return *buffer_;
	}

	operator bool() const {
		return ptr_;
	}

private:
	T *ptr_;
	Buffer *buffer_;
};

} // namespace gl
