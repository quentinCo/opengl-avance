#pragma once

#include <glmlv/GLProgram.hpp>

namespace qc
{

template<typename T>

class BufferObject
{
public:
	BufferObject() {};

	BufferObject(const GLenum target, const std::vector<T>& data)
		: target(target),
		bufferSize(data.size())
	{
		initBuffer(data);
	}

	~BufferObject()
	{
		if (bufferPointer) glDeleteBuffers(1, &bufferPointer);
	}

	BufferObject(const BufferObject<T>&) = delete;
	BufferObject<T>& operator= (const BufferObject<T>&) = delete;

	BufferObject(BufferObject&& o)
		: bufferPointer(o.bufferPointer), target(o.target), bufferSize(o.bufferSize)
	{
		o.bufferPointer = 0;
	}

	BufferObject<T>& operator= (BufferObject<T>&& o)
	{
		bufferPointer = o.bufferPointer;
		target = o.target;
		bufferSize = o.bufferSize;
		o.bufferPointer = 0;

		return *this;
	}

	GLuint getBufferPointer() const
		{return bufferPointer;}

	size_t getSizeBuffer() const
		{return bufferSize;}

private:
	GLuint bufferPointer = 0;
	GLenum target;
	size_t bufferSize = 0;

	void initBuffer(const std::vector<T>& data)
	{
		glGenBuffers(1, &bufferPointer);
		glBindBuffer(GL_ARRAY_BUFFER, bufferPointer);
		glBufferStorage(GL_ARRAY_BUFFER, bufferSize * sizeof(T), data.data(), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
};	

}