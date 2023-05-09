#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "ECSConstants.h"

#include "Registry/Registry.h"
#include "ECSComponents/ECSComponents.h"

int RunUnitTests(int argc, char* argv[])
{
	return Catch::Session().run(argc, argv);
}

TEST_CASE("Testing SparseSet")
{
	ECS::SparseSet<int> set{};

	REQUIRE(set.Size() == 0);

	SECTION("Testing basic adding")
	{
		set.Add(5);

		REQUIRE(set.Contains(5));
		REQUIRE(set.Size() == 1);
	}

	SECTION("Add and Remove 1 value")
	{
		set.Add(5);

		REQUIRE(set.Contains(5));
		REQUIRE(set.Size() == 1);

		set.Remove(5);

		REQUIRE(set.Size() == 0);
	}

	SECTION("Testing clear")
	{
		for (int i{}; i < 10; ++i)
		{
			set.Add(i);
		}

		REQUIRE(set.Size() == 10);

		set.Clear();

		REQUIRE(set.Size() == 0);
	}
}

TEST_CASE("Testing custom ECS")
{
	ECS::Registry registry{};

	REQUIRE(registry.GetAmountOfEntities() == 0);

	SECTION("Making one entity")
	{
		ECS::Entity entity{ registry.CreateEntity() };

		REQUIRE(entity == 0);
	}

	SECTION("Making 10 entities and testing their updates")
	{
		constexpr int size{ 10 };
		float startPositions[size]{};

		for (int i{}; i < size; ++i)
		{
			ECS::Entity entity{ registry.CreateEntity() };

			registry.AddComponent<GravityComponent>(entity);
			registry.AddComponent<TransformComponent>(entity);
			registry.AddComponent<RigidBodyComponent>(entity);

			startPositions[i] = registry.GetComponent<TransformComponent>(entity).Position.y;
		}

		auto view = registry.CreateView<GravityComponent, TransformComponent>();

		view.ForEach([](const auto& grav, auto& trans)
			{
				trans.Position.y += grav.Gravity; // - 981.0
			});

		for (int i{}; i < size; ++i)
		{
			REQUIRE(startPositions[i] > registry.GetComponent<TransformComponent>(i).Position.y);
		}
	}

	SECTION("Removing Entities")
	{
		struct RemoveEntityTestData
		{
			std::string Name;
		};

		for (int i{}; i < 5; ++i)
		{
			ECS::Entity entity{ registry.CreateEntity() };

			registry.AddComponent<RemoveEntityTestData>(entity, std::to_string(i));
		}

		registry.ReleaseEntity(3);

		REQUIRE(registry.GetAmountOfEntities() == 4);
		REQUIRE(registry.GetComponent<RemoveEntityTestData>(2).Name == "2");
		REQUIRE(registry.GetComponent<RemoveEntityTestData>(4).Name == "4");
	}

	SECTION("Creating many entities")
	{
		struct EntityTest final
		{
			std::string Name;
		};

#pragma warning ( push )
#pragma warning ( disable:4834 )
		for (size_t i{}; i < 10; ++i) registry.CreateEntity();
#pragma warning ( pop )

		ECS::Entity entity{ registry.CreateEntity() };

		registry.AddComponent<EntityTest>(entity, "Entity");

		auto view = registry.CreateView<EntityTest>();

		view.ForEach([](const auto& test)->void
			{
				REQUIRE(test.Name == "Entity");
			});
	}
}