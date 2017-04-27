#version 330 core

in vec3 FColor;
in vec2 TexCoord;
in vec4 WorldPos;

out vec4 outColor;

uniform vec2 Resolution;
uniform vec3 LightPos;
uniform vec4 LightColor;
uniform vec4 AmbientColor;
uniform vec3 Falloff;
uniform sampler2D tex;
uniform sampler2D norm_tex;

void main() {
  //RGBA of our diffuse color
  vec4 DiffuseColor = texture(tex, TexCoord);
  if (DiffuseColor.a < 0.5) discard;

  //RGB of our normal map
  vec3 NormalMap = texture(norm_tex, TexCoord).rgb;

  //The delta position of light
  //vec3 LightDir = vec3(LightPos.xy - (gl_FragCoord.xy / Resolution.xy), LightPos.z);

  //Correct for aspect ratio
  //LightDir.x *= Resolution.x / Resolution.y;
  vec3 LightDir = vec3(LightPos.xy - WorldPos.xy, LightPos.z);

  //Determine distance (used for attenuation) BEFORE we normalize our LightDir
  float D = length(LightDir);

  //normalize our vectors
  vec3 N = normalize(NormalMap * 2.0 - 1.0);
  vec3 L = normalize(LightDir);

  //Pre-multiply light color with intensity
  //Then perform "N dot L" to determine our diffuse term
  vec3 Diffuse = (LightColor.rgb * LightColor.a) * max(dot(N, L), 0.0);

  //pre-multiply ambient color with intensity
  vec3 Ambient = AmbientColor.rgb * AmbientColor.a;

  //calculate attenuation
  float Attenuation = 1.0 / ( Falloff.x + (Falloff.y*D) + (Falloff.z*D*D) );
	//float Attenuation = 1.0 / (D * Falloff.x);
  //the calculation which brings it all together
  vec3 Intensity = Ambient + Diffuse * Attenuation;
  vec3 FinalColor = DiffuseColor.rgb * Intensity;
  // this is the real output
  outColor = vec4(FinalColor, DiffuseColor.a);

  //outColor = vec4(gl_FragCoord.xyz/600.0, 1.0);
	//outColor = DiffuseColor;
}
