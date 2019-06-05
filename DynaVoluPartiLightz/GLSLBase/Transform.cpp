#include "stdafx.h"
#include "Transform.h"

Transform::Transform()
{
	Update();
}

Transform::~Transform()
{
}

void Transform::Update()
{
	UpdateMatrix();
	
}

void Transform::SetPosition(glm::vec3 _Position)
{
	mPosition = _Position;
	Update();
}

void Transform::SetEulerAngle(glm::vec3 _EulerAngle)
{
	mEulerAngle = _EulerAngle;
	Update();
}

void Transform::SetScale(glm::vec3 _Scale)
{
	mScale = _Scale;
	Update();
}

Transform & Transform::operator=(const Transform & _Transform)
{
	mPosition = _Transform.mPosition;
	mEulerAngle = _Transform.mEulerAngle;
	mQuaternion = _Transform.mQuaternion;
	mScale = _Transform.mScale;
	return *this;
}

void Transform::Rotate(glm::vec3 _Axis, float delta)
{
	mEulerAngle += _Axis * delta;
	Update();
}

void Transform::PrintModelMatrix()
{
	std::cout << m_m4Model[0][0] << " " << m_m4Model[0][1] << " " << m_m4Model[0][2] << " " << m_m4Model[0][3] << std::endl;
	std::cout << m_m4Model[0][0] << " " << m_m4Model[1][1] << " " << m_m4Model[1][2] << " " << m_m4Model[1][3] << std::endl;
	std::cout << m_m4Model[0][0] << " " << m_m4Model[2][1] << " " << m_m4Model[2][2] << " " << m_m4Model[2][3] << std::endl;
	std::cout << m_m4Model[0][0] << " " << m_m4Model[3][1] << " " << m_m4Model[3][2] << " " << m_m4Model[3][3] << std::endl;
}

void Transform::UpdateMatrix()
{
	glm::mat4 m4Trans = glm::translate(glm::mat4(1.f), mPosition);
	mQuaternion = glm::quat(mEulerAngle);
	glm::mat4 m4Rot = glm::toMat4(mQuaternion);

	glm::mat4 m4Scale = glm::scale(glm::mat4(1.f), mScale);

	m_m4Model = m4Trans * m4Rot * m4Scale;
}
