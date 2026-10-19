#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec3 vertexPos;
out vec3 normalDir;
out vec2 texCoords;

uniform mat4 transformMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main() {
    vec4 model = transformMatrix * vec4(aPosition, 1.f);
    vertexPos = model.xyz;
    normalDir = normalize(mat3(transpose(inverse(transformMatrix))) * aNormal);
    texCoords = aTexCoords;
    
    gl_Position = projectionMatrix * viewMatrix * model;
}
