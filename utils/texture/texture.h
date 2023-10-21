#pragma once

#include <string>
#include <vector>

class Texture 
{
public:
	void setupTexture(const char* texturePath);
    void setupTextureCubemap(const std::vector<std::string>& texPaths);
	void bind(unsigned int slot) const;
	void unbind() const;

private:
	unsigned int _ID;
	int _width, _height, _bpp;
};
