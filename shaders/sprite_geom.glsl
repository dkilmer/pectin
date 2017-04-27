#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 Color[];
in vec3 ScaleRot[];
in vec2 SprOffset[];

smooth out vec3 FColor;
out vec2 TexCoord;
out vec4 WorldPos;

uniform mat4 vp;
uniform vec2 tex_mult;

void main() {
	vec3 g1 = gl_in[0].gl_Position.xyz;
	float rot = ScaleRot[0].z;
	float scale_x = ScaleRot[0].x;
	float scale_y = ScaleRot[0].y;
	mat3 rotation = mat3(
    vec3( cos(rot),  sin(rot),  0.0),
    vec3(-sin(rot),  cos(rot),  0.0),
    vec3(0.0,        0.0,       1.0)
  );

  float col = SprOffset[0].y;
  float row = SprOffset[0].x;
  float zx = col * tex_mult.x;
  float zy = row * tex_mult.y;
  float ox = zx + tex_mult.x;
  float oy = zy + tex_mult.y;

  FColor = Color[0];

	vec4 pos1 = vec4((rotation * vec3(-scale_x, -scale_y, 0.0))+g1, 1.0);
	TexCoord = vec2(zx, oy);
  gl_Position = vp * pos1;
  WorldPos = pos1;
  EmitVertex();

	vec4 pos2 = vec4((rotation * vec3(-scale_x, scale_y, 0.0))+g1, 1.0);
	TexCoord = vec2(zx, zy);
  gl_Position = vp * pos2;
  WorldPos = pos2;
  EmitVertex();

	vec4 pos3 = vec4((rotation * vec3(scale_x, -scale_y, 0.0))+g1, 1.0);
	TexCoord = vec2(ox, oy);
  gl_Position = vp * pos3;
  WorldPos = pos3;
  EmitVertex();

	vec4 pos4 = vec4((rotation * vec3(scale_x, scale_y, 0.0))+g1, 1.0);
	TexCoord = vec2(ox, zy);
  gl_Position = vp * pos4;
  WorldPos = pos4;
  EmitVertex();

  EndPrimitive();
}