#version 330 core

#define MAX_LIGHTS 10

in vec3 FColor;
in vec2 TexCoord;
in vec4 ShadowCoord;
in vec3 WorldPos;
in vec3 Intensity;
in vec3 Normal;

out vec4 outColor;

uniform sampler2D tex;
uniform sampler2D shadow_map;
uniform int num_lights;
uniform struct Light {
	vec4 position;
	vec3 intensities; //a.k.a the color of the light
	float attenuation;
	float ambient_coefficient;
	float cone_angle;
	vec3 cone_direction;
} all_lights[MAX_LIGHTS];
uniform vec3 camera_pos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
  // perform perspective divide
  vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
  // transform to [0,1] range
  projCoords = projCoords * 0.5 + 0.5;
  // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
  float closestDepth = texture(shadow_map, projCoords.xy).r;
  // get depth of current fragment from light's perspective
  float currentDepth = projCoords.z;
  // check whether current frag pos is in shadow
  float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
  return shadow;
}

void main() {
  //RGBA of our diffuse color
//	float shadow = ShadowCalculation(ShadowCoord);
//  vec4 DiffuseColor = texture(tex, TexCoord) + (1.0-shadow);
//	vec3 gamma = vec3(1.0/2.2);
//	vec4 LitColor = vec4(pow(Intensity, gamma) * DiffuseColor.rgb, DiffuseColor.a);
//	float visibility = ShadowCalculation(ShadowCoord);
	//float bias = 0.005;
//	if ( texture(shadow_map, vec3(ShadowCoord.xy, (ShadowCoord.z)/ShadowCoord.w) )  <  ShadowCoord.z-bias) {
//	    visibility = 0.75;
//	}
	//outColor = vec4((LitColor).rgb + (1.0-shadow), LitColor.a);
	//outColor = vec4(vec3(shadow), 1.0);
	//float depthValue = texture(shadow_map, ShadowCoord.xy).r;
	//outColor = vec4(vec3(depthValue), 1.0);
	//outColor = vec4(vec3(texture(shadow_map, ShadowCoord.xyz )), 1.0);

    vec3 color = texture(tex, TexCoord).rgb;
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(0.6, 0.6, 0.6);
    // ambient
    vec3 ambient = 0.8 * color;
    // diffuse
    vec3 lightDir = normalize(all_lights[0].position.xyz - WorldPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(camera_pos - WorldPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;
    // calculate shadow

	  // perform perspective divide
	  vec3 projCoords = ShadowCoord.xyz / ShadowCoord.w;
	  // transform to [0,1] range
	  projCoords = projCoords * 0.5 + 0.5;
	  // get closest depth value from light's perspective (using [0,1] range ShadowCoord as coords)
	  float closestDepth = texture(shadow_map, projCoords.xy).r;
	  // get depth of current fragment from light's perspective
	  float currentDepth = projCoords.z;
	  // check whether current frag pos is in shadow
	  //float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
	  //float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	  float bias = 0.002;
		float shadow = currentDepth - bias > closestDepth  ? 0.7 : 0.0;

    //vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    vec3 lighting = ((ambient * FColor) + (1.0 - shadow) * (diffuse + specular)) * color;
    //vec3 lighting = ((ambient * FColor * 1.2) + (1.0 - shadow) * (diffuse + specular)) * color;
    //vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color + (FColor / 5.0);
    //vec3 lighting = (ambient * (diffuse + specular)) * color;

    outColor = vec4(lighting, 1.0);

}
