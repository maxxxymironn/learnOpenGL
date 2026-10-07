#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 v_color;

out vec3 f_color;

void main() {
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    f_color = v_color;
}
