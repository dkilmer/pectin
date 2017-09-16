#version 330 core

in vec2 TexCoord;
out vec4 outColor;

uniform float step;
uniform sampler2D tex;

void main() {
	vec2 pos = texture(tex, vec2(TexCoord.x, TexCoord.y)).xy;
	float dst = distance(TexCoord, pos);
	vec2 p;
	float d;
	p = texture(tex, vec2(TexCoord.x-step, TexCoord.y+step)).xy;
	if (p.x > 0.0 && p.y > 0.0) {
		d = distance(TexCoord, p);
		if (d < dst) {
			dst = d;
			pos = p;
		}
	}
	p = texture(tex, vec2(TexCoord.x, TexCoord.y+step)).xy;
	if (p.x > 0.0 && p.y > 0.0) {
		d = distance(TexCoord, p);
		if (d < dst) {
			dst = d;
			pos = p;
		}
	}
	p = texture(tex, vec2(TexCoord.x+step, TexCoord.y+step)).xy;
	if (p.x > 0.0 && p.y > 0.0) {
		d = distance(TexCoord, p);
		if (d < dst) {
			dst = d;
			pos = p;
		}
	}
	p = texture(tex, vec2(TexCoord.x-step, TexCoord.y)).xy;
	if (p.x > 0.0 && p.y > 0.0) {
		d = distance(TexCoord, p);
		if (d < dst) {
			dst = d;
			pos = p;
		}
	}
	p = texture(tex, vec2(TexCoord.x+step, TexCoord.y)).xy;
	if (p.x > 0.0 && p.y > 0.0) {
		d = distance(TexCoord, p);
		if (d < dst) {
			dst = d;
			pos = p;
		}
	}
	p = texture(tex, vec2(TexCoord.x-step, TexCoord.y-step)).xy;
	if (p.x > 0.0 && p.y > 0.0) {
		d = distance(TexCoord, p);
		if (d < dst) {
			dst = d;
			pos = p;
		}
	}
	p = texture(tex, vec2(TexCoord.x, TexCoord.y-step)).xy;
	if (p.x > 0.0 && p.y > 0.0) {
		d = distance(TexCoord, p);
		if (d < dst) {
			dst = d;
			pos = p;
		}
	}
	p = texture(tex, vec2(TexCoord.x+step, TexCoord.y-step)).xy;
	if (p.x > 0.0 && p.y > 0.0) {
		d = distance(TexCoord, p);
		if (d < dst) {
			dst = d;
			pos = p;
		}
	}
	outColor = vec4(pos.x, pos.y, 0.0, 1.0);
}
