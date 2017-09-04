#version 330 core

in vec4 FragPos;
in vec2 TexCoord;

uniform vec3 light_pos;
uniform float far_plane;
uniform sampler2D tex;

void main() {
  vec4 DiffuseColor = texture(tex, TexCoord);
  if (DiffuseColor.a < 0.4) discard;

	// get distance between fragment and light source
	float lightDistance = length(FragPos.xyz - light_pos);

	// map to [0;1] range by dividing by far_plane
	lightDistance = lightDistance / far_plane;

	// write this as modified depth
	gl_FragDepth = lightDistance;
}
