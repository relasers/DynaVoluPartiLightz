#pragma once


class Transform
{
private:
	glm::vec3 mPosition;
	glm::vec3 mEulerAngle;
	glm::quat mQuaternion;
	glm::vec3 mScale;
	glm::mat4 m_m4Model;
public:
	Transform();
	~Transform();

	void Update();
	void SetPosition(glm::vec3 _Position);
	void SetEulerAngle(glm::vec3 _EulerAngle);
	void SetScale(glm::vec3 _Scale);

	glm::mat4 GetModelMatrix() { 
		return m_m4Model; };
	Transform& operator= (const Transform& _Transform);

	glm::vec3 GetEulerAngle() const { return mEulerAngle; }
	void Rotate(glm::vec3 _Axis, float delta);
	void PrintModelMatrix();

private:
	void UpdateMatrix();
};
