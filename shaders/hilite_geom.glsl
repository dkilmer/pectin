#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 20) out;

in vec3 Color[];
in vec3 ScaleRot[];
in uvec3 SprOffset[];

smooth out vec3 FColor;
out vec2 TexCoord;

uniform mat4 vp;
uniform vec2 tex_mult;

void emit_point(mat3 rotation, vec3 pt, vec3 offset, vec2 tc) {
	vec4 pos = vec4((rotation * offset) + pt, 1.0);
  gl_Position = vp * pos;
	TexCoord = tc;
  EmitVertex();
}

void main() {
	vec3 pt = gl_in[0].gl_Position.xyz;
	float rot = ScaleRot[0].z;
	float scale_x = ScaleRot[0].x * 1.001;
	float scale_y = ScaleRot[0].y * 1.001;
	float scale_z = 1.001;
	mat3 rotation = mat3(
    vec3( cos(rot),  sin(rot),  0.0),
    vec3(-sin(rot),  cos(rot),  0.0),
    vec3(0.0,        0.0,       1.0)
  );

  uint col = SprOffset[0].x % SprOffset[0].y;
  uint row = SprOffset[0].x / SprOffset[0].y;
  uint face = SprOffset[0].z;
  float zx = tex_mult.x * float(col);
  float zy = tex_mult.y * float(row);

  vec2 tpa = vec2(zx, zy);
  vec2 tpb = vec2(zx+1.0, zy);
  vec2 tpc = vec2(zx, zy+1.0);
  vec2 tpd = vec2(zx+1.0, zy+1.0);

	FColor = Color[0];

  // bottom
  if (face == 4u) {
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.0), tpc);
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, -scale_z), tpa);
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.0), tpd);
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, -scale_z), tpb);
	  EndPrimitive();
  }
  // left
  if (face == 1u) {
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.0), tpc);
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, -scale_z), tpa);
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.0), tpd);
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, -scale_z), tpb);
	  EndPrimitive();
	}
  // right
  if (face == 3u) {
	  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.0), tpc);
	  emit_point(rotation, pt, vec3(scale_x, scale_y, -scale_z), tpa);
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.0), tpd);
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, -scale_z), tpb);
	  EndPrimitive();
	}
  // top
  if (face == 2u) {
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.0), tpc);
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, -scale_z), tpa);
	  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.0), tpd);
	  emit_point(rotation, pt, vec3(scale_x, scale_y, -scale_z), tpb);
	  EndPrimitive();
	}
  // front
  if (face == 0u) {
	  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.01), tpc);
	  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.01), tpa);
	  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.01), tpd);
	  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.01), tpb);
	  EndPrimitive();
	}
}
