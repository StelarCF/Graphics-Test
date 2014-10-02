#version 430 core

layout (location = 0) in vec4 position;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out vec4 vs_color;

layout (location = 1) in vec2 vec2_t;

out vec2 t;

void main(void) {
	gl_Position = proj_matrix * mv_matrix * position;
	vs_color = (position * 2.0 + vec4(0.5, 0.5, 0.5, 0.0));
	t = vec2_t;
}
