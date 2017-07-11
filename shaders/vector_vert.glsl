#version 330

in vec3 position;
in vec3 color;

smooth out vec3 FColor;

uniform mat4 vp;

void main() {
  FColor = color;
  gl_Position = vp * vec4(position, 1.0);
}
