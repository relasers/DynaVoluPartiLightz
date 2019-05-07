#include "stdafx.h"
#include "CCamera.h"


CCamera::CCamera()
{
	m_v3CameraPos = glm::vec3(0, 0, 1);
	m_v3CameraLookatPos = glm::vec3(0.0f, 0.0f, -1.0f);
	m_v3CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

CCamera::~CCamera()
{
}

void CCamera::Initalize(int windowSizeX, int windowSizeY)
{
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	m_v3CameraDirection = glm::normalize(m_v3CameraPos - m_v3CameraLookatPos);
	m_v3CameraRight = glm::normalize(glm::cross(m_v3CameraUp, m_v3CameraDirection));
	m_v3CameraUp = glm::cross(m_v3CameraDirection, m_v3CameraRight);

	m_m4View = glm::lookAt(m_v3CameraPos, m_v3CameraPos + m_v3CameraLookatPos, m_v3CameraUp);
	//m_m4View = glm::lookAt(m_v3CameraPos, m_v3CameraLookatPos, m_v3CameraUp);

	switch (m_CameraType)
	{
	case CameraType::Ortho:
		m_m4Proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.001f, 1000.0f);
		break;

	case CameraType::Perpective:
		m_m4Proj = glm::perspective(45.f, (float)m_WindowSizeX / (float)m_WindowSizeY, 0.001f, 1000.0f);
		break;
	}

	m_m4ProjView = m_m4Proj * m_m4View;
}

void CCamera::KeyInputProcessing(unsigned char key)
{
	if (key == 'w' || key == 'W')
	{
		m_v3CameraPos += m_CameraSpeed * m_v3CameraLookatPos;
	}
	else if (key == 's' || key == 'S')
	{
		m_v3CameraPos -= m_CameraSpeed * m_v3CameraLookatPos;
	}

	if (key == 'a' || key == 'A')
	{
		m_v3CameraPos -= m_v3CameraRight * m_CameraSpeed;
	}
	else if (key == 'd' || key == 'D')
	{
		m_v3CameraPos += m_v3CameraRight * m_CameraSpeed;
	}

	if (key == 'r' || key == 'R')
	{
		m_v3CameraPos += m_v3CameraUp * m_CameraSpeed;
	}
	else if (key == 'f' || key == 'F')
	{
		m_v3CameraPos -= m_v3CameraUp * m_CameraSpeed;
	}

	m_m4View = glm::lookAt(m_v3CameraPos, m_v3CameraPos + m_v3CameraLookatPos, m_v3CameraUp);
	m_m4ProjView = m_m4Proj * m_m4View;
}

void CCamera::MouseInput(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (m_Clicked == false)
		{
			m_MouseDownPoint[0] = x;
			m_MouseDownPoint[1] = y;
			m_Clicked = true;
		}
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		m_Clicked =false;
	}
}

void CCamera::MouseMotion(int x, int y)
{
	if (m_Clicked)
	{

	}
}

// Set-------------------------------------------------------------------
void CCamera::SetCameraType(CameraType _type)
{
	m_CameraType = _type;
}

void CCamera::SetCameraPos(glm::vec3 _CameraPos)
{
	m_v3CameraPos = _CameraPos;
}

void CCamera::SetCameraLookatPos(glm::vec3 _CameraLookatPos)
{
	m_v3CameraLookatPos = _CameraLookatPos;
}

void CCamera::SetCameraUp(glm::vec3 _CameraUp)
{
	m_v3CameraUp = _CameraUp;
}

// Get------------------------------------------------------------------
glm::vec3 CCamera::GetCameraPos()
{
	return m_v3CameraPos;
}

glm::vec3 CCamera::GetCameraLookatPos()
{
	return m_v3CameraLookatPos;
}

glm::vec3 CCamera::GetCameraUp()
{
	return m_v3CameraUp;
}

glm::mat4 CCamera::GetProjViewMat()
{
	return m_m4ProjView;
}
