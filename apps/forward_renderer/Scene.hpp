#pragma once

#include <tiny_obj_loader.h>

#include <vector>

#include "Mesh.hpp"
#include "Light.hpp"

namespace qc
{

class Scene
{
public:
	using PathFile = std::experimental::filesystem::path&;

	Scene() {};

	const std::vector<Mesh>& getMeshes() const
		{return meshes;}

	std::vector<Mesh>& getMeshes()
		{return meshes;}

	const std::vector<Light>& getDirectionalLights() const
		{return directionalLights;}

	std::vector<Light>& getDirectionalLights()
		{return directionalLights;}

	const std::vector<Light>& getPointLights() const
		{return pointLights;}

	std::vector<Light>& getPointLights()
		{return pointLights;}

	void addPointLight(const Light& light)
		{pointLights.push_back(light);}

	void addDirectionalLight(const Light& light)
		{directionalLights.push_back(light);}

	void addMesh(const Mesh& mesh)
		{meshes.push_back(mesh);}

	void addMeshFromObjFile(const PathFile directory, const std::string& nameFile, const glmlv::GLProgram& program, const glm::vec3& position = glm::vec3(0));

private:
	std::vector<Mesh> meshes;
	std::vector<Light> directionalLights;
	std::vector<Light> pointLights;

	// For the cube problem nb vertex != nb normal != nb texCoord
	std::pair<size_t,size_t> findMinMaxIndex(const std::vector<tinyobj::index_t>& indices, std::string& res);

};

} // namespace qc