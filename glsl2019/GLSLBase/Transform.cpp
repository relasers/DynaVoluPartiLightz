#include "stdafx.h"
#include "Transform.h"

Transform::Transform()
	: mPosition(0.0f)
	, mEulerAngles(0.0f)
	, mQuaternion(glm::vec3(0.0f))
	, mScale(1.0f)
	, mTransform(1.0f)
{
}

Transform::~Transform()
{
}

void Transform::set_pos(const glm::vec3& pos)
{
	mPosition = pos;
	GenerateMatrixFromParam();
}

void Transform::set_angle(const glm::vec3& angle)
{
	mEulerAngles = angle;
	GenerateMatrixFromParam();
}

void Transform::set_quat(const glm::quat& quat)
{
	mQuaternion = quat;
	GenerateMatrixFromParam();
}

void Transform::set_scale(const glm::vec3& scale)
{
	mScale = scale;
	GenerateMatrixFromParam();
}

void Transform::set_mtx(const glm::mat4& transform)
{
	mTransform = transform;
	GenerateParamFromMatrix();
}

Transform& Transform::operator=(const Transform & other)
{
	mPosition = other.mPosition;
	mEulerAngles = other.mEulerAngles;
	mQuaternion = other.mQuaternion;
	mScale = other.mScale;
	mTransform = other.mTransform;
	return *this;
}

void Transform::dump()
{
	std::cout << mTransform[0][0] << " " << mTransform[0][1] << " " << mTransform[0][2] << " " << mTransform[0][3] << std::endl;
	std::cout << mTransform[1][0] << " " << mTransform[1][1] << " " << mTransform[1][2] << " " << mTransform[1][3] << std::endl;
	std::cout << mTransform[2][0] << " " << mTransform[2][1] << " " << mTransform[2][2] << " " << mTransform[2][3] << std::endl;
	std::cout << mTransform[3][0] << " " << mTransform[3][1] << " " << mTransform[3][2] << " " << mTransform[3][3] << std::endl;
}

void Transform::GenerateMatrixFromParam()
{
	glm::mat4 translate = glm::translate(glm::mat4(1.f), mPosition);
	glm::mat4 rotate = glm::toMat4(mQuaternion);
	glm::mat4 scale = glm::scale(glm::mat4(1.f), mScale);
	mTransform = translate * rotate * scale;
}

void Transform::GenerateParamFromMatrix()
{
	mPosition = mTransform[3];
	mQuaternion = glm::toQuat(mTransform);
	mEulerAngles = glm::eulerAngles(mQuaternion);
	glm::mat4 inv_translate = glm::inverse(glm::translate(glm::mat4(1.f), mPosition));
	glm::mat4 inv_rotate = glm::inverse(glm::toMat4(mQuaternion));
	glm::mat4 scale = inv_rotate * inv_translate * mTransform;
	mScale = glm::vec3(scale[0][0], scale[1][1], scale[2][2]);
}
