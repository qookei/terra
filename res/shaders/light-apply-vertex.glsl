#version 330 core

layout(location = 0) in vec2 vert_pos;
layout(location = 1) in vec3 tex_pos;
layout(location = 2) in vec4 color;

out vec2 frag_uv;

void main() {
	gl_Position = vec4(vert_pos, 0.0, 1.0);
	frag_uv = tex_pos.xy;
}
