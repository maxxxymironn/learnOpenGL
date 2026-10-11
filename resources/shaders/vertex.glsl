#version 330 core

layout (location = 0) in vec3 position;
// layout (location = 1) in vec3 v_color;
layout (location = 1) in vec2 v_texCoord;

// out vec3 f_color;
out vec2 f_texCoord;

uniform mat4 transformMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main() {
    gl_Position = projectionMatrix * viewMatrix * transformMatrix * vec4(position, 1.f);
    // f_color = v_color;
    f_texCoord = v_texCoord;
}
