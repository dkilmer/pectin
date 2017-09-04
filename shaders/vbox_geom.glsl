#version 330

layout(lines) in;
layout(line_strip, max_vertices = 5) out;

in vec3 Color[];

smooth out vec3 FColor;

uniform mat4 vp;

void main() {
	vec3 p0 = gl_in[0].gl_Position.xyz;
	vec3 p1 = gl_in[1].gl_Position.xyz;

	FColor = Color[0];

	gl_Position = vp * vec4(vec3(p0.x, p0.y, p0.z), 1.0);
  EmitVertex();

	gl_Position = vp * vec4(vec3(p0.x, p1.y, p0.z), 1.0);
  EmitVertex();

	FColor = Color[1];

	gl_Position = vp * vec4(vec3(p1.x, p1.y, p0.z), 1.0);
  EmitVertex();

	gl_Position = vp * vec4(vec3(p1.x, p0.y, p0.z), 1.0);
  EmitVertex();

	FColor = Color[0];

	gl_Position = vp * vec4(vec3(p0.x, p0.y, p0.z), 1.0);
  EmitVertex();
}
