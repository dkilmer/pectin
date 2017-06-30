#version 330 core

#define MAX_LIGHTS 10

layout(points) in;
layout(triangle_strip, max_vertices = 20) out;

in vec3 Color[];
in vec3 ScaleRot[];
in vec3 SprOffset[];

smooth out vec3 FColor;
out vec2 TexCoord;
out vec3 Intensity;

uniform mat4 vp;
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


void emit_point(mat3 rotation, vec3 pt, vec3 offset, vec2 tc, vec3 norm) {
	vec4 pos = vec4((rotation * offset) + pt, 1.0);
	vec3 surfaceToCamera = normalize(camera_pos - pos.xyz);
	vec3 linearColor = vec3(0);
	for(int i = 0; i < num_lights; ++i){
		linearColor += ApplyLight(all_lights[i], norm, pos.xyz, surfaceToCamera);
	}
  gl_Position = vp * pos;
	TexCoord = tc;
  Intensity = linearColor;
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

  float col = mod(SprOffset[0].x, SprOffset[0].z);
  float row = floor(SprOffset[0].x / SprOffset[0].z);
  float thx = tex_mult.x * 0.25;
  float thy = tex_mult.y * 0.25;
  float zx = col * tex_mult.x;
  float zy = row * tex_mult.y;

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
  norm = vec3(0, -1, 0);
  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.0), tpi, norm);
  emit_point(rotation, pt, vec3(scale_x, -scale_y, -scale_z), tpl, norm);
  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.0), tph, norm);
  emit_point(rotation, pt, vec3(-scale_x, -scale_y, -scale_z), tpk, norm);
  EndPrimitive();
  // left
  norm = vec3(-1, 0, 0);
  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.0), tph, norm);
  emit_point(rotation, pt, vec3(-scale_x, -scale_y, -scale_z), tpg, norm);
  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.0), tpc, norm);
  emit_point(rotation, pt, vec3(-scale_x, scale_y, -scale_z), tpd, norm);
  EndPrimitive();
  // right
  norm = vec3(1, 0, 0);
  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.0), tpe, norm);
  emit_point(rotation, pt, vec3(scale_x, scale_y, -scale_z), tpf, norm);
  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.0), tpi, norm);
  emit_point(rotation, pt, vec3(scale_x, -scale_y, -scale_z), tpj, norm);
  EndPrimitive();
  // top
  norm = vec3(0, 1, 0);
  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.0), tpd, norm);
  emit_point(rotation, pt, vec3(-scale_x, scale_y, -scale_z), tpa, norm);
  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.0), tpe, norm);
  emit_point(rotation, pt, vec3(scale_x, scale_y, -scale_z), tpb, norm);
  EndPrimitive();
  // front
  norm = vec3(0, 0, 1);
  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.0), tph, norm);
  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.0), tpd, norm);
  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.0), tpi, norm);
  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.0), tpe, norm);
  EndPrimitive();
}
