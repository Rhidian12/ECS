# ECS

## Introduction
An Entity-Component-System (ECS) is an alternative way of storing information regarding Components and Entities.
The traditional approach is to have a `GameObject` that has a list of `Component`, in which the `GameObject` acts as a wrapper around the `Component` list and manages all of them. This is a very good approach, but has some drawbacks:
- There are a lot of virtual function calls present, since the `GameObject` (usually) has a pointer to a base `Component` class, on which it calls an `Update` function for example.
- The Components are all scattered in memory, resulting in a lot of cache misses. For example, assume we have 1.000 rocks in our scene, and all of these rocks float up and down in the air. These rocks will need some `RockMovementComponent` that continiously translates the rock it's attached to. This will have to happen a 1.000 times per frame, and every time we need to go through our virtual function call, go fetch the data from memory, which is probably scattered all over the place, since the Components are not stored contiguously.

```cpp
class RockMovementComponent final : public Component
{
public:
  RockMovementComponent(GameObject* pOwner)
  : pOwner{ pOwner }
  , pTransformComponent{}
  , pAnimationCurveComponent{}
  , RockSpeed{ 100.f }
  {
    pTransformComponent = pOwner->GetComponent<TransformComponent>();
    pAnimationCurveComponent = pOwner->GetComponent<AnimationCurveComponent>();
    StartPosition = pTransformComponent->GetPosition();
    EndPosition = Vec3{ StartPosition.x, StartPosition.y + 500.f, StartPosition.z };
  }
  
  virtual void Update(float DeltaTime) override
  {
    /* Assume there is an Animation Curve set up, as in Unreal Engine, that ranges from 0 -> 1 -> 0 */
    pTransformComponent->SetPosition(Lerp(StartPosition, EndPosition, pAnimationCurveComponent->GetFloatValue()));
  }
  
private:
  GameObject* pOwner;
  TransformComponent* pTransformComponent;
  AnimationCurveComponent* pAnimationCurveComponent;
  float RockSpeed;
  Vec3 StartPosition;
  Vec3 EndPosition;
};
```

Instead of having tons of scattered components all doing the same thing, needing the same type of data, stored in different GameObjects. We could store all of these Components together, preventing cache misses, and let overarching systems execute code.

This is the basis of an ECS.
Instead of using GameObjects which manage the Components, we have Entities, which are nothing more than an ID.
Instead of using Components which hold data and have functionality regarding that data, our Components only hold data.
The final part of the ECS is a `System`. Systems are classes that have a list of `Entities`, and execute specific functionality on each `Entity`, provided the `Entity` has the required Components.

```cpp
class RockMovementSystem final : public System<RockMovementSystem>
{
public:
  virtual void UpdateSystem() override
  {
    for (const Entity& entity : Entities)
    {
      if (pEntityManager->GetEntitySignature(entity) == flags)
      {
        TransformComponent* const pTransformComponent{ pComponentManager->GetComponent<TransformComponent>(entity) };
        AnimationCurveComponent* const pAnimationCurve{ pComponentManager->GetComponent<AnimationCurveComponent>(entity);
        RockComponent* const pRockComponent{ pComponentManager->GetComponent<RockComponent>(entity) };

        pTransformComponent->SetPosition(Lerp(pRockComponent->StartPosition, pRockComponent->EndPosition, pAnimationCurveComponent->GetFloatValue()));
      }
    }
  }

  inline static std::bitset<MaxComponentTypes> flags{};
};
```

The reason an ECS is faster than the traditional approach is because of Data Oriented Design (which once again leads into cache misses and hits).
For those unfamiliar with a cache miss and hit, here's an oversimplification: <br>
Whenever the program needs to access data in memory, it searches in the cache for that specific piece of data, if the data is present in the cache, we get a cache hit and we can continue. <br>
However, when the data is *not* in the cache, we get a cache *miss* and the program needs to search the other pieces of memory for the specific data we requested.
This is a slight problem, since the cache is extremely fast, while the rest of the memory is a lot slower.<br>
All of this relates to the `Locality of Reference`.<br>
This means that whenever data is requested from memory, the cache will grab not only that specific piece of data, but also some of the data adjacent in memory to the requested data. It does this in the hope that the adjacent data is requested, and therefore if requested, instantly produces a cache hit, instead of a cache miss and a search through memory.

The way we can follow this Data Oriented Design and pay heed to the Locality of Reference is by making sure that all of our data is stored contiguously in memory.
For those who are unaware of what contiguous storage means: `std::vector`, the standard C-array (`int[]`) and many other containers use contiguous storage, which means that they store all of their data directly next to each other in memory.

## Implementation

### Components

This ECS uses a `ComponentManager` to store all Components and their Component ID's. Each Component has a unique ID which the ComponentManager uses to distinguish between different Components. <br>
The way Components have different ID's is by using a combination of static variables and templating. <br>
All user-defined Components must inherit from `Component`, which inherits from `IComponent`. The templated `Component` class requires a derived Component as its template, which then uses to instantiate `TypeCounter`. `TypeCounter` will always have the same base class, `IComponent`, but will be called with a different function template `DerivedComponent`.

