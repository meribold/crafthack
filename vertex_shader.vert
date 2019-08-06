#version 330 core

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec2 iTextureCoord;

out vec2 textureCoord;

uniform mat4 tMatrix;

void main() {
    gl_Position = tMatrix * vec4(iPosition, 1.0);
    textureCoord = iTextureCoord;
}
