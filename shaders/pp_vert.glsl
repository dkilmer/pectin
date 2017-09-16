#version 330 core

in vec3 position;
out vec2 TexCoord;

uniform mat4 vp;

void main()
{
	TexCoord = position.xy;
  gl_Position = vp * vec4(position, 1.0);
}
