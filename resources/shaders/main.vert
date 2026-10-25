#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 vertexPos;
out vec3 normalDir;
out vec2 texCoords;

uniform mat4 uTransformMat;
uniform mat4 uViewMat;
uniform mat4 uProjectionMat;

void main() {
    vec4 model = uTransformMat * vec4(aPosition, 1.f);
    vertexPos = model.xyz;
    normalDir = normalize(mat3(transpose(inverse(uTransformMat))) * aNormal);
    texCoords = aTexCoords;
    
    gl_Position = uProjectionMat * uViewMat * model;
}
