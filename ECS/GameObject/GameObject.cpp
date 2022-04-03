#include "GameObject.h"

GO::GameObject::~GameObject()
{
	for (Component* pC : Components)
	{
		delete pC;
		pC = nullptr;
	}
}

void GO::GameObject::AddComponent(Component* pComponent)
{
	Components.push_back(pComponent);
}

void GO::GameObject::Update()
{
	for (Component* const pComponent : Component)
	{

	}
}
