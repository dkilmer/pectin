#version 330 core

in vec2 TexCoord;
out vec4 outColor;

uniform float step_x;
uniform float step_y;
uniform float da;
uniform float db;
uniform float f;
uniform float k;
uniform sampler2D tex;

float a_val(float xoff, float yoff) {
	return texture(tex, vec2(TexCoord.x+(xoff * step_x), TexCoord.y+(yoff * step_y))).r;
}

float b_val(float xoff, float yoff) {
	return texture(tex, vec2(TexCoord.x+(xoff * step_x), TexCoord.y+(yoff * step_y))).b;
}

float laplace_a() {
	float a = a_val(0,0) * -1;
	a = a + (a_val(-1,0) * 0.2);
	a = a + (a_val(1,0) * 0.2);
	a = a + (a_val(0,-1) * 0.2);
	a = a + (a_val(0,1) * 0.2);
	a = a + (a_val(-1,-1) * 0.05);
	a = a + (a_val(1,-1) * 0.05);
	a = a + (a_val(-1,1) * 0.05);
	a = a + (a_val(1,1) * 0.05);
	return a;
}

float laplace_b() {
	float b = b_val(0,0) * -1;
	b = b + (b_val(-1,0) * 0.2);
	b = b + (b_val(1,0) * 0.2);
	b = b + (b_val(0,-1) * 0.2);
	b = b + (b_val(0,1) * 0.2);
	b = b + (b_val(-1,-1) * 0.05);
	b = b + (b_val(1,-1) * 0.05);
	b = b + (b_val(-1,1) * 0.05);
	b = b + (b_val(1,1) * 0.05);
	return b;
}


void main() {
	float la = laplace_a();
	float lb = laplace_b();
	float a = a_val(0,0);
	float b = b_val(0,0);

	a = a + ((da * la) - (a * b * b) + (f * (1.0 - a)));
	b = b + ((db * lb) + (a * b * b) - ((k + f) * b));

	outColor = vec4(clamp(a,0,1), 0, clamp(b,0,1), 1);
}