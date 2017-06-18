#version 330 core

in vec3 FColor;
in vec2 TexCoord;
in vec2 LineTexCoord;
in vec4 WorldPos;

out vec4 outColor;

uniform sampler2D tex;

void main() {
  //RGBA of our diffuse color
  vec4 DiffuseColor = texture(tex, TexCoord);
  vec4 LineDiffuseColor = texture(tex, LineTexCoord);
  LineDiffuseColor.rgb = (1.0-LineDiffuseColor.rgb) * FColor;
	//outColor = DiffuseColor; // - FColor.r;
	//outColor = vec4(1.0, 1.0, 1.0, 1.0);
	// outputRed = (foregroundRed * foregroundAlpha) + (backgroundRed * (1.0 - foregroundAlpha));
	vec4 ocolor = DiffuseColor; //(LineDiffuseColor * LineDiffuseColor.a) + (DiffuseColor * (1.0 - LineDiffuseColor.a));
  if (ocolor.a < 0.4) discard;
  outColor = vec4(ocolor.rgb * (1 + (WorldPos.z / 10)), ocolor.a);
}
