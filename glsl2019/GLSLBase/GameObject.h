#pragma once
#include "Transform.h"
#include "Mesh.h"

class Transform;
class GameObject {
public:
	GameObject();
	virtual ~GameObject();

	void bind_resources();
	void render();

	void set_mesh(Mesh* pMesh) { mpMesh = pMesh; };
	Mesh* mesh() const { return mpMesh; }
	Transform& transform() { return mTransform; }

private:
	Mesh*		mpMesh;
	Transform	mTransform;
};