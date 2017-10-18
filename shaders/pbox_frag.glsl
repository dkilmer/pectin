#version 330 core

in vec3 FColor;
in vec4 WorldPos;

out vec4 outColor;

void main() {
  outColor = vec4(FColor, 1.0);
}
