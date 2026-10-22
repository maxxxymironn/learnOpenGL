#version 330 core

layout (location = 0) in vec3 v_position;
layout (location = 2) in vec2 v_texCoords;

out vec2 texCoords;

uniform mat4 transformMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main() {
    texCoords = v_texCoords;
    gl_Position = projectionMatrix * viewMatrix * transformMatrix * vec4(v_position, 1.f);
}