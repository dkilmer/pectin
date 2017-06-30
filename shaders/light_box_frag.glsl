#version 330 core

in vec3 FColor;
in vec2 TexCoord;
in vec3 Intensity;

out vec4 outColor;

uniform sampler2D tex;

void main() {
  //RGBA of our diffuse color
  vec4 DiffuseColor = texture(tex, TexCoord);
	vec3 gamma = vec3(1.0/2.2);
	outColor = vec4(pow(Intensity, gamma) * DiffuseColor.rgb, DiffuseColor.a);
}
