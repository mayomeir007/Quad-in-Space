#include <gtc/matrix_transform.hpp>
#include "Quad.h"
#include "Shader.h"

Quad::Quad():m_model(1.0f),m_position(0.0f),m_rotation(0.0f),m_scale(1.0f)
{
	m_isDirty = true;

	//data that represents vertices for the quad
	GLfloat vertices[] = { -0.5f,  0.5f, 0.0f,
						    0.5f,  0.5f, 0.0f,
						    0.5f, -0.5f, 0.0f,
					       -0.5f, -0.5f, 0.0f  };

	//data that represents colors for the quad
	GLfloat colors[] = { 1.0f, 1.0f, 1.0f,
					     1.0f, 1.0f, 1.0f,
					     1.0f, 1.0f, 1.0f,
					     1.0f, 1.0f, 1.0f  };

	//data that represents UV coordinates for the quad
	GLfloat UVs[] = { 0.0f, 0.0f,
					  1.f, 0.0f,
					  1.f, 1.f,
					  0.0f, 1.f };

	//index buffer to control the rendering
	GLuint indices[] = { 0, 1, 3,
					     3, 1, 2 };

	m_buffer.CreateBuffer(6, true);
	m_buffer.FillEBO(indices, sizeof(indices), Buffer::FillType::Once);
	m_buffer.FillVBO(Buffer::VBOType::VertexBuffer, vertices, sizeof(vertices), Buffer::FillType::Once);
	m_buffer.FillVBO(Buffer::VBOType::ColorBuffer, colors, sizeof(colors), Buffer::FillType::Once);
	m_buffer.FillVBO(Buffer::VBOType::TextureBuffer, UVs, sizeof(UVs), Buffer::FillType::Once);

	m_buffer.LinkEBO();
	m_buffer.LinkVBO("vertexIn", Buffer::VBOType::VertexBuffer, Buffer::ComponentType::XYZ, Buffer::DataType::FloatData);
	m_buffer.LinkVBO("colorIn", Buffer::VBOType::ColorBuffer, Buffer::ComponentType::RGB, Buffer::DataType::FloatData);
	m_buffer.LinkVBO("textureIn", Buffer::VBOType::TextureBuffer, Buffer::ComponentType::UV, Buffer::DataType::FloatData);
}

Quad::~Quad()
{
	m_texture.Unload();
	m_buffer.DestroyBuffer();
}

/// <summary>
/// updates the model matrix if changes were made to the position, rotation, and/or scale
/// </summary>
void Quad::Update()
{
	if (m_isDirty)
	{
		m_model = glm::mat4(1.0f);
		m_model = glm::translate(m_model, m_position);
		m_model = glm::rotate(m_model, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		m_model = glm::rotate(m_model, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		m_model = glm::rotate(m_model, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		m_model = glm::scale(m_model, m_scale);
		m_isDirty = false;
	}
}

/// <summary>
/// loads a new texture image, and sets it to the default position in 3d space
/// </summary>
/// <param name="filename">path to the image</param>
void Quad::LoadNewTexture(const std::string& filename)
{
	m_texture.Unload(); 
	SetDefaultPosition();
	m_texture.Load(filename);
}

void Quad::SaveTextureImage(const std::string& filename)
{
	m_texture.SaveImage(filename);
}


void Quad::SaveTextureImageWithEffects(const std::string& filename)
{
	m_texture.SaveImageWithEffects(filename);
}

/// <summary>
/// renders the quad, and the texture image if one was loaded
/// </summary>
void Quad::Render()
{
	Shader::Instance()->SendUniformData("model", m_model);

	m_texture.Bind();
	m_buffer.Render(Buffer::DrawType::Triangles);
	m_texture.Unbind();
}

const glm::vec3& Quad::GetPosition() const
{
	return m_position;
}

const glm::vec3& Quad::GetRotation() const
{
	return m_rotation;
}

const glm::vec3& Quad::GetScale() const
{
	return m_scale;
}

void Quad::SetPosition(GLfloat x, GLfloat y, GLfloat z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
	m_isDirty = true;
}

void Quad::SetRotation(GLfloat pitch, GLfloat yaw, GLfloat roll)
{
	m_rotation.x = pitch;
	m_rotation.y = yaw;
	m_rotation.z = roll;
	m_isDirty = true;

}

void Quad::SetScale(GLfloat x, GLfloat y, GLfloat z)
{
	m_scale.x = x;
	m_scale.y = y;
	m_scale.z = z;
	m_isDirty = true;

}

/// <summary>
/// restores quad positionv (includin rrotation and scale) in 3d space to the default values 
/// </summary>
void Quad::SetDefaultPosition()
{
	m_position = glm::vec3(0.0f);
	m_rotation = glm::vec3(0.0f);
	m_scale = glm::vec3(1.0f);

	m_isDirty = true;
}

void Quad::InvertColors()
{
	m_texture.Invert();
	m_texture.Reload();
}

void Quad::Blur(GLfloat blurPercent, bool isInvert)
{
	m_texture.Blur(blurPercent/100,isInvert);
	m_texture.Reload();
}