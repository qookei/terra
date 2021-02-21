#pragma once

#include <glm/glm.hpp>

namespace render {

struct vertex {
	glm::vec2 pos;
	glm::vec3 tex;
	glm::vec4 color;
};

} // namespace render
