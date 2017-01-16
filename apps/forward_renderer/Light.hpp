#pragma once 

#include <glmlv/GLProgram.hpp>
#include <glmlv/simple_geometry.hpp>

namespace qc
{

class Light
{
public:
	Light() {};
	Light(const glm::vec3& pos, float intensity)
		: lightPosition(pos), lightIntensity(intensity)
	{}

	const glm::vec3& getLightPosition() const
		{return lightPosition;}

	glm::vec3& getLightPosition()
		{return lightPosition;}

	float getLightIntensity() const
		{return lightIntensity;}

	float& getLightIntensity()
		{return lightIntensity;}


private:
	glm::vec3 lightPosition;
	float lightIntensity;
};

}