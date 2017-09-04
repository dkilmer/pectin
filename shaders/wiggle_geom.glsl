#version 330 core

#define TWO_PI 6.2831853072

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
uniform float time;

void emit_point(mat3 rotation, vec3 pt, vec3 offset, vec2 tc) {
	vec4 pos = vec4((rotation * offset) + pt, 1.0);
	TexCoord = tc;
  gl_Position = vp * pos;
  WorldPos = pos;
  EmitVertex();
}

float oscillate(float phase, float speed) {
	//return -(cos((time+phase) * TWO_PI * (1.0/speed)) * 0.5) + 0.5;
	return -(cos((time+phase) * TWO_PI) * 0.5) + 0.5;
}

void main() {
	vec3 pt = gl_in[0].gl_Position.xyz;
	float color_change = Color[0].r;
	float speed = Color[0].g;
	float phase = Color[0].b;
	float x_scale_change = (ScaleRot[0].x * 0.2);
	float y_scale_change = (ScaleRot[0].y * 0.2);
	float osc = oscillate(phase, speed);
	float rot = ScaleRot[0].z;
	float scale_x = 0.501 + (osc * x_scale_change);
	float scale_y = 0.501 + (osc * y_scale_change);
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

  FColor = vec3(color_change);
  emit_point(rotation, pt, vec3(-scale_x, -scale_y, 0.0), vec2(zx, oy));
  emit_point(rotation, pt, vec3(-scale_x, scale_y, 0.0), vec2(zx, zy));
  emit_point(rotation, pt, vec3(scale_x, -scale_y, 0.0), vec2(ox, oy));
  emit_point(rotation, pt, vec3(scale_x, scale_y, 0.0), vec2(ox, zy));
  EndPrimitive();
}