#version 330 core

in vec3 color;
in vec3 scale_rot;
in vec3 position;
in vec2 spr_offset;

out vec3 Color;
out vec3 ScaleRot;
out vec2 SprOffset;

uniform mat4 vp;

void main()
{
  Color = color;
  ScaleRot = scale_rot;
  SprOffset = spr_offset;
  gl_Position = vec4(position, 1.0);
}
