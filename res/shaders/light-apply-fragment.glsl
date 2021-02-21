#version 330 core

in vec4 frag_color;
in vec3 frag_uv;

uniform sampler2D tex;

void main() {
	gl_FragColor = texture(tex, frag_uv.xy) * frag_color;
}
