#pragma once
#include "../ECSConstants.h"

#include <queue> /* std::queue */
#include <vector> /* std::vector */

namespace ECS
{
	class EntityManager final
	{
	public:
		EntityManager();

		Entity CreateEntity() noexcept;
		void DestroyEntity(const Entity id) noexcept;

		void SetSignatureSafe(const Entity id, const ComponentType& signature);
		const EntitySignature& GetEntitySignatureSafe(const Entity id) noexcept;

		__forceinline auto SetSignatureUnsafe(const Entity id, const EntitySignature& signature) noexcept { EntitySignatures[id] = signature; }
		__forceinline auto GetSignatureUnsafe(const Entity id) const noexcept { return EntitySignatures[id]; }

	private:
		std::queue<Entity> AvailableEntityIDs;
		std::vector<EntitySignature> EntitySignatures; /* Although this vector's size will be constant, we need dynamically allocated memory */
	};
}