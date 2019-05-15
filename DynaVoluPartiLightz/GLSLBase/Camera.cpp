#include "stdafx.h"
#include "Camera.h"

Camera::Camera() : mMatrixMode(CameraMatrixMode::Perspective), mFov(45.0f)
{
	mCameraPos = glm::vec3(0.0f, 15.5f, 55.5f);
	//mCameraLookatPos = glm::vec3(0.0f, 0.0f, 0.0f);
	mCameraLookDir = glm::vec3(0,0,-1);
	mCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	mCameraRight = glm::vec3(1.0f,0.0f,0.0f);
}

Camera::~Camera()
{
}

void Camera::SetWindowSize(float _WindowSizeX, float _WindowSizeY)
{
	m_WindowSizeX = _WindowSizeX;
	m_WindowSizeY = _WindowSizeY;
}

void Camera::UpdateCameraMatrix()
{
	mViewMatrix = glm::lookAt(mCameraPos, mCameraPos + mCameraLookDir, mCameraUp);

	switch (mMatrixMode)
	{
	case CameraMatrixMode::Orthographic:
		mProjMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 2.0f);
		mProjViewMatrix = mProjMatrix * mViewMatrix;
		break;
	case CameraMatrixMode::Perspective:
		mProjMatrix = glm::perspective(mFov, (float)m_WindowSizeX / (float)m_WindowSizeY, 0.001f, 1000.f);
		mProjViewMatrix = mProjMatrix * mViewMatrix;
		break;
	}
}

void Camera::InitializeProjMatrices()
{
	
	mViewMatrix = glm::lookAt(mCameraPos, mCameraPos+ mCameraLookDir, mCameraUp);

	switch (mMatrixMode)
	{
	case CameraMatrixMode::Orthographic:
		InitializeOrthoProjMatrices();
		break;
	case CameraMatrixMode::Perspective:
		InitializePerspectiveProjMatrices();
		break;
	}

}

void Camera::InitializeOrthoProjMatrices()
{
	
	mProjMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 2.0f);
	mProjViewMatrix = mProjMatrix * mViewMatrix;
}

void Camera::InitializePerspectiveProjMatrices()
{
	mViewMatrix = glm::lookAt(mCameraPos, mCameraPos + mCameraLookDir, mCameraUp);

	// near 를 0 으로 하지 말 것 ( 깊이 날아가는 문제 )
	mProjMatrix = glm::perspective(mFov, (float)m_WindowSizeX / (float)m_WindowSizeY, 0.001f, 1000.f);
	mProjViewMatrix = mProjMatrix * mViewMatrix;
}

void Camera::Rotate(const glm::vec3& vAxis, float fAngle)
{
	glm::mat3x3 mtxRotate = glm::rotate(glm::mat4x4(), DEGREE_TO_RADIAN(fAngle), vAxis);
	glm::vec3 vLook = mCameraLookDir * mtxRotate;
	mCameraRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), vLook));
	mCameraUp = glm::normalize(glm::cross(vLook, mCameraRight));
	mCameraLookDir = glm::normalize(glm::cross(mCameraUp, mCameraRight));
	UpdateCameraMatrix();
}

void Camera::KeyInput(unsigned char key, int x, int y)
{
	static const float weight = 0.25f;
	switch (key) {
	case 'w': case 'W':
		mCameraPos += mCameraLookDir * weight;
		break;
	case 'a': case 'A':
		mCameraPos += -mCameraRight * weight;
		break;
	case 's': case 'S':
		mCameraPos += -mCameraLookDir * weight;
		break;
	case 'd': case 'D':
		mCameraPos += mCameraRight * weight;
		break;
	}
	UpdateCameraMatrix();
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
			Rotate(mCameraUp, dx * weight);
			Rotate(-mCameraRight, dy * weight);

			prev_x = x;
			prev_y = y;
			UpdateCameraMatrix();
		}
		break;
	}
}
