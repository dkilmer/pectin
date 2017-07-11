#version 330 core

#define MAX_LIGHTS 10

layout(points) in;
layout(triangle_strip, max_vertices = 20) out;

in vec3 Color[];
in vec3 ScaleRot[];
in uvec3 SprOffset[];

smooth out vec3 FColor;
out vec2 TexCoord;
out vec4 ShadowCoord;
out vec3 Intensity;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 vp;
uniform mat4 depth_bias_vp;
uniform vec2 tex_mult;
uniform int num_lights;
uniform struct Light {
	vec4 position;
	vec3 intensities; //a.k.a the color of the light
	float attenuation;
	float ambient_coefficient;
	float cone_angle;
	vec3 cone_direction;
} all_lights[MAX_LIGHTS];
uniform vec3 camera_pos;
uniform float shininess;
uniform vec3 spec_color;

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
		float distanceToLight = length(light.position.xyz - surfacePos);
		attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));

		//cone restrictions (affects attenuation)
		float lightToSurfaceAngle = degrees(acos(dot(-surfaceToLight, normalize(light.cone_direction))));
		if(lightToSurfaceAngle > light.cone_angle){
			attenuation = 0.0;
		}
	}

	//ambient
	//vec3 ambient = light.ambient_coefficient * surfaceColor * light.intensities;
	vec3 ambient = light.ambient_coefficient * light.intensities;

	//diffuse
	float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
	//vec3 diffuse = diffuseCoefficient * surfaceColor * light.intensities;
	vec3 diffuse = diffuseCoefficient * light.intensities;

	//specular
	float specularCoefficient = 0.0;
	if(diffuseCoefficient > 0.0)
		specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), shininess);
	vec3 specular = specularCoefficient * spec_color * light.intensities;

	//linear color (color before gamma correction)
	return ambient + attenuation*(diffuse + specular);
}


void emit_point(mat3 rotation, vec3 pt, vec3 offset, vec2 tc, vec3 norm, float occ) {
	vec4 pos = vec4((rotation * offset) + pt, 1.0);
	vec3 surfaceToCamera = normalize(camera_pos - pos.xyz);
	vec3 linearColor = vec3(0);
	for(int i = 0; i < num_lights; ++i){
		linearColor += ApplyLight(all_lights[i], norm, pos.xyz, surfaceToCamera);
	}
  gl_Position = vp * pos;
  FColor = vec3(occ);
	TexCoord = tc;
	ShadowCoord = depth_bias_vp * pos;
  Intensity = linearColor;
  WorldPos = pos.xyz;
  Normal = norm;
  EmitVertex();
}

