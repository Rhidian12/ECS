#include "System.h"

namespace ECS
{
	System::~System()
	{
		for (ComponentInfo& info : Components)
		{
			for (IComponent*& pC : info.Components)
			{
				delete pC;
				pC = nullptr;
			}
		}
	}

	Entity System::CreateEntity() noexcept
	{
		assert(static_cast<Entity>(Entities.size()) < MaxEntities);

		EntitySignatures.emplace_back(EntitySignature{});
		return Entities.emplace_back(static_cast<Entity>(Entities.size()));
	}
}