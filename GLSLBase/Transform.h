#pragma once


class Transform
{
private:

	glm::vec3 mPosition;
	glm::vec3 mEulerAngle;
	glm::vec4 mQuaternion;
	glm::vec3 mScale;

	glm::mat4 m_m4Model;


public:
	Transform();
	~Transform();

	void Update();
	glm::mat4 GetModelMatrix() { return m_m4Model; };
	Transform& operator= (const Transform& _Transform);

private:
	void UpdateMatrix();
};