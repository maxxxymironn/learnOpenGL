#version 330 core

in vec3 f_color;

out vec4 color;

uniform float colorCoeff;

void main() {
    color = vec4(f_color * colorCoeff, 1.0);
}