#version 330 core

struct Material {
    // vec3 ambient;   // commented because often diffuse = ambient // object self color
    sampler2D diffuse;   // object self color
    sampler2D specular;
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
in vec2 texCoords;

out vec4 color;

uniform vec3 viewPos;
uniform Light light;
uniform Material material;

void main() {
    vec3 normal = normalize(normalDir);

    // ambient
    vec3 ambient = vec3(texture(material.diffuse, texCoords)) * light.ambient;

    // diffuse
    vec3 lightDir = normalize(light.position - vertexPos);
    float rawDiffuseCoeff = dot(normal, lightDir);
    float diffuseCoeff = max(rawDiffuseCoeff, 0.f);
    vec3 diffuse = vec3(texture(material.diffuse, texCoords)) * light.diffuse * diffuseCoeff;

    // specular
    vec3 reflectedLightDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(viewPos - vertexPos);
    float specularityCoeff = max(dot(reflectedLightDir, viewDir), 0.f);
    
    vec3 test = vec3(1.f) - vec3(texture(material.specular, texCoords));
    float testCoeff = sign(step(1.f, test.x) + step(1.f, test.y) + step(1.f, test.z));
    
    vec3 specular = test * testCoeff * light.specular * pow(specularityCoeff, material.shininess);
    specular *= step(0.f, rawDiffuseCoeff);
    
    color = vec4(ambient + diffuse + specular, 1.f);
}
