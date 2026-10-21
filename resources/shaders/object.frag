#version 330 core

struct Material {
    sampler2D diffuse;   // object self color
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 direction;
    float innerBorder;
    float outerBorder;
    
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
    // ambient
    vec3 ambient = vec3(texture(material.diffuse, texCoords)) * light.ambient;
    
    float theta = dot(normalize(-light.direction), normalize(light.position - vertexPos));    
    if (theta > light.outerBorder) {        
        vec3 normal = normalize(normalDir);
        vec3 distance = light.position - vertexPos;
    
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

        // coeffs
        float borderCoeff = clamp((theta - light.outerBorder) / (light.innerBorder - light.outerBorder), 0.f, 1.f);

        float dl = length(distance);
        float attenuationCoeff = 1.f / (light.constant + dl * light.linear + dl * dl * light.quadratic);
        
        vec3 finalColor = ambient + (diffuse + specular) * borderCoeff * attenuationCoeff;
        color = vec4(finalColor, 1.f);
    } 
    else {
        color = vec4(ambient, 1.f);
    }
}
