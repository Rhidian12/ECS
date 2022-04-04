#include "ComponentManager.h"

namespace ECS
{
    ComponentManager::ComponentManager()
    {
        Components.reserve(MaxComponentTypes);
        Components.resize(MaxComponentTypes);

        // for (ComponentKey& key : Components)
        // {
        //     key.Components.reserve(MaxEntities);
        //     key.Components.resize(MaxEntities);
        // }
    }
}