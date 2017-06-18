#version 330 core

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;
in vec3 Color[];

smooth out vec3 FColor;

uniform mat4 vp;
uniform float scale;

void main() {
	vec3 g1 = gl_in[0].gl_Position.xyz;
	vec3 g2 = gl_in[1].gl_Position.xyz;
	vec3 v1 = normalize(g1 - g2) * scale;
	vec3 v2 = normalize(g2 - g1) * scale;

	FColor = Color[0]; //vec3(1.0, 0.0, 0.0);
  gl_Position = vp * vec4(-v2.y + g1.x, v2.x + g1.y, 0.0, 1.0);
  EmitVertex();

	FColor = Color[1]; //vec3(1.0, 0.0, 0.0);
  gl_Position = vp * vec4(v1.y + g2.x, -v1.x + g2.y, 0.0, 1.0);
  EmitVertex();

	FColor = Color[0]; //vec3(0.0, 0.0, 1.0);
  gl_Position = vp * vec4(v2.y + g1.x, -v2.x + g1.y, 0.0, 1.0);
  EmitVertex();

	FColor = Color[1]; //vec3(0.0, 0.0, 1.0);
  gl_Position = vp * vec4(-v1.y + g2.x, v1.x + g2.y, 0.0, 1.0);
  EmitVertex();

  EndPrimitive();
}
