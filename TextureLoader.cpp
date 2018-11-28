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
		GL_TEXTURE_2D,	//! ָ���Ƕ�άͼƬ
		level,			//! ָ��Ϊ��һ�������������mipmap,��lod,����ľͲ��ü����ģ�Զ��ʹ�ý�С������
		GL_RGB,			//! �����ʹ�õĴ洢��ʽ
		width,			//! ��ȣ���һ����Կ�����֧�ֲ��������������Ⱥ͸߶Ȳ���2^n��
		height,			//! ��ȣ���һ����Կ�����֧�ֲ��������������Ⱥ͸߶Ȳ���2^n��
		border,			//! �Ƿ�ı�
		GL_BGR_EXT,		//! ���ݵĸ�ʽ��bmp�У�windows,����ϵͳ�д洢��������bgr��ʽ
		GL_UNSIGNED_BYTE, //! ������8bit����
		pixels
	);

	FreeImage_Unload(dib);

	return _textureId;
}
