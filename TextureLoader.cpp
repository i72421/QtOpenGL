#include "TextureLoader.h"

GLuint TextureLoader::LoadTexture(const char* filename, GLenum image_format, GLint internal_format, GLint level, GLint border)
{
	FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(filename, 0);
	FIBITMAP *dib = FreeImage_Load(fifmt, filename, 0);

	dib = FreeImage_ConvertTo24Bits(dib);
	BYTE *pixels = (BYTE*)FreeImage_GetBits(dib);
	int width = FreeImage_GetWidth(dib);
	int height = FreeImage_GetHeight(dib);

	GLuint _textureId;
	glGenTextures(1, &_textureId);
	glBindTexture(GL_TEXTURE_2D, _textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(
		GL_TEXTURE_2D,	//! 指定是二维图片
		level,			//! 指定为第一级别，纹理可以做mipmap,即lod,离近的就采用级别大的，远则使用较小的纹理
		GL_RGB,			//! 纹理的使用的存储格式
		width,			//! 宽度，老一点的显卡，不支持不规则的纹理，即宽度和高度不是2^n。
		height,			//! 宽度，老一点的显卡，不支持不规则的纹理，即宽度和高度不是2^n。
		border,			//! 是否的边
		GL_BGR_EXT,		//! 数据的格式，bmp中，windows,操作系统中存储的数据是bgr格式
		GL_UNSIGNED_BYTE, //! 数据是8bit数据
		pixels
	);

	FreeImage_Unload(dib);

	return _textureId;
}
