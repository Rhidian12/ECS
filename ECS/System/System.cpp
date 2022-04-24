#include "System.h"

namespace ECS
{
	System::~System()
	{
		//for (std::vector<IComponent*, STLPoolAlloc<IComponent*>>& components : Components)
		//{
		//	for (IComponent*& pC : components)
		//	{
				//delete pC;
				//pC = nullptr;
		//	}
		//}
	}

	Entity System::CreateEntity()
	{
		assert(static_cast<Entity>(Entities.size()) < MaxEntities);

		EntitySignatures.emplace_back(EntitySignature{});
		return Entities.emplace_back(static_cast<Entity>(Entities.size()));
	}
}