#include "stdafx.h"
#include "Camera.h"

Camera::Camera() : mMatrixMode(CameraMatrixMode::Perspective), mFov(45.0f)
{
	mCameraPos = glm::vec3(0.0f, 15.5f, 55.5f);
	mCameraLookatPos = glm::vec3(0.0f, 0.0f, 0.0f);
	mCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
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
	mViewMatrix = glm::lookAt(mCameraPos, mCameraLookatPos, mCameraUp);

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
	
	mViewMatrix = glm::lookAt(mCameraPos, mCameraLookatPos, mCameraUp);

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
	mViewMatrix = glm::lookAt(mCameraPos, mCameraLookatPos, mCameraUp);

	// near 를 0 으로 하지 말 것 ( 깊이 날아가는 문제 )
	mProjMatrix = glm::perspective(mFov, (float)m_WindowSizeX / (float)m_WindowSizeY, 0.001f, 1000.f);
	mProjViewMatrix = mProjMatrix * mViewMatrix;
}