```cpp
#pragma once
#include "../ECSConstants.h"
#include "../TypeCounter/TypeCounter.h"

namespace ECS
{
  class IComponent
  {
  public:
    virtual ~IComponent() = default;
  };

  template<typename DerivedComponent>
  class Component : public IComponent
  {
  public:
    virtual ~Component() = default;

    static __forceinline auto GetComponentID() noexcept { return ComponentID; }

  protected:
    inline static const ComponentType ComponentID{ TypeCounter<IComponent>::Get<DerivedComponent>() };
  };
}
```

```cpp
#pragma once
#include "../ECSConstants.h"

namespace ECS
{
  /* This class is heavily based on a class made by the absolute genius Arne Van Kerschaver */
  template <typename Type>
  class TypeCounter final
  {
  public:
    /* Since it's ++Counter, the Component Counter will always start at 1, can be easily changed, but who cares */
    template <typename SecondaryType>
    __forceinline constexpr static ComponentType Get() { return ++Counter; }

    __forceinline constexpr static ComponentType Get() { return Counter; }

  private:
    inline static ComponentType Counter{};
  };
}
```

The `ComponentManager` itself has a `std::vector` of `ComponentKey`, a struct which holds a Component ID and a list of `IComponent*`. <br>
When a new Component gets added to the ComponentManager, it is placed in the list at the index of its ComponentID. This makes it very easy to access Components via `GetComponent`, since all that is required is the ComponentID, and an EntityID. The ComponentID is always available, since it is a static function, which allows me to easily use `Type::GetComponentID()`, which works, as long as `Type` is a class that is derived from `Component`.<br>
A possible safety check for this is using SFINAE, in the form of `std::enable_if_t<std::is_base_of_v<Component, Type>>`.

```cpp
template<typename Type>
Type* ComponentManager::GetComponent(const Entity id) const noexcept
{
  if (id == InvalidEntityID)
    return nullptr;

  const auto componentID{ Type::GetComponentID() };
  const ComponentKey& key{ Components[componentID] };

  if (key.ComponentID != InvalidComponentID)
  {
    return static_cast<Type*>(Components[componentID].Components[id]);
  }
  else
  {
    return nullptr;
  }
}
  ```

### Entities

Entities are managed by the `EntityManager`, which holds a list of all possible Entities in existence, whether they're being used or not. It also allows the user to create and destroy entities, by reassigning Entity ID's. I used a `std::queue` to hold all possible Entity ID's, since it allows for easy pushing and popping of ID's.

```cpp
#pragma once
#include "../ECSConstants.h"

#include <queue> /* std::queue */
#include <array> /* std::array */

namespace ECS
{
  class EntityManager final
  {
  public:
    EntityManager();

    Entity CreateEntity() noexcept;
    void DestroyEntity(const Entity id) noexcept;

    void SetSignatureSafe(const Entity id, const ComponentType& signature) noexcept;
    const EntitySignature& GetEntitySignatureSafe(const Entity id) noexcept;

    __forceinline auto SetSignatureUnsafe(const Entity id, const EntitySignature& signature) noexcept { EntitySignatures[id] = signature; }
    __forceinline auto GetSignatureUnsafe(const Entity id) const noexcept { return EntitySignatures[id]; }

  private:
    std::queue<Entity> AvailableEntityIDs;
    std::array<EntitySignature, MaxEntities> EntitySignatures;
  };
}
```

### Systems

The Systems are managed by the `SystemManager`. It manages the lifetime of all the Systems, creates them, and also executes them for ease-of-use. <br>
The systems themselves are almost identical to the Components. They have a list of Entities and a static templated SystemID.

```cpp
#pragma once
#include "../ECSConstants.h"
#include "../TypeCounter/TypeCounter.h"

#include <array> /* std::array */

namespace ECS
{
  class ISystem
  {
  public:
    virtual ~ISystem() = default;
  };

  template<typename DerivedSystem>
  class System : public ISystem
  {
  public:
    System();
    virtual ~System() = default;

    void AddEntity(const Entity id) noexcept;
    void RemoveEntity(const Entity id) noexcept;

    static __forceinline auto GetSystemID() noexcept { return SystemID; }

  protected:
    inline static const SystemID SystemID{ TypeCounter<ISystem>::Get<DerivedSystem>() };

    std::array<Entity, MaxEntities> Entities;
  };
}
```

```cpp
#pragma once
#include "../ECSConstants.h"
#include "../System/System.h"

#include "../TypeList/TypeList.h"

#include <array> /* std::array */

namespace ECS
{
  class SystemManager final
  {
  public:
    ~SystemManager();

    template<typename DerivedSystem>
    DerivedSystem* const AddSystem() noexcept;

    template<typename DerivedSystem>
    DerivedSystem* const GetSystem() noexcept;

    void Update() noexcept;

  private:
    struct SystemInfo final
    {
      SystemInfo()
        : SystemID{ InvalidSystemID }
        , pSystem{}
      {}

      SystemInfo(const SystemID& id, ISystem* _pSystem)
        : SystemID{ id }
        , pSystem{ _pSystem }
      {}

      SystemID SystemID;
      ISystem* pSystem;
    };

    std::array<SystemInfo, MaxSystems> Systems;

    using SystemTypes = typelist::tlist<>;

    template<size_t Index>
    void ExecuteSystems();

    template<size_t ... Indices>
    void ExecuteSystems(std::index_sequence<Indices...>);
  };
}
```
