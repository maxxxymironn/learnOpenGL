#version 330 core

out vec4 color;

in vec3 normalVec;
in vec3 fragmentPos;

uniform vec3 lightColor;
uniform vec3 cubeColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    vec3 ambient = vec3(0.05f, 0.05f, 0.05f) * lightColor;
    
    vec3 norm = normalize(normalVec);
    vec3 lightDir = normalize(lightPos - fragmentPos);
    vec3 viewDir = normalize(viewPos - fragmentPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float specularStrength = 1.f;
    float spec = pow(max(dot(viewDir, reflectDir), 0.f), 32);
    vec3 specular = specularStrength * spec * lightColor;

    float diff = max(dot(norm, lightDir), 0.f);
    vec3 diffuse = diff * lightColor;

    vec3 resultColor = (diffuse + ambient + specular) * cubeColor;
    color = vec4(resultColor, 1.f);
}