#version 330 core

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec3 iColor;
layout (location = 2) in vec2 iTextureCoord;

out vec3 color;
out vec2 textureCoord;

void main() {
    gl_Position = vec4(iPosition, 1.0);
    color = iColor;
    textureCoord = iTextureCoord;
}
