#pragma once
//#include <experimental/filesystem>

#include <glmlv/filesystem.hpp>
#include <glmlv/GLProgram.hpp>

namespace qc
{

class Texture
{
public:
	using PathFile = const glmlv::fs::path&;
	Texture() {}
	Texture(PathFile pathFile)
		{initTexBuffer(pathFile);};

	~Texture()
	{
		if (texPointer) glDeleteTextures(1, &texPointer);
	}

	Texture(const Texture&) = delete;
	Texture& operator= (const Texture&) = delete;

	Texture(Texture&& o)
		: texPointer(o.texPointer)
	{
		o.texPointer = 0;
	}

	Texture& operator= (Texture&& o)
	{
		texPointer = o.texPointer;
		o.texPointer = 0;

		return *this;
	}


	GLuint getTexPointer() const
		{return texPointer;}

private:
	GLuint texPointer = 0;

	void initTexBuffer(PathFile pathFile);
};

}