#version 330 core

#define MAX_LIGHTS 10

smooth in vec3 FColor;
smooth in vec3 Occlusion;
in vec2 TexCoord;
//in vec4 ShadowCoord;
in vec3 WorldPos;
smooth in vec3 Intensity;
in vec3 Normal;

out vec4 outColor;

uniform sampler2D tex;
uniform samplerCube shadow_map;
uniform int num_lights;
uniform struct Light {
	vec4 position;
	vec3 light_color;
	float attenuation;
	float ambient_coefficient;
	float cone_angle;
	vec3 cone_direction;
} all_lights[MAX_LIGHTS];
uniform vec3 camera_pos;
uniform vec3 light_pos;
uniform float far_plane;
uniform float bias;

float ShadowCalculation(vec3 frag_pos)
{
//	vec3 projCoords = shadow_coord.xyz / shadow_coord.w;
//	projCoords = (projCoords * 0.5) + 0.5;
//	float closestDepth = texture(shadow_map, projCoords.xy).r;
	vec3 frag_to_light = frag_pos - light_pos;
  float closestDepth = texture(shadow_map, frag_to_light).r;
  //closestDepth = closestDepth * far_plane;
  float currentDepth = length(frag_to_light) / far_plane;
  // check whether current frag pos is in shadow
	//float bias = 0.02;
	//float bias = max(0.05 * (1.0 - dot(Normal, frag_to_light/far_plane)), 0.005);
	float shadow = (currentDepth - bias) > closestDepth  ? 0.5 : 1.0;
  //float shadow = currentDepth > closestDepth  ? 0.0 : 1.0;
  //return closestDepth;
  //return currentDepth;
  return shadow;
}

void main() {
	vec4 presetColor = vec4(FColor, 1.0);
	vec4 color = texture(tex, TexCoord);

	float shadow = ShadowCalculation(WorldPos);
	//outColor = vec4(vec3(shadow), 1.0);
	outColor = vec4(((color.rgb * Occlusion)+0.1) * Intensity * shadow, color.a);
	//outColor = vec4(vec3(shadow), 1.0);
	//outColor = vec4(vec3(ShadowCalculation(ShadowCoord)), 1.0);
	//outColor = texture(shadow_map, ShadowCoord.xy);
	//outColor = vec4(color.rgb * Occlusion, color.a);
	//outColor = vec4(Intensity * (color.rgb * Occlusion), color.a);
	//outColor = vec4(Intensity * shadow, 1.0);
}
