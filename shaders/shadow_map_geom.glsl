#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 100) out;

in vec3 Color[];
in vec3 ScaleRot[];
in uvec3 SprOffset[];

out vec4 FragPos;

uniform mat4 depth_vps[6];

void emit_point(int face, mat3 rotation, vec3 pt, vec3 offset) {
	if (face != 4) {
	gl_Layer = face;
	FragPos = vec4((rotation * offset) + pt, 1.0);
  gl_Position = depth_vps[face] * FragPos;
  EmitVertex();
	}
}

void main() {
	vec3 pt = gl_in[0].gl_Position.xyz;
	float rot = ScaleRot[0].z;
	float scale_x = ScaleRot[0].x;
	float scale_y = ScaleRot[0].y;
	float scale_z = 1.0;
	mat3 rotation = mat3(
    vec3( cos(rot),  sin(rot),  0.0),
    vec3(-sin(rot),  cos(rot),  0.0),
    vec3(0.0,        0.0,       1.0)
  );
  uint adj = SprOffset[0].z;
	vec3 norm;

	for (int face=0; face<6; face++) {
	  // bottom
	  if ((adj & 0x400u) == 0u) {
		  norm = vec3(0, -1, 0);
		  emit_point(face, rotation, pt, vec3(scale_x, -scale_y, 0.0));
		  emit_point(face, rotation, pt, vec3(scale_x, -scale_y, -scale_z));
		  emit_point(face, rotation, pt, vec3(-scale_x, -scale_y, 0.0));
		  emit_point(face, rotation, pt, vec3(-scale_x, -scale_y, -scale_z));
		  EndPrimitive();
	  }
	  // left
	  if ((adj & 0x1000u) == 0u) {
		  norm = vec3(-1, 0, 0);
		  emit_point(face, rotation, pt, vec3(-scale_x, -scale_y, 0.0));
		  emit_point(face, rotation, pt, vec3(-scale_x, -scale_y, -scale_z));
		  emit_point(face, rotation, pt, vec3(-scale_x, scale_y, 0.0));
		  emit_point(face, rotation, pt, vec3(-scale_x, scale_y, -scale_z));
		  EndPrimitive();
		}
	  // right
	  if ((adj & 0x4000u) == 0u) {
		  norm = vec3(1, 0, 0);
		  emit_point(face, rotation, pt, vec3(scale_x, scale_y, 0.0));
		  emit_point(face, rotation, pt, vec3(scale_x, scale_y, -scale_z));
		  emit_point(face, rotation, pt, vec3(scale_x, -scale_y, 0.0));
		  emit_point(face, rotation, pt, vec3(scale_x, -scale_y, -scale_z));
		  EndPrimitive();
		}
	  // top
	  if ((adj & 0x10000u) == 0u) {
		  norm = vec3(0, 1, 0);
		  emit_point(face, rotation, pt, vec3(-scale_x, scale_y, 0.0));
		  emit_point(face, rotation, pt, vec3(-scale_x, scale_y, -scale_z));
		  emit_point(face, rotation, pt, vec3(scale_x, scale_y, 0.0));
		  emit_point(face, rotation, pt, vec3(scale_x, scale_y, -scale_z));
		  EndPrimitive();
		}
	  // front
	  if ((adj & 0x10u) == 0u) {
		  norm = vec3(0, 0, 1);
		  emit_point(face, rotation, pt, vec3(-scale_x, -scale_y, 0.0));
		  emit_point(face, rotation, pt, vec3(-scale_x, scale_y, 0.0));
		  emit_point(face, rotation, pt, vec3(scale_x, -scale_y, 0.0));
		  emit_point(face, rotation, pt, vec3(scale_x, scale_y, 0.0));
		  EndPrimitive();
		}

	}


}
