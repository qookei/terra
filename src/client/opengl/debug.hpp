#pragma once

#include <epoxy/gl.h>
#include <iostream>

namespace gl {

inline void debug_callback(GLenum source, GLenum type, GLuint, 
		GLenum severity, GLsizei length, 
		const char *message, const void *) {

	const char *severity_str = "";
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH: severity_str = "high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM: severity_str = "medium"; break;
		case GL_DEBUG_SEVERITY_LOW: severity_str = "low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: severity_str = "notif"; break;
	}

	const char *source_str = "?";
	switch (source) {
		case GL_DEBUG_SOURCE_API: source_str = "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: source_str = "window system"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: source_str = "shader compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: source_str = "third party"; break;
		case GL_DEBUG_SOURCE_APPLICATION: source_str = "app"; break;
		case GL_DEBUG_SOURCE_OTHER: source_str = "other"; break;
	}

	const char *type_str = "?";
	switch (type) {
		case GL_DEBUG_TYPE_ERROR: type_str = "error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_str = "deprecated behavior"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: type_str = "undefined behavior"; break;
		case GL_DEBUG_TYPE_PORTABILITY: type_str = "portability"; break;
		case GL_DEBUG_TYPE_MARKER: type_str = "marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP: type_str = "push group"; break;
		case GL_DEBUG_TYPE_POP_GROUP: type_str = "pop group"; break;
		case GL_DEBUG_TYPE_OTHER: type_str = "other"; break;
	}

	std::cerr << "GL debug: sev.: " << severity_str
		<< ", source: " << source_str
		<< ", type: " << type_str
		<< ", message: \""
		<< std::string_view{message, static_cast<size_t>(length)}
		<< "\"" << std::endl;
}

void init_debug() {
	glEnable(GL_DEBUG_OUTPUT);
#ifndef NDEBUG
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // disable if in release
#endif
	glDebugMessageCallback(debug_callback, nullptr);
	glDebugMessageControl(GL_DEBUG_SOURCE_SHADER_COMPILER, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
}

} // namespace gl
