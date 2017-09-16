#version 330 core

in vec2 TexCoord;
out vec4 outColor;

uniform sampler2D tex;

void main() {
	vec2 pos = texture(tex, vec2(TexCoord.x, TexCoord.y)).xy;
	float dst = distance(TexCoord, pos);
	outColor = vec4(vec3(1.0-(dst*5.0)), 1.0);
}
