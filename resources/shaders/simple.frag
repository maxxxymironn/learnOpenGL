#version 330 core

in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;

void main() {
    FragColor = texture(texture_diffuse1, texCoords);
    // FragColor = vec4(0.3f, 0.6f, 0.2f, 1.f);
}
