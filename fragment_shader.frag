#version 330 core

in vec3 color;
in vec2 textureCoord;

out vec4 fragmentColor;

uniform sampler2D textureSampler;

void main() {
    fragmentColor = texture(textureSampler, textureCoord);
}
