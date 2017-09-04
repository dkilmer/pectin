#version 330 core

smooth in vec3 FColor;
in vec2 TexCoord;
smooth in vec3 Intensity;
in vec4 WorldPos;

out vec4 outColor;

uniform sampler2D tex;

void main() {
  //RGBA of our diffuse color
	vec4 presetColor = vec4(FColor, 1.0);
  vec4 color = texture(tex, TexCoord);
  if (color.a < 0.4) discard;
  //outColor = vec4(FColor.rgb, DiffuseColor.a);
  //outColor = DiffuseColor;
	outColor = vec4( color.rgb * Intensity, color.a);
}
