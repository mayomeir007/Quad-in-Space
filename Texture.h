#pragma once

#include <string>
#include <SDL_image.h>
#include "gl.h"

class Texture
{

public:

	Texture();

	void Bind();
	void Load(const std::string& filename);
	void Unbind();
	void Unload();
	void Reload();

	void SaveImage(const std::string& filename);
	void SaveImageWithEffects(const std::string& filename);

	void Invert();
	void Blur(GLfloat blurFactor, bool isInvert);

private:
	Uint8* HorizontalBlur(GLsizei radius, GLfloat sigma);
	void VerticalBlur(Uint8* tempPixels, GLsizei radius, GLfloat sigma);
	const char* GetExtension(const char* filename);

	SDL_Surface* m_textureData; //includes  pixels of loaded image without the current effects applied on it
	Uint8* m_pixelsWithEffects = nullptr; //pixels of loaded image WITH the current effects applied on it 
	GLuint m_ID;

};