#pragma once

#include <glmlv/ViewController.hpp>

namespace qc
{

class Camera
{

public:
	Camera(GLFWwindow* window, float fovy, float aspectRatio, float nearPlan = 0.1, float farPlan = 100.f, float speed = 1.f)
		: viewController(window, speed),
		projMatrix(glm::perspective(fovy, aspectRatio, nearPlan, farPlan))
	{}

	const glm::mat4& getViewMatrix() const
		{return viewController.getViewMatrix();}

	const glm::mat4& getProjMatrix() const
		{return projMatrix;}

	glm::mat4 computeModelViewMatrix(const glm::mat4& modelMatrix)
		{return getViewMatrix() * modelMatrix;}

	glm::mat4 computeModelViewProjMatrix(const glm::mat4& modelMatrix)
		{return projMatrix * computeModelViewMatrix(modelMatrix);}

	glm::mat4 computeNormalMatrix(const glm::mat4& modelMatrix)
		{return glm::transpose(glm::inverse(computeModelViewMatrix(modelMatrix)));}

	void updateViewController(float ellapsedTime)
		{viewController.update(ellapsedTime);}

private:
	glm::mat4 projMatrix;
	glmlv::ViewController viewController;
};

} // namespace qc