#version 330 core

#define MAX_LIGHTS 10

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 Color[];
in vec3 ScaleRot[];
in uvec3 SprOffset[];

smooth out vec3 FColor;
out vec2 TexCoord;
smooth out vec3 Intensity;
out vec4 WorldPos;

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
	vec3 ambient = light.ambient_coefficient * light.light_color;

	//diffuse
	float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
	vec3 diffuse = diffuseCoefficient * light.light_color;
	return attenuation*diffuse;
}

void emit_point(mat3 rotation, vec3 pt, vec3 offset, vec2 tc, vec3 norm) {
	vec4 pos = vec4((rotation * offset) + pt, 1.0);
	vec3 surfaceToCamera = normalize(camera_pos - pos.xyz);
	vec3 linearColor = vec3(0);
	for(int i = 0; i < num_lights; i++){
		linearColor += ApplyLight(all_lights[i], norm, pos.xyz, surfaceToCamera);
	}
	TexCoord = tc;
  Intensity = linearColor;
  gl_Position = vp * pos;
  WorldPos = pos;
  EmitVertex();
}

void main() {
	vec3 pt = gl_in[0].gl_Position.xyz;
	float rot = ScaleRot[0].z;
	float scale_x = ScaleRot[0].x;
	float scale_y = ScaleRot[0].y;
	mat3 rotation = mat3(
    vec3( cos(rot),  sin(rot),  0.0),
    vec3(-sin(rot),  cos(rot),  0.0),
    vec3(0.0,        0.0,       1.0)
  );

  uint col = SprOffset[0].x % SprOffset[0].y;
  uint row = SprOffset[0].x / SprOffset[0].y;
  float zx = float(col) * tex_mult.x;
  float zy = float(row) * tex_mult.y;
  float ox = zx + tex_mult.x;
  float oy = zy + tex_mult.y;
	vec4 pos;

	vec3 norm = vec3(0, 0, 1);
  FColor = Color[0];
  emit_point(rotation, pt, vec3(-scale_x, -scale_y, pt.z), vec2(zx, oy), norm);
  emit_point(rotation, pt, vec3(-scale_x, scale_y, pt.z), vec2(zx, zy), norm);
  emit_point(rotation, pt, vec3(scale_x, -scale_y, pt.z), vec2(ox, oy), norm);
  emit_point(rotation, pt, vec3(scale_x, scale_y, pt.z), vec2(ox, zy), norm);
  EndPrimitive();
}