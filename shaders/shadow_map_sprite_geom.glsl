#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 20) out;

in vec3 Color[];
in vec3 ScaleRot[];
in uvec3 SprOffset[];

out vec4 FragPos;
out vec2 TexCoord;

uniform mat4 depth_vps[6];
uniform vec2 tex_mult;

void emit_point(int face, mat3 rotation, vec3 pt, vec3 offset, vec2 tc) {
	if (face != 4) {
	gl_Layer = face;
	FragPos = vec4((rotation * offset) + pt, 1.0);
	TexCoord = tc;
  gl_Position = depth_vps[face] * FragPos;
  EmitVertex();
	}
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

	for (int face=0; face<6; face++) {
	  emit_point(face, rotation, pt, vec3(-scale_x, -scale_y, 0.0), vec2(zx, oy));
	  emit_point(face, rotation, pt, vec3(-scale_x, scale_y, 0.0), vec2(zx, zy));
	  emit_point(face, rotation, pt, vec3(scale_x, -scale_y, 0.0), vec2(ox, oy));
	  emit_point(face, rotation, pt, vec3(scale_x, scale_y, 0.0), vec2(ox, zy));
	  EndPrimitive();
  }
}