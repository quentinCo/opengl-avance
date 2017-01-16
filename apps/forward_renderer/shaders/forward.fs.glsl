#version 330

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
//in vec3 vTexCoords;
in vec2 vTexCoords;

uniform mat4 uModelViewMatrix;

//  Light
//   Directional
uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightIntensity;

//   Point
uniform vec3 uPointLightPosition;
uniform vec3 uPointLightIntensity;

//  Diffuse color
uniform vec3 uKd;

//  Texture
uniform bool uActiveTexture;
uniform sampler2D uKdSampler;


out vec3 fColor;


vec3 computeLightIntensityToPoint()
{
	//  Pointlight transitional variables
	vec3 pointLightMV = (uModelViewMatrix * vec4(uPointLightPosition,1)).xyz;
	float distToPointLight = length(pointLightMV - vViewSpacePosition);
	vec3 dirToPointLight = (pointLightMV - vViewSpacePosition) / distToPointLight;
	vec3 pointLightIntensityToPoint = uPointLightIntensity * max(0.0, dot(vViewSpaceNormal, dirToPointLight));

	//  DirectionalLight transitional variables
	vec3 dirLightMV = (uModelViewMatrix * vec4(uDirectionalLightDir, 0)).xyz;
	vec3 directionalLightIntensityToPoint = uDirectionalLightIntensity * max(0.0, dot(vViewSpaceNormal, dirLightMV));

	vec3 lightIntensityToPoint = (pointLightIntensityToPoint + directionalLightIntensityToPoint) / (distToPointLight * distToPointLight);

	return lightIntensityToPoint;
}

void main()
{
	vec3 lightIntensityToPoint = computeLightIntensityToPoint();

	vec3 textureColor = vec3(1);
	if(uActiveTexture)
	{
		textureColor = texture(uKdSampler, vTexCoords).xyz;
	}

	fColor = uKd * textureColor * lightIntensityToPoint;
	
	//fColor = uKd * lightIntensityToPoint;
	//fColor = textureColor;
	//fColor = vec3(vTexCoords, 0);
	//fColor = vViewSpaceNormal;
}