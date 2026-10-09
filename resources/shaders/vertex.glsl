#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 v_color;
layout (location = 2) in vec2 v_texCoord;

out vec3 f_color;
out vec2 f_texCoord;

uniform mat4 transformMatrix;

void main() {
    gl_Position = transformMatrix * vec4(position.x, position.y, 0.f, 1.f);
    f_color = v_color;
    f_texCoord = v_texCoord;
}
