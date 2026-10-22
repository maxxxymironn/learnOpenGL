#version 330 core

in vec2 texCoords;

out vec4 color;

uniform vec3 lightColor;
uniform sampler2D lightPointTex;

void main() {
    color = vec4(texture(lightPointTex, texCoords).xyz * lightColor, 1.f);
}