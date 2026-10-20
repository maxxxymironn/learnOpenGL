#version 330 core

struct Material {
    // vec3 ambient;   // commented because often diffuse = ambient // object self color
    sampler2D diffuse;   // object self color
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;   // ligth self color
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 normalDir;
in vec3 vertexPos;
in vec2 texCoords;

out vec4 color;

uniform vec3 viewPos;
uniform Light light;
uniform Material material;

void main() {
    vec3 normal = normalize(normalDir);
    vec3 distance = light.position - vertexPos;

    // ambient
    vec3 ambient = vec3(texture(material.diffuse, texCoords)) * light.ambient;

    // diffuse
    vec3 lightDir = normalize(distance);
    float rawDiffuseCoeff = dot(normal, lightDir);
    float diffuseCoeff = max(rawDiffuseCoeff, 0.f);
    vec3 diffuse = vec3(texture(material.diffuse, texCoords)) * light.diffuse * diffuseCoeff;

    // specular
    vec3 reflectedLightDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(viewPos - vertexPos);
    
    float specularityCoeff = max(dot(reflectedLightDir, viewDir), 0.f);
    vec3 specularTex = vec3(texture(material.specular, texCoords));
    vec3 specular = specularTex * light.specular * pow(specularityCoeff, material.shininess);
    specular *= step(0.f, rawDiffuseCoeff);

    // emission
    float emissionOutsideBorder = 1.f - step(0.001f, dot(specularTex, specularTex));
    vec3 emission = vec3(texture(material.emission, texCoords));

    float dl = length(distance);
    float attenuation = 1.f / (light.constant + light.linear * dl + light.quadratic * dl * dl);
    
    vec3 finalColor = ambient + (diffuse + specular) * attenuation;
    color = vec4(finalColor, 1.f);
}
