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

Transform & Transform::operator=(const Transform & _Transform)
{
	mPosition = _Transform.mPosition;
	mEulerAngle = _Transform.mEulerAngle;
	mQuaternion = _Transform.mQuaternion;
	mScale = _Transform.mScale;
	return *this;
}

void Transform::UpdateMatrix()
{
	glm::mat4 m4Trans = glm::translate(glm::mat4(1.f), mPosition);
	mQuaternion = glm::quat(mEulerAngle);
	glm::mat4 m4Rot = glm::toMat4(mQuaternion);

	glm::mat4 m4Scale = glm::scale(glm::mat4(50.f), mScale);

	m_m4Model = m4Trans * m4Rot * m4Scale;
}
