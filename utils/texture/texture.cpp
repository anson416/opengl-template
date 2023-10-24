#include "texture.h"

#include "GL/glew.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include <iostream>

void Texture::setupTexture(const char* texturePath)
{
	std::cout << "INF: Loading texture " << texturePath << "..." << std::endl;
	stbi_set_flip_vertically_on_load(true);  /* Tell stb_image.h to flip loaded texture's on the y-axis */
	unsigned char* data = stbi_load(texturePath, &_width, &_height, &_bpp, 0);  /* Load the texture data into "data" */
	GLenum format = 3;
	switch (_bpp) {
		case 1: format = GL_RED; break;
		case 3: format = GL_RGB; break;
		case 4: format = GL_RGBA; break;
	}

	glGenTextures(1, &_ID);
	glBindTexture(GL_TEXTURE_2D, _ID);

	/* Set texture wrapping parameters */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	/* Set texture filtering parameters */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
	else {
		std::cerr << "ERR: Failed to load " << texturePath << std::endl;
		exit(1);
	}

	// std::cout << "Load " << texturePath << " successfully!" << std::endl;
	glBindTexture(GL_TEXTURE_2D, 0);
}

/*
Loads a cubemap texture from 6 individual texture faces.
Order:
+X (right)
-X (left)
+Y (top)
-Y (bottom)
+Z (front) 
-Z (back)
*/
void Texture::setupTextureCubemap(const std::vector<std::string>& texPaths)
{
    glGenTextures(1, &_ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _ID);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	stbi_set_flip_vertically_on_load(false);  /* Tell stb_image.h to flip loaded texture's on the y-axis */
    for (unsigned int i = 0; i < texPaths.size(); i++) {
        std::cout << "INF: Loading cubemap " << texPaths[i] << "..." << std::endl;
        unsigned char *data = stbi_load(texPaths[i].c_str(), &_width, &_height, &_bpp, 0);
        GLenum format = 3;
	    switch (_bpp) {
		    case 1: format = GL_RED; break;
            case 3: format = GL_RGB; break;
            case 4: format = GL_RGBA; break;
	    }

        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, _width, _height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
        }
        else {
            std::cerr << "ERR: Failed to load " << texPaths[i] << std::endl;
            exit(1);
        }
    }

	// std::cout << "Load Cubemap successfully!" << std::endl;
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, _ID);
}

void Texture::bindCubemap(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _ID);
}

void Texture::unbind(void) const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::unbindCubemap(void) const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
