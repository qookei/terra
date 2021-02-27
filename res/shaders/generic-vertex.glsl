#version 330 core

layout(location = 0) in vec2 vert_pos;
layout(location = 1) in vec3 tex_pos;
layout(location = 2) in vec4 color;

out vec4 frag_color;
out vec3 frag_uv;

uniform mat4 ortho;
uniform ivec2 chunk_pos;

void main() {
	gl_Position = ortho * vec4(vert_pos + chunk_pos * 512.0, 0.0, 1.0);
	frag_color = color;
	frag_uv = tex_pos;
}
