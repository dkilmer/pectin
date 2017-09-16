#version 330 core

in vec2 TexCoord;
out vec4 outColor;

void main() {
	outColor = vec4(TexCoord.x, TexCoord.y, 0.0, 1.0);
	//outColor = vec4(1.0);
}
