#pragma once

#include <glm.hpp>
#include "gl.h"
#include "Buffer.h"
#include "Texture.h"

class Quad
{

public:

	Quad();
	~Quad();

	void Update();
	void Render();
	void LoadNewTexture(const std::string& filename);
	void SaveTextureImage(const std::string& filename);
	void SaveTextureImageWithEffects(const std::string& filename);

	const glm::vec3& GetPosition() const;
	const glm::vec3& GetRotation() const;
	const glm::vec3& GetScale() const;

	void SetPosition(GLfloat x, GLfloat y, GLfloat z);
	void SetRotation(GLfloat pitch, GLfloat yaw, GLfloat roll);
	void SetScale(GLfloat x, GLfloat y, GLfloat z);

	void SetDefaultPosition();

	void InvertColors();
	void Blur(GLfloat blurPercent, bool isInvert);


private:

	Buffer m_buffer;	
	Texture m_texture;

	bool m_isDirty;

	glm::mat4 m_model;
	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;

};