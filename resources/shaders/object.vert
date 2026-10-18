#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 vertColor;

uniform mat4 transformMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 cubeColor;

void main() {
    vec4 model = transformMatrix * vec4(position, 1.f);    
    gl_Position = projectionMatrix * viewMatrix * model;

    vec3 fragmentPos = model.xyz;
    vec3 normalDir = normalize(mat3(transpose(inverse(transformMatrix))) * normal);
    
    vec3 ambientColor = vec3(0.05f, 0.05f, 0.05f) * lightColor;
    
    vec3 lightDir = normalize(lightPos - fragmentPos);
    vec3 viewDir = normalize(viewPos - fragmentPos);
    vec3 reflectDir = reflect(-lightDir, normalDir);

    float diffuseCoeff = max(dot(lightDir, normalDir), 0.f);
    vec3 diffuseColor = lightColor * diffuseCoeff;
    
    float specularStrength = 1.f;
    float specularityCoeff = pow(max(dot(viewDir, reflectDir), 0.f), 32);
    vec3 specularColor = specularStrength * specularityCoeff * lightColor;

    vertColor = (ambientColor + diffuseColor + specularColor) * cubeColor;
}
