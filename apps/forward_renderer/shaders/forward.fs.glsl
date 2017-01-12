#version 330

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec3 vTexCoords;

//  Light
//   Directional
uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightIntensity;

//   Point
uniform vec3 uPointLightPosition;
uniform vec3 uPointLightIntensity;

//   Diffuse color
uniform vec3 uKd;

out vec3 fColor;

vec3 computeColor()
{
	//  Pointlight transitional varibles
	float distToPointLight = length(uPointLightPosition - vViewSpacePosition);
	vec3 dirToPointLight = (uPointLightPosition - vViewSpacePosition) / distToPointLight;
	vec3 pointLightIntensityToPoint = uPointLightIntensity * max(0.0, dot(vViewSpaceNormal, dirToPointLight));

	//  DirectionalLight transitional varibles
	vec3 directionalLightIntensityToPoint = uDirectionalLightIntensity * max(0.0, dot(vViewSpaceNormal, uDirectionalLightDir));

	vec3 lightIntensityToPoint = (pointLightIntensityToPoint + directionalLightIntensityToPoint) / (distToPointLight * distToPointLight);

	return (uKd * lightIntensityToPoint);
}

void main()
{
	fColor = computeColor();

	//fColor = vViewSpaceNormal;
}