#version 330 core

smooth in vec3 FColor;
in vec2 TexCoord;

out vec4 outColor;

uniform sampler2D tex;

void main() {
	outColor = texture(tex, TexCoord);
}
