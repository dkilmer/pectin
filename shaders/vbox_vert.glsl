#version 330

in vec3 position;
in vec3 color;

smooth out vec3 Color;

void main() {
  Color = color;
  gl_Position = vec4(position, 1.0);
}
