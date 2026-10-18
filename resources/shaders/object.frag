#version 330 core

struct Material {
    vec3 ambient;   // object self color
    vec3 diffuse;   // object self color
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;   // ligth self color
    vec3 specular;
};

in vec3 normalDir;
in vec3 vertexPos;

out vec4 color;

uniform vec3 viewPos;
uniform Light light;
uniform Material material;

void main() {
    vec3 normal = normalize(normalDir);

    // ambient
    vec3 ambient = material.ambient * light.ambient;

    // diffuse
    vec3 lightDir = normalize(light.position - vertexPos);
    float rawDiffuseCoeff = dot(normal, lightDir);
    float diffuseCoeff = max(rawDiffuseCoeff, 0.f);
    vec3 diffuse = material.diffuse * light.diffuse * diffuseCoeff;

    // specular
    vec3 reflectedLightDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(viewPos - vertexPos);
    float specularityCoeff = max(dot(reflectedLightDir, viewDir), 0.f);
    vec3 specular = material.specular * light.specular * pow(specularityCoeff, material.shininess);
    specular *= step(0.f, rawDiffuseCoeff);
    
    color = vec4(ambient + diffuse + specular, 1.f);
}
