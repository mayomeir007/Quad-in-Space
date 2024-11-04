#include <iostream>
#include <vector>
#include <glm.hpp>

#include "Texture.h"

Texture::Texture()
{
	m_textureData = nullptr;
	m_ID = 0;
}

void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture::Load(const std::string& filename)
{
	m_textureData = IMG_Load(filename.c_str());

	if (!m_textureData)
	{
		std::cout << "Error loading texture." << std::endl;
		return;
	}

	GLsizei width = m_textureData->w;
	GLsizei height = m_textureData->h;
	Uint8* pixels = (Uint8*)m_textureData->pixels;
	Uint8 depth = m_textureData->format->BytesPerPixel;
	GLint format = ((depth == 4) ? GL_RGBA : GL_RGB);

	glGenTextures(1, &m_ID);

	glBindTexture(GL_TEXTURE_2D, m_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	m_pixelsWithEffects = new Uint8[width * height * depth]();
	std::copy_n(pixels, width * height * depth, m_pixelsWithEffects);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Reload()
{
	GLsizei width = m_textureData->w;
	GLsizei height = m_textureData->h;
	Uint8* pixels = m_pixelsWithEffects;
	Uint8 depth = m_textureData->format->BytesPerPixel;
	GLint format = ((depth == 4) ? GL_RGBA : GL_RGB);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Unload()
{
	delete[] m_pixelsWithEffects;
	SDL_FreeSurface(m_textureData);
	glDeleteTextures(1, &m_ID);
}

/// <summary>
/// saves the texture pixels without the current effects applied on it to an image file 
/// </summary>
/// <param name="filename">save path</param>
void Texture::SaveImage(const std::string& filename)
{
	std::string extension = GetExtension(filename.c_str());

	if (extension == "jpg")
	{
		IMG_SaveJPG(m_textureData, filename.c_str(), 100);
	}
	else
	{
		IMG_SavePNG(m_textureData, filename.c_str());
	}
}

/// <summary>
/// saves the texture pixels WITH the current effects applied on it, namely m_pixelsWithEffects, to an image file 
/// </summary>
/// <param name="filename">save path</param>
void Texture::SaveImageWithEffects(const std::string& filename)
{
	std::string extension = GetExtension(filename.c_str());

	GLsizei width = m_textureData->w;
	GLsizei height = m_textureData->h;
	Uint8 depth = m_textureData->format->BytesPerPixel;
	int pitch = m_textureData->pitch;
	Uint32 format = m_textureData->format->format;

	SDL_Surface* pixelData = SDL_CreateRGBSurfaceWithFormatFrom(m_pixelsWithEffects, width, height, depth, pitch, format);

	if (extension == "jpg")
	{
		IMG_SaveJPG(pixelData, filename.c_str(), 100);
	}
	else
	{
		IMG_SavePNG(pixelData, filename.c_str());
	}

	SDL_FreeSurface(pixelData);

}

void Texture::Invert()
{
	Uint8 depth = m_textureData->format->BytesPerPixel;

	if (m_pixelsWithEffects != nullptr)
	{
		GLsizei nbytes = m_textureData->w * m_textureData->h * m_textureData->format->BytesPerPixel;

		for (int i = 0; i < nbytes; i++)
		{
			if (depth < 4 || (i + 1) % 4 != 0)
			{
				m_pixelsWithEffects[i] = 255 - m_pixelsWithEffects[i];
			}
		}
	}
}


void Texture::Blur(GLfloat blurFactor, bool isInvert)
{
	GLsizei bradiusHori = GLsizei(blurFactor * m_textureData->w / 2);
	GLsizei bradiusVerti = GLsizei(blurFactor * m_textureData->h / 2);

	if (bradiusHori == 0 || bradiusVerti == 0)
	{
		GLsizei nbytes = m_textureData->w * m_textureData->h * m_textureData->format->BytesPerPixel;
		std::copy_n((Uint8*)m_textureData->pixels, nbytes, m_pixelsWithEffects);
	}
	else
	{
		HorizontalBlur(bradiusHori,bradiusHori*.3f);
		VerticalBlur(bradiusVerti, bradiusVerti * .3f);
	}

	if (isInvert)
	{
		Invert();
	}
}


void Texture::HorizontalBlur(GLsizei radius, GLfloat sigma)
{
	// Create the horizontal Gaussian kernel
	int kernelSize = 2 * radius + 1;
	std::vector<double> kernel(kernelSize, 0.0);
	double sum = 0.0;

	for (int i = -radius; i <= radius; ++i) {
		double exponent = -(i * i) / (2 * sigma * sigma);
		double value = std::exp(exponent) / (sqrt(2 * M_PI) * sigma);
		kernel[i + radius] = value;
		sum += value;
	}

	// Normalize the kernel
	for (int i = 0; i < kernelSize; ++i) {
		kernel[i] /= sum;
	}

	Uint8 depth = m_textureData->format->BytesPerPixel;
	GLsizei width = m_textureData->w;
	const Uint8* pixels = (Uint8*)m_textureData->pixels;

	// Apply the horizontal blur pass
	for (int i = 0; i < m_textureData->h; ++i) {
		for (int j = 0; j < width; ++j) {
			m_pixelsWithEffects[i * width * depth + j * depth] = 0; //Red channel
			m_pixelsWithEffects[i * width * depth + j * depth + 1] = 0; //green channel
			m_pixelsWithEffects[i * width * depth + j * depth + 2] = 0; //blue channel

			for (int k = -radius; k <= radius; ++k) {
				if (j + k >= 0 && j + k < width)
				{
					m_pixelsWithEffects[i * width * depth + j * depth] += Uint8(kernel[k + radius] * pixels[i * width * depth + (j + k) * depth]);
					m_pixelsWithEffects[i * width * depth + j * depth + 1] += Uint8(kernel[k + radius] * pixels[i * width * depth + (j + k) * depth + 1]);
					m_pixelsWithEffects[i * width * depth + j * depth + 2] += Uint8(kernel[k + radius] * pixels[i * width * depth + (j + k) * depth + 2]);
				}
			}
		}
	}
}

void Texture::VerticalBlur(GLsizei radius, GLfloat sigma)
{
	// Create the vertical Gaussian kernel
	int kernelSize = 2 * radius + 1;
	std::vector<double> kernel(kernelSize, 0.0);
	double sum = 0.0;

	for (int i = -radius; i <= radius; ++i) {
		double exponent = -(i * i) / (2 * sigma * sigma);
		double value = std::exp(exponent) / (sqrt(2 * M_PI) * sigma);
		kernel[i + radius] = value;
		sum += value;
	}

	// Normalize the kernel
	for (int i = 0; i < kernelSize; ++i) {
		kernel[i] /= sum;
	}

	Uint8 depth = m_textureData->format->BytesPerPixel;

	// Apply the vertical blur pass
	for (int i = 0; i < m_textureData->h; ++i) {
		for (int j = 0; j < m_textureData->w; ++j) {

			glm::vec3 RGBvalue(0.0f);
			for (int k = -radius; k <= radius; ++k) {
				if (i + k >= 0 && i + k < m_textureData->h)
				{
					RGBvalue.x += float(kernel[k + radius] * m_pixelsWithEffects[(i + k) * m_textureData->w * depth + j * depth]);
					RGBvalue.y += float(kernel[k + radius] * m_pixelsWithEffects[(i + k) * m_textureData->w * depth + j * depth + 1]);
					RGBvalue.z += float(kernel[k + radius] * m_pixelsWithEffects[(i + k) * m_textureData->w * depth + j * depth + 2]);
				}
			}
			m_pixelsWithEffects[i * m_textureData->w * depth + j * depth] = Uint8(RGBvalue.x);
			m_pixelsWithEffects[i * m_textureData->w * depth + j * depth + 1] = Uint8(RGBvalue.y);
			m_pixelsWithEffects[i * m_textureData->w * depth + j * depth + 2] = Uint8(RGBvalue.z);

		}
	}
}

const char* Texture::GetExtension(const char* filename)
{
	size_t pathlen = strlen(filename);
	const char* nameP = filename + pathlen;

	while (nameP != filename)
	{
		if (*nameP == '.')
		{
			break;
		}
		--nameP;
	}

	return nameP + 1;
}