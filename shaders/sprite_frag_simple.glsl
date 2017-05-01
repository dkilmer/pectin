#version 330 core

in vec3 FColor;
in vec2 TexCoord;
in vec4 WorldPos;

out vec4 outColor;

uniform sampler2D tex;

void main() {
  //RGBA of our diffuse color
  vec4 DiffuseColor = texture(tex, TexCoord);
  if (DiffuseColor.a < 0.5) discard;
	outColor = DiffuseColor;
	//outColor = vec4(1.0, 1.0, 1.0, 1.0);
}
