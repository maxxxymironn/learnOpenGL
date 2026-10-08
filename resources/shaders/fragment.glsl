#version 330 core

in vec3 f_color;
in vec2 f_texCoord;

out vec4 color;

uniform float colorCoeff;
uniform sampler2D tex1;
uniform sampler2D tex2;

void main() {
    
    color = mix(texture(tex2, f_texCoord) * vec4(f_color, 1.f), texture(tex1, f_texCoord) * colorCoeff, 0.9f);
}