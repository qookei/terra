#pragma once

#include <opengl/debug.hpp>
#include <epoxy/gl.h>

namespace gl {

void init() {
	gl::init_debug();
	glEnable(GL_BLEND);
}

} // namespace gl
