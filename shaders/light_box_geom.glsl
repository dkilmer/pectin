#version 330 core

#define MAX_LIGHTS 10

layout(points) in;
layout(triangle_strip, max_vertices = 20) out;

in vec3 Color[];
in vec3 ScaleRot[];
in uvec3 SprOffset[];

smooth out vec3 FColor;
smooth out vec3 Occlusion;
out vec2 TexCoord;
smooth out vec3 Intensity;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 vp;
uniform vec2 tex_mult;
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

vec3 ApplyLight(Light light, vec3 normal, vec3 surfacePos, vec3 surfaceToCamera) {
	vec3 surfaceToLight;
	float attenuation = 1.0;
	if(light.position.w == 0.0) {
		//directional light
		surfaceToLight = normalize(light.position.xyz);
		attenuation = 1.0; //no attenuation for directional lights
	} else {
		//point light
		surfaceToLight = normalize(light.position.xyz - surfacePos);
		float distanceToLight = length(surfacePos - light.position.xyz);
		attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));
	}

	//ambient
	//vec3 ambient = light.ambient_coefficient * surfaceColor * light.light_color;
	vec3 ambient = light.ambient_coefficient * light.light_color;

	//diffuse
	float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
	//vec3 diffuse = diffuseCoefficient * surfaceColor * light.light_color;
	vec3 diffuse = diffuseCoefficient * light.light_color;

	//specular
	//	float specularCoefficient = 0.0;
	//	if(diffuseCoefficient > 0.0)
	//		specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), shininess);
	//	vec3 specular = specularCoefficient * spec_color * light.light_color;

	//linear color (color before gamma correction)
	//return ambient + attenuation*(diffuse + specular);
	//return diffuse + attenuation;
	return attenuation*diffuse;
}


void emit_point(mat3 rotation, vec3 pt, vec3 offset, vec2 tc, vec3 norm, float occ) {
	vec4 pos = vec4((rotation * offset) + pt, 1.0);
	vec3 surfaceToCamera = normalize(camera_pos - pos.xyz);
	vec3 linearColor = vec3(0);
	for(int i = 0; i < num_lights; i++){
		linearColor += ApplyLight(all_lights[i], norm, pos.xyz, surfaceToCamera);
	}
  gl_Position = vp * pos;
  Occlusion = vec3(occ);
	TexCoord = tc;
  Intensity = linearColor;
  WorldPos = pos.xyz;
  Normal = norm;
  EmitVertex();
}

float calc_occlusion(uint s1, uint s2, uint c) {
	if (s1 == 1u && s2 == 1u) return 0.25;
	uint o = (3u - (s1 + s2 + c)) + 1u;
	return float(o) / 4.0;
}

