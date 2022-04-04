#include "ComponentManager.h"

namespace ECS
{
    ComponentManager::ComponentManager()
    {
        Components.reserve(MaxComponentTypes);
        Components.resize(MaxComponentTypes);
    }
}