void main() {
	vec3 pt = gl_in[0].gl_Position.xyz;
	float rot = ScaleRot[0].z;
	float scale_x = ScaleRot[0].x;
	float scale_y = ScaleRot[0].y;
	float scale_z = Color[0].r;
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
	sur[0] = ((adj & 0x01u) == 0x01u) ? 0u : 1u;
	sur[1] = ((adj & 0x02u) == 0x02u) ? 0u : 1u;
	sur[2] = ((adj & 0x04u) == 0x04u) ? 0u : 1u;
	sur[3] = ((adj & 0x08u) == 0x08u) ? 0u : 1u;
	sur[4] = ((adj & 0x10u) == 0x10u) ? 0u : 1u;
	sur[5] = ((adj & 0x20u) == 0x20u) ? 0u : 1u;
	sur[6] = ((adj & 0x40u) == 0x40u) ? 0u : 1u;
	sur[7] = ((adj & 0x80u) == 0x80u) ? 0u : 1u;
	sur[8] = ((adj & 0x100u) == 0x100u) ? 0u : 1u;
	sur[9] = ((adj & 0x200u) == 0x200u) ? 0u : 1u;
	sur[10] = ((adj & 0x400u) == 0x400u) ? 0u : 1u;
	sur[11] = ((adj & 0x800u) == 0x800u) ? 0u : 1u;
	sur[12] = ((adj & 0x1000u) == 0x1000u) ? 0u : 1u;
	sur[13] = ((adj & 0x2000u) == 0x2000u) ? 0u : 1u;
	sur[14] = ((adj & 0x4000u) == 0x4000u) ? 0u : 1u;
	sur[15] = ((adj & 0x8000u) == 0x8000u) ? 0u : 1u;
	sur[16] = ((adj & 0x10000u) == 0x10000u) ? 0u : 1u;
	sur[17] = ((adj & 0x20000u) == 0x20000u) ? 0u : 1u;
	sur[18] = ((adj & 0x40000u) == 0x40000u) ? 0u : 1u;
	sur[19] = ((adj & 0x80000u) == 0x80000u) ? 0u : 1u;
	sur[20] = ((adj & 0x100000u) == 0x100000u) ? 0u : 1u;
	sur[21] = ((adj & 0x200000u) == 0x200000u) ? 0u : 1u;
	sur[22] = ((adj & 0x400000u) == 0x400000u) ? 0u : 1u;
	sur[23] = ((adj & 0x800000u) == 0x800000u) ? 0u : 1u;
	sur[24] = ((adj & 0x1000000u) == 0x1000000u) ? 0u : 1u;
	sur[25] = ((adj & 0x2000000u) == 0x2000000u) ? 0u : 1u;
	sur[26] = ((adj & 0x4000000u) == 0x4000000u) ? 0u : 1u;

	float vocc[8];
	// front
	// bottom left
	vocc[0] = min(float(sur[0]+sur[1]+sur[3]+sur[4]+sur[9]+sur[10]+sur[12]+sur[13]), 5.0) / 5.0;
	// bottom right
	vocc[1] = min(float(sur[1]+sur[2]+sur[4]+sur[5]+sur[10]+sur[11]+sur[13]+sur[14]), 5.0) / 5.0;
	// top left
	vocc[2] = min(float(sur[3]+sur[4]+sur[6]+sur[7]+sur[12]+sur[13]+sur[15]+sur[16]), 5.0) / 5.0;
	// top right
	vocc[3] = min(float(sur[4]+sur[5]+sur[7]+sur[8]+sur[13]+sur[14]+sur[16]+sur[17]), 5.0) / 5.0;
	// back
	// bottom left
	vocc[4] = min(float(sur[9]+sur[10]+sur[12]+sur[13]+sur[18]+sur[19]+sur[21]+sur[22]), 5.0) / 5.0;
	// bottom right
	vocc[5] = min(float(sur[10]+sur[11]+sur[13]+sur[14]+sur[19]+sur[20]+sur[22]+sur[23]), 5.0) / 5.0;
	// top left
	vocc[6] = min(float(sur[12]+sur[13]+sur[15]+sur[16]+sur[21]+sur[22]+sur[24]+sur[25]), 5.0) / 5.0;
	// top right
	vocc[7] = min(float(sur[13]+sur[14]+sur[16]+sur[17]+sur[22]+sur[23]+sur[25]+sur[26]), 5.0) / 5.0;

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

  // bottom
  if ((adj & 0x400u) == 0u) {
	  norm = vec3(0, -1, 0);
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.0), tpi, norm, vocc[1]);
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, -scale_z), tpl, norm, vocc[5]);
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.0), tph, norm, vocc[0]);
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, -scale_z), tpk, norm, vocc[4]);
	  EndPrimitive();
  }
  // left
  if ((adj & 0x1000u) == 0u) {
	  norm = vec3(-1, 0, 0);
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.0), tph, norm, vocc[0]);
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, -scale_z), tpg, norm, vocc[4]);
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.0), tpc, norm, vocc[2]);
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, -scale_z), tpd, norm, vocc[6]);
	  EndPrimitive();
	}
  // right
  if ((adj & 0x4000u) == 0u) {
	  norm = vec3(1, 0, 0);
	  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.0), tpe, norm, vocc[3]);
	  emit_point(rotation, pt, vec3(scale_x, scale_y, -scale_z), tpf, norm, vocc[7]);
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.0), tpi, norm, vocc[1]);
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, -scale_z), tpj, norm, vocc[5]);
	  EndPrimitive();
	}
  // top
  if ((adj & 0x10000u) == 0u) {
	  norm = vec3(0, 1, 0);
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.0), tpd, norm, vocc[2]);
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, -scale_z), tpa, norm, vocc[6]);
	  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.0), tpe, norm, vocc[3]);
	  emit_point(rotation, pt, vec3(scale_x, scale_y, -scale_z), tpb, norm, vocc[7]);
	  EndPrimitive();
	}
  // front
  if ((adj & 0x10u) == 0u) {
	  norm = vec3(0, 0, 1);
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.0), tph, norm, vocc[0]);
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.0), tpd, norm, vocc[2]);
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.0), tpi, norm, vocc[1]);
	  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.0), tpe, norm, vocc[3]);
	  EndPrimitive();
	}
}
