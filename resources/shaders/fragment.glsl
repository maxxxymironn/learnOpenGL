#version 330 core

out vec4 color;

uniform vec3 lightColor;
uniform vec3 cubeColor;

void main() {
    vec3 ambientColor = vec3(0.1f, 0.1f, 0.1f);
    color = vec4(lightColor * cubeColor, 1.f);
}