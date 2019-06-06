#pragma once
#include "stdafx.h"
#include "GameObject.h"


GameObject::GameObject() : mpMesh(nullptr)
{
}

GameObject::~GameObject()
{
}

void GameObject::bind_resources()
{
	if(nullptr != mpMesh) mpMesh->bind();
}

void GameObject::render()
{
	if (nullptr != mpMesh) mpMesh->render();
}
