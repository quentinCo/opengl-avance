#include <glmlv/Image2DRGBA.hpp>

#include "Texture.hpp"

using namespace qc;


void Texture::initTexBuffer(PathFile pathFile)
{
	auto imageTex = glmlv::readImage(pathFile);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texPointer);
	glBindTexture(GL_TEXTURE_2D, texPointer);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, imageTex.width(), imageTex.height());
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageTex.width(), imageTex.height(), GL_RGBA, GL_UNSIGNED_BYTE, imageTex.data());
	glBindTexture(GL_TEXTURE_2D, 0);
}