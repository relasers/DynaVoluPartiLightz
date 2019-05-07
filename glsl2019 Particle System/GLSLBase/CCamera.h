#pragma once

#include "Dependencies\glew.h"
#include "Dependencies\freeglut.h"

#include "Dependencies/glew.h"
#include "Dependencies/glm/glm.hpp"
#include "Dependencies/glm/gtc/matrix_transform.hpp"

enum CameraType {
	Ortho,
	Perpective
};

class CCamera
{
public:
	CCamera();
	~CCamera();

	void Initalize(int windowSizeX, int windowSizeY);

	void KeyInputProcessing(unsigned char key);
	void MouseInput(int button, int state, int x, int y);
	void MouseMotion(int x, int y);

	// Projection Type
	void SetCameraType(CameraType _type);

	// Pos, Look, Up
	void SetCameraPos(glm::vec3 _CameraPos);
	void SetCameraLookatPos(glm::vec3 _CameraLookatPos);
	void SetCameraUp(glm::vec3 _CameraUp);

	// Get
	glm::vec3 GetCameraPos();
	glm::vec3 GetCameraLookatPos();
	glm::vec3 GetCameraUp();
	glm::mat4 GetProjViewMat();

private:
	unsigned int m_WindowSizeX{ 0 };
	unsigned int m_WindowSizeY{ 0 };

	float m_CameraSpeed = 0.05f;

	CameraType m_CameraType;

	glm::vec3 m_v3CameraUp;
	glm::vec3 m_v3CameraLookatPos;
	glm::vec3 m_v3CameraPos;

	glm::vec3 m_v3CameraDirection;
	glm::vec3 m_v3CameraRight;

	glm::vec3 m_v3CameraFront;

	// Matrices
	glm::mat4 m_m4View;
	glm::mat4 m_m4Proj;
	glm::mat4 m_m4ProjView;

	//
	bool m_Clicked = true;
	int m_MouseDownPoint[2];
};