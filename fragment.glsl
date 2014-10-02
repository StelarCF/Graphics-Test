#version 430 core

in vec4 vs_color;
out vec4 color;

in vec2 t;

uniform sampler2D s;

void main(void) {
	vec2 pos;
	pos = gl_FragCoord.xy * 1.3;
	pos.y = 1484 - pos.y;
	color = (vs_color * (texelFetch(s, ivec2(pos), 0) * (cos(t.x / 5) + 1) / 2) + (texelFetch(s, ivec2(vec2(vs_color.x * 2227, vs_color.y * 1484))/*ivec2(pos)*/, 0)) * (cos(t.x / 5 + 3.1415926) + 1) / 2) / 2;//vs_color;
}