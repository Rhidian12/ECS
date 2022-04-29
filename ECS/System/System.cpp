#include "System.h"

namespace ECS
{
	System::~System()
	{
		for (const auto& container : Components)
		{
			for (IComponent* pC : container)
			{
				PoolAllocator::deallocate(pC);
			}
		}
	}

	Entity System::CreateEntity()
	{
		assert(static_cast<Entity>(Entities.Size()) < MaxEntities);

		EntitySignatures.Emplace(EntitySignature{});
		Entities.Emplace(static_cast<Entity>(Entities.Size()));
		return Entities.Back();
	}
}