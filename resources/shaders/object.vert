#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 vertexPos;
out vec3 normalDir;

uniform mat4 transformMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main() {
    vec4 model = transformMatrix * vec4(position, 1.f);
    vertexPos = model.xyz;
    normalDir = normalize(mat3(transpose(inverse(transformMatrix))) * normal);
    
    gl_Position = projectionMatrix * viewMatrix * model;
}
