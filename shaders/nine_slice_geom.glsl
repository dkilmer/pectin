#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 Color[];
in vec3 ScaleRot[];
in uvec3 SprOffset[];

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


	vec2 tex_margin = tex_mult * 0.25;
  uint col = SprOffset[0].x % SprOffset[0].y;
  uint row = SprOffset[0].x / SprOffset[0].y;
  uint adj = SprOffset[0].z;

  // something to the left
  float zx_margin = ((adj & 0x0001u) == 0x0001u) ? 1.0 : 0.0;
  // something above
  float zy_margin = ((adj & 0x0002u) == 0x0002u) ? 1.0 : 0.0;
  // something to the right
  float ox_margin =  ((adj & 0x0004u) == 0x0004u) ? 1.0 : 0.0;
  // something below
  float oy_margin =  ((adj & 0x0008u) == 0x0008u) ? 1.0 : 0.0;

  float zx = float(col) * tex_mult.x;
  float zy = float(row) * tex_mult.y;
  float ox = zx + tex_mult.x;
  float oy = zy + tex_mult.y;
  zx = zx + (zx_margin * tex_margin.x);
  zy = zy + (zy_margin * tex_margin.y);
  ox = ox - (ox_margin * tex_margin.x);
  oy = oy - (oy_margin * tex_margin.y);
	vec4 pos;
	float mscale_x = scale_x;
	float mscale_y = scale_y;
	zx_margin = 1.0 - zx_margin;
	zy_margin = 1.0 - zy_margin;
	ox_margin = 1.0 - ox_margin;
	oy_margin = 1.0 - oy_margin;

  FColor = Color[0];
  emit_point(rotation, pt, vec3(-scale_x - (mscale_x*zx_margin), -(scale_y + (mscale_y*oy_margin)), 0.0), vec2(zx, oy));
  emit_point(rotation, pt, vec3(-scale_x - (mscale_x*zx_margin), scale_y + (mscale_y*zy_margin), 0.0), vec2(zx, zy));
  emit_point(rotation, pt, vec3(scale_x + (mscale_x*ox_margin), -(scale_y + (mscale_y*oy_margin)), 0.0), vec2(ox, oy));
  emit_point(rotation, pt, vec3(scale_x + (mscale_x*ox_margin), scale_y + (mscale_y*zy_margin), 0.0), vec2(ox, zy));
  EndPrimitive();
}