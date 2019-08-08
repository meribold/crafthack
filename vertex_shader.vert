#version 330 core

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec2 iTextureCoord;

out vec2 textureCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(iPosition, 1.0);
    textureCoord = iTextureCoord;
}
