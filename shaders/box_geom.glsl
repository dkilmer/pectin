#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 20) out;

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
  gl_Position = vp * pos;
	TexCoord = tc;
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
	float px1 = 0.00390625;

  FColor = Color[0];
  // front
  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.0), tph);
  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.0), tpd);
  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.0), tpi);
  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.0), tpe);
  EndPrimitive();
  // top
  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.0), tpd);
  emit_point(rotation, pt, vec3(-scale_x, scale_y, -2.0 * scale_y), tpa);
  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.0), tpe);
  emit_point(rotation, pt, vec3(scale_x, scale_y, -2.0 * scale_y), tpb);
  EndPrimitive();
  // bottom
  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.0), tpi);
  emit_point(rotation, pt, vec3(scale_x, -scale_y, -2.0 * scale_y), tpl);
  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.0), tph);
  emit_point(rotation, pt, vec3(-scale_x, -scale_y, -2.0 * scale_y), tpk);
  EndPrimitive();
  // left
  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.0), tph);
  emit_point(rotation, pt, vec3(-scale_x, -scale_y, -2.0 * scale_y), tpg);
  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.0), tpc);
  emit_point(rotation, pt, vec3(-scale_x, scale_y, -2.0 * scale_y), tpd);
  EndPrimitive();
  // right
  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.0), tpe);
  emit_point(rotation, pt, vec3(scale_x, scale_y, -2.0 * scale_y), tpf);
  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.0), tpi);
  emit_point(rotation, pt, vec3(scale_x, -scale_y, -2.0 * scale_y), tpj);
  EndPrimitive();
}