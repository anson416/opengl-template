#pragma once

#include <string>
#include <vector>

class Texture 
{
public:
	void setupTexture(const char* texturePath);
    void setupTextureCubemap(const std::vector<std::string>& texPaths);
	void bind(unsigned int slot) const;
	void bindCubemap(unsigned int slot) const;
	void unbind(void) const;
	void unbindCubemap(void) const;

private:
	unsigned int _ID;
	int _width, _height, _bpp;
};
