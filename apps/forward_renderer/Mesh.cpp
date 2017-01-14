#include <glmlv/simple_geometry.hpp>

#include "Mesh.hpp"

using namespace qc;

Mesh::Mesh(const glmlv::SimpleGeometry& geometry, const glmlv::GLProgram& program, const glm::vec3& position)
{
	modelMatrix = glm::translate(modelMatrix, position);
	initBuffers(geometry.vertexBuffer, geometry.indexBuffer, program);
}

void Mesh::drawMesh() const
{
	glBindVertexArray(vao->getArrayPointer());
	glDrawElements(GL_TRIANGLES, ibo->getSizeBuffer(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void Mesh::initBuffers(const std::vector<glmlv::Vertex3f3f2f>& vertexBuffer, const std::vector<uint32_t>& indexBuffer, const glmlv::GLProgram& program)
{
	// VBO
	vbo = std::make_shared<qc::BufferObject<glmlv::Vertex3f3f2f>>(GL_ARRAY_BUFFER, vertexBuffer);
	// IBO
	ibo = std::make_shared<qc::BufferObject<uint32_t>>(GL_ARRAY_BUFFER, indexBuffer);
	// VAO
	vao = std::make_shared<qc::ArrayObject<glmlv::Vertex3f3f2f>>(vbo, ibo, program);
}
