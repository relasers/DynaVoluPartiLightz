#pragma once
#include "stdafx.h"
#include "GameObject.h"


GameObject::GameObject()
{
	mTransform = std::make_unique<Transform>();
	mTransform->Update();
}

GameObject::GameObject(GameObject&& _other)
{
	mTransform = std::move(_other.mTransform);
	mMesh = _other.mMesh;
}

GameObject::~GameObject()
{
}

GameObject& GameObject::operator=(const GameObject& rhs)
{
	if (this == &rhs)      // 동일 객체인가?
		return *this;

	mTransform.reset(new Transform(*rhs.mTransform));
	mMesh = rhs.mMesh;

	return *this;
}

GameObject& GameObject::operator=(GameObject&& rhs)
{
	if (this == &rhs)      // 동일 객체인가?
		return *this;

	//POD types use operator= as normal
	mTransform = std::move(rhs.mTransform);

	return *this;
}