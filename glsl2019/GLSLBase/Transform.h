#pragma once

class Transform {
public:
	Transform();
	~Transform();

	void set_pos(const glm::vec3& pos);
	void set_angle(const glm::vec3& angle);
	void set_quat(const glm::quat& quat);
	void set_scale(const glm::vec3& scale);
	void set_mtx(const glm::mat4& transform);

	glm::vec3 pos() const { return mPosition; };
	glm::vec3 angle() const { return mEulerAngles; };
	glm::quat quat() const { return mQuaternion; };
	glm::vec3 scale() const { return mScale; };
	glm::mat4 mtx() const { return mTransform; };

	Transform& operator= (const Transform& other);

	void dump();

private:
	void GenerateMatrixFromParam();
	void GenerateParamFromMatrix();

public:
	glm::vec3 mPosition;
	glm::vec3 mEulerAngles;
	glm::quat mQuaternion;
	glm::vec3 mScale;
	glm::mat4 mTransform;
};
