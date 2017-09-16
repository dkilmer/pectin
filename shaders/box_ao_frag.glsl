#version 330 core

smooth in vec3 FColor;
smooth in vec3 Occlusion;
in vec2 TexCoord;
in vec3 WorldPos;
smooth in vec3 Intensity;
in vec3 Normal;

out vec4 outColor;

uniform sampler2D tex;
uniform struct Light {
	vec4 position;
	vec3 light_color;
	float attenuation;
	float ambient_coefficient;
	float cone_angle;
	vec3 cone_direction;
} pt_light;

void main() {
	vec4 presetColor = vec4(FColor, 1.0);
	vec4 color = texture(tex, TexCoord);
	outColor = vec4(((color.rgb * Occlusion)+0.1) * Intensity, color.a);
}
