#version 330 core

in vec2 TexCoord;
out vec4 outColor;

uniform sampler2D tex;

void main() {
	vec4 c = texture(tex, vec2(TexCoord.x, TexCoord.y));
	float ac = 1.0-smoothstep(0.5,1,c.r);
	outColor =  vec4(ac/4,ac,0,1);
}
