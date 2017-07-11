#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 20) out;

in vec3 Color[];
in vec3 ScaleRot[];
in uvec3 SprOffset[];

smooth out vec3 FColor;
out vec2 TexCoord;

uniform mat4 depth_vp;
uniform vec2 tex_mult;

void emit_point(mat3 rotation, vec3 pt, vec3 offset, vec2 tc, vec3 norm) {
	vec4 pos = vec4((rotation * offset) + pt, 1.0);
  gl_Position = depth_vp * pos;
	TexCoord = tc;
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
  float zx = float(col) * tex_mult.x;
  float zy = float(row) * tex_mult.y;

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
  if ((adj & 0x400u) == 0u) {
	  norm = vec3(0, -1, 0);
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.0), tpi, norm);
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, -scale_z), tpl, norm);
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.0), tph, norm);
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, -scale_z), tpk, norm);
	  EndPrimitive();
  }
  // left
  if ((adj & 0x1000u) == 0u) {
	  norm = vec3(-1, 0, 0);
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.0), tph, norm);
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, -scale_z), tpg, norm);
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.0), tpc, norm);
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, -scale_z), tpd, norm);
	  EndPrimitive();
	}
  // right
  if ((adj & 0x4000u) == 0u) {
	  norm = vec3(1, 0, 0);
	  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.0), tpe, norm);
	  emit_point(rotation, pt, vec3(scale_x, scale_y, -scale_z), tpf, norm);
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.0), tpi, norm);
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, -scale_z), tpj, norm);
	  EndPrimitive();
	}
  // top
  if ((adj & 0x10000u) == 0u) {
	  norm = vec3(0, 1, 0);
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.0), tpd, norm);
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, -scale_z), tpa, norm);
	  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.0), tpe, norm);
	  emit_point(rotation, pt, vec3(scale_x, scale_y, -scale_z), tpb, norm);
	  EndPrimitive();
	}
  // front
  if ((adj & 0x10u) == 0u) {
	  norm = vec3(0, 0, 1);
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.0), tph, norm);
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.0), tpd, norm);
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.0), tpi, norm);
	  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.0), tpe, norm);
	  EndPrimitive();
	}
}
