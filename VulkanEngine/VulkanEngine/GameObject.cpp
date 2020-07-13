#include "pch.h"
#include "GameObject.h"

#include "EntityManager.h"

#pragma region Constructor

GameObject::GameObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Transform> transform)
{
	this->mesh = mesh;
	this->transform = transform;

	instanceId = -1;
	active = false;
}

#pragma endregion

#pragma region Accessors

std::shared_ptr<Transform> GameObject::GetTransform()
{
	return std::shared_ptr<Transform>();
}

void GameObject::SetTransform(std::shared_ptr<Transform> value)
{
	transform = value;
}

std::shared_ptr<Mesh> GameObject::GetMesh()
{
	return mesh;
}

bool GameObject::GetActive()
{
	return active;
}

#pragma endregion

#pragma region Spawning

void GameObject::Spawn()
{
	if (transform == nullptr) {
		transform = std::make_shared<Transform>();
	}

	instanceId = mesh->AddInstance(transform);
	active = true;
}

#pragma endregion