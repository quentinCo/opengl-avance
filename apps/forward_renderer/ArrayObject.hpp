#pragma once

#include <glmlv/GLProgram.hpp>

#include "BufferObject.hpp"

namespace qc
{
template<typename T>
using VertexBuffer = const std::shared_ptr<BufferObject<T>>&;
typedef const std::shared_ptr<BufferObject<uint32_t>>& IndexBuffer;

template<typename T>
class ArrayObject
{
public:
	ArrayObject() {}
	ArrayObject(VertexBuffer<T> vbo, IndexBuffer ibo, const glmlv::GLProgram& program)
	{
		initVao(vbo, ibo, program);
	}

	~ArrayObject()
	{
		if (vao) glDeleteBuffers(1, &vao);
	}

	GLuint getArrayPointer() const
		{return vao;}

private:
	GLuint vao = 0;

	void initVao(VertexBuffer<T> vbo, IndexBuffer ibo, const glmlv::GLProgram& program)
	{
		glGenVertexArrays(1, &vao);

		const GLint positionAttrLocation = glGetAttribLocation(program.glId(), "aPosition");
		const GLint normalAttrLocation = glGetAttribLocation(program.glId(), "aNormal");
		const GLint textAttrLocation = glGetAttribLocation(program.glId(), "aTexCoords");

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo->getBufferPointer());
		glEnableVertexAttribArray(positionAttrLocation);
		glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(T), (const GLvoid*)offsetof(T, position));

		if (normalAttrLocation > 0)
		{
			glEnableVertexAttribArray(normalAttrLocation);
			glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(T), (const GLvoid*)offsetof(T, normal));
		}

		if (textAttrLocation > 0)
		{
			glEnableVertexAttribArray(textAttrLocation);
			glVertexAttribPointer(textAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(T), (const GLvoid*)offsetof(T, texCoords));
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->getBufferPointer());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

};

}