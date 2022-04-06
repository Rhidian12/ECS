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


#ifdef DEBUG
	Entity System::CreateEntity()
	{
		assert(static_cast<Entity>(Entities.size()) < MaxEntities);

		return Entities.emplace_back(static_cast<Entity>(Entities.size()));
	}
#endif
}