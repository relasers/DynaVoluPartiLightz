#pragma once
#include "Transform.h"
#include "Mesh.h"

class Transform;
class GameObject
{
private:
	std::unique_ptr<Transform> mTransform;
	Mesh* mMesh;
public:
	GameObject();
	GameObject(GameObject&& _other);
	GameObject& operator=(const GameObject &rhs);
	GameObject& operator=(GameObject&& rhs);
	virtual ~GameObject();

	Transform* GetTransform() { return mTransform.get(); }
	void SetPosition(glm::vec3 _Position) { mTransform->SetPosition(_Position); };
	void SetEulerAngle(glm::vec3 _EulerAngle) { mTransform->SetEulerAngle(_EulerAngle); };
	void SetScale(glm::vec3 _Scale) { mTransform->SetScale(_Scale); };

	void SetMesh(Mesh* _Mesh) { mMesh = _Mesh; };
	Mesh* GetMesh() const { 
		if (mMesh != nullptr) return mMesh;
		else return nullptr; }
};