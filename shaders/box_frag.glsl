#version 330 core

#define MAX_LIGHTS 10

in vec3 FColor;
in vec2 TexCoord;
in vec3 WorldPos;
in vec3 Normal;

out vec4 outColor;

uniform int num_lights;
uniform struct Light {
	vec4 position;
	vec3 intensities; //a.k.a the color of the light
	float attenuation;
	float ambient_coefficient;
	float cone_angle;
	vec3 cone_direction;
} all_lights[MAX_LIGHTS];
uniform sampler2D tex;
uniform vec3 camera_pos;
uniform float shininess;
uniform vec3 spec_color;

vec3 ApplyLight(Light light, vec3 normal, vec3 surfacePos, vec3 surfaceToCamera) {
	vec3 surfaceToLight;
	float attenuation = 1.0;
	if(light.position.w == 0.0) {
		//directional light
		surfaceToLight = normalize(light.position.xyz);
		attenuation = 1.0; //no attenuation for directional lights
	} else {
		//point light
		surfaceToLight = normalize(light.position.xyz - surfacePos);
		float distanceToLight = length(light.position.xyz - surfacePos);
		attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));

		//cone restrictions (affects attenuation)
		float lightToSurfaceAngle = degrees(acos(dot(-surfaceToLight, normalize(light.cone_direction))));
		if(lightToSurfaceAngle > light.cone_angle){
			attenuation = 0.0;
		}
	}

	//ambient
	//vec3 ambient = light.ambient_coefficient * surfaceColor * light.light_color;
	vec3 ambient = light.ambient_coefficient * light.intensities;

	//diffuse
	float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
	//vec3 diffuse = diffuseCoefficient * surfaceColor * light.light_color;
	vec3 diffuse = diffuseCoefficient * light.intensities;

	//specular
	float specularCoefficient = 0.0;
	if(diffuseCoefficient > 0.0)
		specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), shininess);
	vec3 specular = specularCoefficient * spec_color * light.intensities;

	//linear color (color before gamma correction)
	return ambient + attenuation*(diffuse + specular);
}

void main() {
	mat4 model = mat4(1.0);
	//vec3 normal = normalize(transpose(inverse(mat3(model))) * Normal);
	//vec3 normal = normalize(transpose(inverse(mat3(1.0))) * Normal);
	vec3 normal = Normal;
	//vec3 surfacePos = vec3(model * vec4(WorldPos, 1));
	vec3 surfacePos = WorldPos;
	vec4 surfaceColor = texture(tex, TexCoord);
	vec3 surfaceToCamera = normalize(camera_pos - surfacePos);

	//combine color from all the lights
	vec3 linearColor = vec3(0);
	for(int i = 0; i < num_lights; ++i){
		linearColor += ApplyLight(all_lights[i], normal, surfacePos, surfaceToCamera);
	}
	//linearColor = linearColor * surfaceColor.rgb;

	//final color (after gamma correction)
	//vec3 gamma = vec3(1.0/2.2);
	vec3 gamma = vec3(1.0/2.2);
	outColor = vec4(pow(linearColor, gamma) * surfaceColor.rgb, surfaceColor.a);
	//outColor = vec4(linearColor, surfaceColor.a);
}
