#include "stdafx.h"
#include "Camera.h"

Camera::Camera()
	: m_vUp			{ glm::vec3(0.0f, 1.0f, 0.0f) }
	, m_vLook		{ glm::vec3(0.0f, -0.2f, -1.0f) }
	, m_vRight		{ glm::vec3(1.0f, 0.0f, 0.0f) }
	, m_vPos		{ glm::vec3(0.0f, 0.0f, 0.6f) }
	, m_ProjType	{ ProjType ::Perspective }
	, m_fFovy		{ 45.0f }
	, m_fAspect		{ static_cast<float>(g_ClientWidth) / static_cast<float>(g_ClientHeight) }
	, m_fNear		{ 0.05f }
	, m_fFar		{ 1000.0f }
{
	GenerateProjView(m_ProjType);
}

void Camera::GenerateProjView(ProjType proj)
{
	switch (proj) {
	case ProjType::Orthographic: GenerateOrthographicProjView(); break;
	case ProjType::Perspective: GeneratePerspectiveProjView(); break;
	}
}

void Camera::GenerateOrthographicProjView()
{
	m_ProjType = ProjType::Orthographic;
	m_mtxView = glm::lookAt(m_vPos, m_vPos + m_vLook, m_vUp);
	m_mtxProj = glm::ortho(
		  -static_cast<float>(g_ClientWidth) * 0.5f
		, static_cast<float>(g_ClientWidth) * 0.5f
		, -static_cast<float>(g_ClientHeight) * 0.5f
		, static_cast<float>(g_ClientHeight) * 0.5f
		, m_fNear
		, m_fFar);
	m_mtxProjView = m_mtxProj * m_mtxView;
}

void Camera::GeneratePerspectiveProjView()
{
	m_ProjType = ProjType::Perspective;
	m_mtxView = glm::lookAt(m_vPos, m_vPos + m_vLook, m_vUp);
	m_mtxProj = glm::perspective(m_fFovy, m_fAspect, m_fNear, m_fFar);
	m_mtxProjView = m_mtxProj * m_mtxView;
}

void Camera::RegenerateProjView()
{
	GenerateProjView(m_ProjType);
}

void Camera::Rotate(const glm::vec3& vAxis, float fAngle)
{
	glm::mat3x3 mtxRotate = glm::rotate(glm::mat4x4(), DEGREE_TO_RADIAN(fAngle), vAxis);
	glm::vec3 vLook = m_vLook * mtxRotate;
	m_vRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), vLook));
	m_vUp = glm::normalize(glm::cross(vLook, m_vRight));
	m_vLook = glm::normalize(glm::cross(m_vUp, m_vRight));
	RegenerateProjView();
}

void Camera::KeyInput(unsigned char key, int x, int y)
{
	static const float weight = 0.05f;
	switch (key) {
	case 'w': case 'W':
		m_vPos += m_vLook * weight;
		break;
	case 'a': case 'A':
		m_vPos += -m_vRight * weight;
		break;
	case 's': case 'S':
		m_vPos += -m_vLook * weight;
		break;
	case 'd': case 'D':
		m_vPos += m_vRight * weight;
		break;
	}
	RegenerateProjView();
}

void Camera::MouseInput(int x, int y, int button, int state)
{
	static bool bMouseDown = false;
	static int prev_x = 0, prev_y = 0;
	switch (button) {
	case GLUT_LEFT_BUTTON:
		switch (state) {
		case GLUT_DOWN:
			break;
		case GLUT_UP:
			break;
		}
		break;
	case GLUT_RIGHT_BUTTON:
		switch (state) {
		case GLUT_DOWN:
			bMouseDown = true;
			prev_x = x;
			prev_y = y;
			break;
		case GLUT_UP:
			bMouseDown = false;
			break;
		}
		break;
	case -1:
		if (true == bMouseDown) {
			static const float weight = 0.1f;
			int dx = x - prev_x;
			int dy = y - prev_y;
			Rotate(m_vUp, dx * weight);
			Rotate(-m_vRight, dy * weight);

			prev_x = x;
			prev_y = y;
			RegenerateProjView();
		}
		break;
	}
}
