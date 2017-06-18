#version 330 core

in vec3 FColor;

out vec4 outColor;

void main() {
	outColor = vec4(FColor, 0.3);
}
