#ifndef TextureLoader_H
#define TextureLoader_H

#include <windows.h>
#include <gl/gl.h>
#include "FreeImage.h"

class TextureLoader
{
public:
	TextureLoader() = default;
	~TextureLoader() = default;

	static GLuint LoadTexture(const char* filename,
		GLenum image_format = GL_RGB,
		GLint internal_format = GL_RGB,
		GLint level = 0,
		GLint border = 0);
};

#endif //TextureLoader_H