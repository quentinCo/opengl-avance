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
	using VertexBuffer = qc::BufferObject<glmlv::Vertex3f3f2f>;
	using IndexBuffer = qc::BufferObject<uint32_t>;
	using ArrayObject = qc::ArrayObject<glmlv::Vertex3f3f2f>;
	using PathFile = const glmlv::fs::path&;

	Mesh() {};
	Mesh(const std::vector<glmlv::Vertex3f3f2f>& vbo, const std::vector<uint32_t>& ibo, const glmlv::GLProgram& program, const glm::vec3& position = glm::vec3(0));
	Mesh(const glmlv::SimpleGeometry& geometry, const glmlv::GLProgram& program, const glm::vec3& position = glm::vec3(0), const PathFile& pathFile = "");
/*
	Mesh(const Mesh&o )	
	Mesh& operator= (const Mesh& o)
	{

	};
*/
	Mesh(Mesh&& o) = default;
	Mesh& operator = (Mesh&& o) = default;

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

	void setDiffuseColor(const glm::vec3& color)
		{diffuseColor = color;}

	const Texture& getDiffuseTexture() const
		{return diffuseTexture;}

	void setDiffuseTexture(PathFile pathFile)
		{diffuseTexture = Texture(pathFile);}

	const Texture& getNormalTexture() const
		{return normalTexture;}

	void setNormalTexture(PathFile pathFile)
		{normalTexture = Texture(pathFile);}

	void drawMesh() const;

private:
	VertexBuffer vbo;
	IndexBuffer ibo;
	ArrayObject vao;

	glm::mat4 modelMatrix = glm::mat4(1.f);

	glm::vec3 diffuseColor = glm::vec3(1);

	Texture diffuseTexture;
	Texture normalTexture;

	void initBuffers(const std::vector<glmlv::Vertex3f3f2f>& vertexBuffer, const std::vector<uint32_t>& indexBuffer, const glmlv::GLProgram& program);

};

}