#include <gtc\matrix_transform.hpp>
#include "Camera.h"
#include "Shader.h"

Camera::Camera()
{
	m_view = glm::mat4(1.0f);
	m_proj = glm::mat4(1.0f);

	m_position = glm::vec3(0.0f,0.0f,2.0f);
	m_direction = glm::vec3(0.0f, 0.0f, -1.0f);
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);

	m_view = glm::lookAt(m_position, m_position + m_direction, m_up);
	Shader::Instance()->SendUniformData("view", m_view);
}

/// <summary>
/// sets the projection matrix and sends it to the graphics pipleine
/// </summary>
void Camera::Set3DView()
{

	GLfloat FOV = 45.0f;
	GLfloat aspectRatio = 1280.0f / 720.0f;

	m_proj = glm::perspective(FOV, aspectRatio, 0.001f, 1000.0f);
	Shader::Instance()->SendUniformData("proj", m_proj);
}

/// <summary>
/// sets the viewport of the cmera to the desired dimensions in screen coordinates
/// </summary>
/// <param name="x">left of viewport</param>
/// <param name="y">bottom of viewport</param>
/// <param name="width">width of viewport</param>
/// <param name="height">height of viewport</param>
void Camera::SetViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	glViewport(x, y, width, height);
}
