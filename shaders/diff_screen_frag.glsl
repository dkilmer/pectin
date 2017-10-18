#version 330 core

in vec2 TexCoord;
out vec4 outColor;

uniform sampler2D tex;

void main() {
	outColor = texture(tex, vec2(TexCoord.x, TexCoord.y));
}
