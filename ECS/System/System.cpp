#include "System.h"

namespace ECS
{
	void System::AddEntity(const Entity id)
	{
		Entities.insert(id);
	}

	void System::RemoveEntity(const Entity id) noexcept
	{
		Entities.erase(id);
	}
}