#include "Texture.h"
#include "GLFW/glfw3.h"
#include <stb_image.h>
#include <iostream>

Texture::Texture(const std::string& InType, const std::string& InPath)
{
	type = InType;
	path = InPath;
}

Texture::~Texture()
{
	if (ID > 0)
	{
		glDeleteTextures(1, &ID);
		ID = 0;
	}

	if (ID_GammaCorrected > 0)
	{
		glDeleteTextures(1, &ID_GammaCorrected);
		ID_GammaCorrected = 0;
	}
}

void Texture::SetSRGB(bool bSRGB)
{
	sRGB = bSRGB;
}

void Texture::SetTextureWrapS(unsigned int S)
{
	textureWrapS = S;
}

void Texture::SetTextureWrapT(unsigned int T)
{
	textureWrapT = T;
}

void Texture::LoadFromFile(const char* path, const std::string& directory, bool gammaCorrection)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int& textureID = gammaCorrection ? ID_GammaCorrected : ID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = gammaCorrection ? GL_SRGB : GL_RGB;
		else if (nrComponents == 4)
			format = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
}

const std::string& Texture::GetType()
{
	return type;
}

unsigned int Texture::GetID()
{
	return ID;
}

unsigned int Texture::GetGammaCorrectedID()
{
	return ID_GammaCorrected;
}

unsigned int Texture::GetCorrectID() const
{
	return sRGB ? ID_GammaCorrected : ID;
}

const std::string& Texture::GetPath()
{
	return path;
}
