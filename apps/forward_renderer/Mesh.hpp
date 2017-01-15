#pragma once

#include <vector>
#include <memory>

// glm
#include <glm/glm.hpp>

// qc
#include "BufferObject.hpp"
#include "ArrayObject.hpp"
#include "Texture.hpp"

namespace glmlv //TO_SEE
{
	struct Vertex3f3f2f;
	struct SimpleGeometry;
}

namespace qc
{

class Mesh
{
public:
	using VertexBuffer = std::shared_ptr<qc::BufferObject<glmlv::Vertex3f3f2f>>;
	using IndexBuffer = std::shared_ptr<qc::BufferObject<uint32_t>>;
	using ArrayObject = std::shared_ptr<qc::ArrayObject<glmlv::Vertex3f3f2f>>;
	using PathFile = const std::experimental::filesystem::path&;

	Mesh() {};
	Mesh(const glmlv::SimpleGeometry& geometry, const glmlv::GLProgram& program, const glm::vec3& position = glm::vec3(0));

	glm::mat4 getModelMatrix()
		{return modelMatrix;}

	const glm::mat4& getModelMatrix() const
		{return modelMatrix;}

	void setModelMatrix(const glm::mat4& modelMatrix)
		{this->modelMatrix = modelMatrix;}

	const glm::vec3& getDiffuseColor() const
		{return diffuseColor;}

	glm::vec3& getDiffuseColor()
		{return diffuseColor;}

	const std::shared_ptr<Texture> getTexture() const
		{return texture;}

	void setTexture(PathFile pathFile)
		{texture = std::make_shared<Texture>(pathFile);}

	void drawMesh() const;

private:
	VertexBuffer vbo;
	IndexBuffer ibo;
	ArrayObject vao;

	glm::mat4 modelMatrix = glm::mat4(1.f);

	glm::vec3 diffuseColor = glm::vec3(1);

	std::shared_ptr<Texture> texture;

	void initBuffers(const std::vector<glmlv::Vertex3f3f2f>& vertexBuffer, const std::vector<uint32_t>& indexBuffer, const glmlv::GLProgram& program);

};

}