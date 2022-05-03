#include "System.h"

namespace ECS
{
	System::~System()
	{
		for (const auto& container : Components)
		{
			for (IComponent* pC : container)
			{
				//PoolAllocator::deallocate(pC);
				delete pC;
			}
		}
	}

	Entity System::CreateEntity()
	{
		assert(static_cast<Entity>(Entities.size()) < MaxEntities);

		EntitySignatures.push_back(EntitySignature{});
		Entities.push_back(static_cast<Entity>(Entities.size()));
		return Entities.back();
	}
}