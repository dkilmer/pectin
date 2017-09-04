#version 330 core

in vec3 FColor;
in vec2 TexCoord;
in vec4 WorldPos;

out vec4 outColor;

uniform sampler2D tex;

vec3 rgb2hsv(vec3 c) {
	vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
	vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
	vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

	float d = q.x - min(q.w, q.y);
	float e = 1.0e-10;
	return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c) {
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
  vec4 diffuseColor = texture(tex, TexCoord);
  if (diffuseColor.a < 0.4) discard;

  vec3 hsvColor = rgb2hsv(diffuseColor.rgb);
	hsvColor.b += clamp(((FColor.r * 0.15f) - 0.05f), 0.0, 1.0);

  outColor = vec4(hsv2rgb(hsvColor), diffuseColor.a);
}
