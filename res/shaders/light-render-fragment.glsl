#version 330 core

in vec4 frag_color;
in vec3 frag_uv;

void main() {
	gl_FragColor = frag_color;
}
