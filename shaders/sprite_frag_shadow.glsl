#version 330 core

in vec3 FColor;
in vec2 TexCoord;
in vec4 WorldPos;

out vec4 outColor;

uniform sampler2D tex;
uniform samplerCube shadow_map;
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
  //RGBA of our diffuse color
  vec4 DiffuseColor = texture(tex, TexCoord);
  if (DiffuseColor.a < 0.4) discard;
  //outColor = vec4(FColor.rgb, DiffuseColor.a);
  outColor = DiffuseColor;
}
