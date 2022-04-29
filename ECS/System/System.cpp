#include "System.h"

namespace ECS
{
	System::~System()
	{
		//for (size_t i{}; i < Components.Size(); ++i)
		//{
		//	for (size_t j{}; j < Components[j].Size(); ++j)
		//	{
		//		delete Components[i][j];
		//	}
		//}
	}

	Entity System::CreateEntity()
	{
		assert(static_cast<Entity>(Entities.Size()) < MaxEntities);

		EntitySignatures.Emplace(EntitySignature{});
		Entities.Emplace(static_cast<Entity>(Entities.Size()));
		return Entities.Back();
	}
}