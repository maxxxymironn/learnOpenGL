#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirectionalLigth {
    vec3 direction;

    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct Flashlight {
    vec3 position;
    vec3 direction;

    vec3 diffuse;
    vec3 specular;

    float innerBorder;
    float outerBorder;

    float constant;
    float linear;
    float quadratic;
};

in vec3 normalDir;
in vec3 vertexPos;
in vec2 texCoords;

out vec4 color;

uniform vec3 viewPos;
uniform vec3 ambientLight;

uniform Material material;
uniform DirectionalLigth directLight;
#define POINT_LIGHT_NUM 5
uniform PointLight pointLight[POINT_LIGHT_NUM];
uniform Flashlight flashlight;

vec3 calculateDirectLight(vec3 normal, vec3 viewDir);
vec3 calculatePointLight(vec3 normal, vec3 viewDir, PointLight light);
vec3 calculateFlashlight(vec3 normal, vec3 viewDir);

void main() {
    vec3 startColor = ambientLight * texture(material.diffuse, texCoords).xyz;

    vec3 normal = normalize(normalDir);
    vec3 viewDir = normalize(viewPos - vertexPos);

    startColor += calculateDirectLight(normal, viewDir);
    startColor += calculateFlashlight(normal, viewDir);
    for (int i = 0; i < POINT_LIGHT_NUM; ++i) {
        startColor += calculatePointLight(normal, viewDir, pointLight[i]);
    }

    color = vec4(startColor, 1.f);
}

vec3 calculateDirectLight(vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-directLight.direction);

    // diffuse
    float diffuseCoeff = max(dot(normal, lightDir), 0.f);
    vec3 diffuse = texture(material.diffuse, texCoords).xyz * directLight.diffuse * diffuseCoeff;

    // specular
    vec3 reflectedLightDir = reflect(-lightDir, normal);
    float dotCoeff = max(dot(reflectedLightDir, viewDir), 0.f);
    float specularityCoeff = pow(dotCoeff, material.shininess);
    vec3 specular = texture(material.specular, texCoords).xyz * directLight.specular * specularityCoeff;

    return diffuse + specular;
}

vec3 calculatePointLight(vec3 normal, vec3 viewDir, PointLight light) {
    vec3 lightDir = normalize(light.position - vertexPos);

    // diffuse
    float diffuseCoeff = max(dot(lightDir, normal), 0.f);
    vec3 diffuse = texture(material.diffuse, texCoords).xyz * light.diffuse * diffuseCoeff;

    // specular
    vec3 reflectedLightDir = reflect(-lightDir, normal);
    float dotCoeff = max(dot(reflectedLightDir, viewDir), 0.f);
    float specularityCoeff = pow(dotCoeff, material.shininess);
    vec3 specular = texture(material.specular, texCoords).xyz * light.specular * specularityCoeff;

    // attenuation
    float distance = length(light.position - vertexPos);
    float attenuationCoeff = 1.f / (light.constant + distance * light.linear + distance * distance * light.quadratic);

    return (diffuse + specular) * attenuationCoeff;
}

vec3 calculateFlashlight(vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(flashlight.position - vertexPos);
    float theta = dot(-lightDir, normalize(flashlight.direction));
    if (theta > flashlight.outerBorder) {
        // diffuse
        float diffuseCoeff = max(dot(lightDir, normal), 0.f);
        vec3 diffuse = texture(material.diffuse, texCoords).xyz * flashlight.diffuse * diffuseCoeff;

        // specular
        vec3 reflectedLightDir = reflect(-lightDir, normal);
        float dotCoeff = max(dot(reflectedLightDir, viewDir), 0.f);
        float specularityCoeff = pow(dotCoeff, material.shininess);
        vec3 specular = texture(material.specular, texCoords).xyz * flashlight.specular * specularityCoeff;

        // onBorderCoeff
        float onBorderCoeff = clamp((theta - flashlight.outerBorder) / (flashlight.innerBorder - flashlight.outerBorder), 0.f, 1.f);

        // attenuation
        float distance = length(flashlight.position - vertexPos);
        float attenuation = 1.f / (flashlight.constant + distance * flashlight.linear + distance * distance * flashlight.quadratic);

        return (diffuse + specular) * onBorderCoeff * attenuation;
    }
    else return vec3(0.f);
}