void main() {
	vec3 pt = gl_in[0].gl_Position.xyz;
	float rot = ScaleRot[0].z;
	float scale_x = ScaleRot[0].x;
	float scale_y = ScaleRot[0].y;
	float scale_z = 1.0;
	mat3 rotation = mat3(
    vec3( cos(rot),  sin(rot),  0.0),
    vec3(-sin(rot),  cos(rot),  0.0),
    vec3(0.0,        0.0,       1.0)
  );

  uint col = SprOffset[0].x % SprOffset[0].y;
  uint row = SprOffset[0].x / SprOffset[0].y;
  uint adj = SprOffset[0].z;
  float thx = tex_mult.x * 0.25;
  float thy = tex_mult.y * 0.25;
  float zx = tex_mult.x * float(col);
  float zy = tex_mult.y * float(row);

  uint sur[27];
	sur[0] = ((adj & 0x01u) == 0x01u) ? 1u : 0u;
	sur[1] = ((adj & 0x02u) == 0x02u) ? 1u : 0u;
	sur[2] = ((adj & 0x04u) == 0x04u) ? 1u : 0u;
	sur[3] = ((adj & 0x08u) == 0x08u) ? 1u : 0u;
	sur[4] = ((adj & 0x10u) == 0x10u) ? 1u : 0u;
	sur[5] = ((adj & 0x20u) == 0x20u) ? 1u : 0u;
	sur[6] = ((adj & 0x40u) == 0x40u) ? 1u : 0u;
	sur[7] = ((adj & 0x80u) == 0x80u) ? 1u : 0u;
	sur[8] = ((adj & 0x100u) == 0x100u) ? 1u : 0u;
	sur[9] = ((adj & 0x200u) == 0x200u) ? 1u : 0u;
	sur[10] = ((adj & 0x400u) == 0x400u) ? 1u : 0u;
	sur[11] = ((adj & 0x800u) == 0x800u) ? 1u : 0u;
	sur[12] = ((adj & 0x1000u) == 0x1000u) ? 1u : 0u;
	sur[13] = ((adj & 0x2000u) == 0x2000u) ? 1u : 0u;
	sur[14] = ((adj & 0x4000u) == 0x4000u) ? 1u : 0u;
	sur[15] = ((adj & 0x8000u) == 0x8000u) ? 1u : 0u;
	sur[16] = ((adj & 0x10000u) == 0x10000u) ? 1u : 0u;
	sur[17] = ((adj & 0x20000u) == 0x20000u) ? 1u : 0u;
	sur[18] = ((adj & 0x40000u) == 0x40000u) ? 1u : 0u;
	sur[19] = ((adj & 0x80000u) == 0x80000u) ? 1u : 0u;
	sur[20] = ((adj & 0x100000u) == 0x100000u) ? 1u : 0u;
	sur[21] = ((adj & 0x200000u) == 0x200000u) ? 1u : 0u;
	sur[22] = ((adj & 0x400000u) == 0x400000u) ? 1u : 0u;
	sur[23] = ((adj & 0x800000u) == 0x800000u) ? 1u : 0u;
	sur[24] = ((adj & 0x1000000u) == 0x1000000u) ? 1u : 0u;
	sur[25] = ((adj & 0x2000000u) == 0x2000000u) ? 1u : 0u;
	sur[26] = ((adj & 0x4000000u) == 0x4000000u) ? 1u : 0u;

  vec2 tpa = vec2(zx+thx, zy);
  vec2 tpb = vec2(zx+thx+thx, zy);
  vec2 tpc = vec2(zx, zy+thy);
  vec2 tpd = vec2(zx+thx, zy+thy);
  vec2 tpe = vec2(zx+thx+thx, zy+thy);
  vec2 tpf = vec2(zx+thx+thx+thx, zy+thy);
  vec2 tpg = vec2(zx, zy+thy+thy);
  vec2 tph = vec2(zx+thx, zy+thy+thy);
  vec2 tpi = vec2(zx+thx+thx, zy+thy+thy);
  vec2 tpj = vec2(zx+thx+thx+thx, zy+thy+thy);
  vec2 tpk = vec2(zx+thx, zy+thy+thy+thy);
  vec2 tpl = vec2(zx+thx+thx, zy+thy+thy+thy);

	vec4 pos;
	vec3 norm;
	FColor = Color[0];

  // bottom
  if (sur[10] == 0u) {
	  norm = vec3(0, -1, 0);
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.0), tpi, norm, calc_occlusion(sur[1], sur[11], sur[2]));
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, -scale_z), tpl, norm, calc_occlusion(sur[11], sur[19], sur[20]));
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.0), tph, norm, calc_occlusion(sur[1], sur[9], sur[0]));
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, -scale_z), tpk, norm, calc_occlusion(sur[9], sur[19], sur[18]));
	  EndPrimitive();
  }
  // left
  if (sur[12] == 0u) {
	  norm = vec3(-1, 0, 0);
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.0), tpd, norm, calc_occlusion(sur[3], sur[9], sur[0]));
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, -scale_z), tpc, norm, calc_occlusion(sur[9], sur[21], sur[18]));
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.0), tph, norm, calc_occlusion(sur[3], sur[15], sur[6]));
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, -scale_z), tpg, norm, calc_occlusion(sur[15], sur[21], sur[24]));
	  EndPrimitive();
	}
  // right
  if (sur[14] == 0u) {
	  norm = vec3(1, 0, 0);
	  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.0), tpe, norm, calc_occlusion(sur[5], sur[17], sur[8]));
	  emit_point(rotation, pt, vec3(scale_x, scale_y, -scale_z), tpf, norm, calc_occlusion(sur[17], sur[23], sur[26]));
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.0), tpi, norm, calc_occlusion(sur[5], sur[11], sur[2]));
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, -scale_z), tpj, norm, calc_occlusion(sur[11], sur[23], sur[20]));
	  EndPrimitive();
	}
  // top
  if (sur[16] == 0u) {
	  norm = vec3(0, 1, 0);
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.0), tpd, norm, calc_occlusion(sur[7], sur[15], sur[6]));
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, -scale_z), tpa, norm, calc_occlusion(sur[15], sur[25], sur[24]));
	  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.0), tpe, norm, calc_occlusion(sur[7], sur[17], sur[8]));
	  emit_point(rotation, pt, vec3(scale_x, scale_y, -scale_z), tpb, norm, calc_occlusion(sur[17], sur[25], sur[26]));
	  EndPrimitive();
	}
  // front
  // if (sur[4] == 1u) {
  if ((adj & 0x10u) == 0u) {
	  norm = vec3(0, 0, 1);
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.0), tph, norm, calc_occlusion(sur[1], sur[3], sur[0]));
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.0), tpd, norm, calc_occlusion(sur[3], sur[7], sur[6]));
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.0), tpi, norm, calc_occlusion(sur[1], sur[5], sur[2]));
	  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.0), tpe, norm, calc_occlusion(sur[5], sur[7], sur[8]));
	  EndPrimitive();
	}
}
