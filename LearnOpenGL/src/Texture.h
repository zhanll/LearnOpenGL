#pragma once
#include <string>
#include "glad/glad.h"

class Texture
{
public:
	Texture(const std::string& InType, const std::string& InPath);
	~Texture();

	void SetSRGB(bool bSRGB);
	void SetTextureWrapS(unsigned int S);
	void SetTextureWrapT(unsigned int T);
	void LoadFromFile(bool gammaCorrection = false);
	const std::string& GetType();
	unsigned int GetID();
	unsigned int GetGammaCorrectedID();
	unsigned int GetCorrectID() const;
	const std::string& GetPath();

private:
	std::string type;
	std::string path;
	unsigned int ID = 0;
	unsigned int ID_GammaCorrected = 0;
	unsigned int textureWrapS = GL_REPEAT;
	unsigned int textureWrapT = GL_REPEAT;
	bool sRGB = false;
};