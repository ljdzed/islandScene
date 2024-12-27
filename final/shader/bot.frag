#version 330 core

in vec3 color;

// TODO: To add UV input to this fragment shader
in vec2 uv;

// TODO: To add the texture sampler
uniform bool useTexture;
uniform sampler2D textureSampler;


in vec3 worldPosition;
in vec3 worldNormal;

out vec3 finalColor;

uniform vec3 lightPosition;
uniform vec3 lightIntensity;
uniform mat4 lightSpaceMatrix;
uniform sampler2D depthMap;


void main()
{
	finalColor = color;

	finalColor =  vec3(texture(textureSampler, uv).rgb);

	// TODO: lighting, tone mapping, gamma correction

	vec3 lightDir = lightPosition - worldPosition;
	lightDir = normalize(lightDir);
	float dotprod = max(dot(lightDir, normalize(worldNormal)), 0.0);

	float intensity = 10 * dotprod;


	finalColor *= intensity * (lightIntensity / (4 * 3.14159 *  dot(lightDir, lightDir)));
	finalColor /= (finalColor+1);
	finalColor = pow(finalColor, vec3(1.0/2.2));

	vec4 lightSpace = lightSpaceMatrix * vec4(worldPosition, 1.0f);
	vec3 ndcSpace = (lightSpace.xyz/lightSpace.w) * 0.5 + 0.5;



	float existingDepth = texture(depthMap, ndcSpace.xy).r; // radiance
	float depth  = ndcSpace.z; // radiance


	float shadow = (depth >= existingDepth + 1e-3) ? 0.2 : 1.0;
	if (abs(ndcSpace.x) <= 1 && abs(ndcSpace.y) <= 1 && ndcSpace.z >= 0 && ndcSpace.z <= 1){ // in bound and valid r
		finalColor *= shadow;
	}

}


