#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 Color[];
in vec3 ScaleRot[];
in vec3 SprOffset[];

smooth out vec3 FColor;
out vec2 TexCoord;
out vec4 WorldPos;

uniform mat4 vp;
uniform vec2 tex_mult;

void emit_point(mat3 rotation, vec3 pt, vec3 offset, vec2 tc) {
	vec4 pos = vec4((rotation * offset) + pt, 1.0);
	TexCoord = tc;
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

  float col = mod(SprOffset[0].x, SprOffset[0].z);
  float row = floor(SprOffset[0].x / SprOffset[0].z);
  float zx = col * tex_mult.x;
  float zy = row * tex_mult.y;
  float ox = zx + tex_mult.x;
  float oy = zy + tex_mult.y;
	vec4 pos;

  FColor = Color[0];
  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.0), vec2(zx, oy));
  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.0), vec2(zx, zy));
  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.0), vec2(ox, oy));
  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.0), vec2(ox, zy));
  EndPrimitive();
}