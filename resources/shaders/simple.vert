#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 texCoords;

uniform mat4 uTransformMat;
uniform mat4 uViewMat;
uniform mat4 uProjectionMat;

void main() {
    texCoords = aTexCoords;
    gl_Position = uProjectionMat * uViewMat * uTransformMat * vec4(aPos, 1.0);
}