# ECS

## Introduction
An Entity-Component-System (ECS) is an alternative way of storing information regarding Components and Entities.
The traditional approach is to have a `GameObject` that has a list of `Components`, in which the `GameObject` acts as a wrapper around the `Component` list and manages all of them. This is a very good approach, but has some drawbacks:
- There are a lot of virtual function calls present, since the `GameObject` (usually) has a pointer to a base `Component` class, on which it calls an `Update` function to all of the derived `Components`.
- The Components are all scattered in memory, because they are defined on the heap, resulting in a lot of cache misses. For example, assume we have 1.000 rocks in our scene, and all of these rocks float up and down in the air. These rocks will need some `RockMovementComponent` that continiously translates the rock it's attached to. This will have to happen a 1.000 times per frame, and every time we need to go through our virtual function call, which means going through a vtable, go fetch the data from memory, which is probably scattered all over the place, since the Components are not stored contiguously.

```cpp
// Keep in mind that this is an example and not the absolute best way of implementing this
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

This is a very Object-Oriented way of working, which is fine, but costs a lot of memory and time.</br>
However, what if we were to use a <i>Data-Oriented</i> way of working?
In our Data-Oriented design, Objects no longer exist.</br>
Let me demonstrate: </br>
In a Object-Oriented design fashion, we could make a class `Ball` that contains the Ball's position, size, colour and velocity. If we want to do anything with it, we need to get the `Ball` object and call its functions,
```cpp
class Ball final
{
public:
  Ball()
    : Position{}
    , Radius{}
    , Colour{}
    , Velocity{}
  {}
  
  void Update(float DeltaTime)
  {
    Position += Velocity * DeltaTime;
  }
  
  void SetPosition(const Vec3& pos) { Position = pos; }
  /* similar setters */
  
  const Vec3& GetPosition() const { return Position; }
  /* similar getters */

private:
  Vec3 Position;
  float Radius;
  RGBColour Colour;
  Vec3 Velocity;
};
```

If we were to make this class in a Data-Oriented design fashion, there would be no concept of a singular `Ball`. There is only a collection of data that we interpret as a `Ball`.
```cpp
struct Ball final
{
  Ball()
    : Position{}
    , Radius{}
    , Colour{}
    , Velocity{}
  {}

  Vec3 Position;
  float Radius;
  RGBColour Colour;
  Vec3 Velocity;
};
```
Although there is still a struct, therefore an object, that represents a `Ball`, it does not contain <i>any</i> functionality. It only contains data about what a `Ball` is. We cannot update this `Ball`, nor is it the purpose of this object to <i>be</i> updated. All of the data is tightly packed together in memory, and we if we store this `Ball` together with all other Balls, we have a contigious array of Balls, resulting in a lot less cache misses when we search for Balls in memory.

This is the basis of an ECS.
Instead of using GameObjects which manage the Components, we have Entities, which are nothing more than an ID (number).
Instead of using Components which hold data and have functionality regarding that data, our Components only hold data and have <i>no</i> functionality.
The final part of the ECS is a `System`.<br>
Systems can be many things and their implementation always differs on the programmer, but the basis of a `System` is that it's the functionality for Components and Entities. Components and Entities are nothing more than data, and a `System` is the functionality.

The reason an ECS is faster than the traditional approach is because of Data Oriented Design (which once again leads into cache misses and hits).
For those unfamiliar with a cache miss and hit, here's an oversimplification: <br>
Whenever the program needs to access data in memory, it searches in the cache for that specific piece of data, if the data is present in the cache, we get a cache hit and we can continue. <br>
However, when the data is *not* in the cache, we get a cache *miss* and the program needs to search the other pieces of memory for the specific data we requested.
This is a slight problem, since the cache is extremely fast, while the rest of the memory is a lot slower.<br>
All of this relates to the `Locality of Reference`.<br>
This means that whenever data is requested from memory, the cache will grab not only that specific piece of data, but also some of the data adjacent in memory to the requested data. It does this in the hope that the adjacent data is requested, and therefore if requested, instantly produces a cache hit, instead of a cache miss and a search through memory.

The way we can follow this Data Oriented Design and pay heed to the Locality of Reference is by making sure that all of our data is stored contiguously in memory.
For those who are unaware of what contiguous storage means: `std::vector`, the standard C-array (`int[]`) and many other containers use contiguous storage, which means that they store all of their data directly next to each other in memory.

### Benchmarks

23/08: ECS is 233.84% slower than EntT and 204.55% slower than GameObject-Component approach

I compared the ECS I wrote to a traditional GameObject - Component system and EntT and the ECS is slightly faster.
The way I tested this is by creating 100'000 entities (and Game Objects) that both do the same thing. They simulate some very simple physics using 3 different components (RigidBody, Transform and Gravity). <br>
I then timed how long it took to run EntT's lambda vs. my ECS's lambda vs. `for (GameObject* pG : GameObjects) { pG->Update(); }` over a duration of 5 seconds. <br>
These benchmarks can be found in the `ECS.cpp` file.

## Benchmarks
Visual Studio 2022, standard Release build on x64, times:

#### Initialisation Times
- Custom ECS Initialisation Time: 32605600 nanoseconds
- EntT Initialisation Time:       14511300 nanoseconds
- GameObject Initialisation Time: 35082000 nanoseconds

#### Update Times
- Custom ECS Update Time: 1610018 nanoseconds
- EntT Update Time:       688503 nanoseconds
- GameObject Update Time: 787093 nanoseconds 

#### Memory Usage:
- Custom ECS Total Memory Usage: 25 MB 
- EntT Total Memory Usage:       Miniscule (Not visible in VS, less than 11 MB)
- GameObject Total Memory Usage: 19 MB
