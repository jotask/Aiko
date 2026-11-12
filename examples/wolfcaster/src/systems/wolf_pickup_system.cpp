#include "wolf_pickup_system.h"

#include "map/wolf_map.h"
#include "math/math.h"
#include "objects/wolf_player.h"

namespace wc
{
    void WolfPickupSystem::updateMapPickups(WolfMap& map, WolfPlayer& player)
    {
        constexpr float ObjectRadius = 0.2f;
        const std::vector<WolfWorldObject>& objects = map.getObjects();
        std::vector<size_t> objectsToRemove;
        for (size_t i = 0 ; i < objects.size(); ++i)
        {
            const WolfWorldObject& obj = objects[i];
            const float distance = aiko::math::distance(player.position(), obj.position);
            if (distance < WolfPlayer::PlayerRadius + ObjectRadius)
            {
                bool isObjectConsumed = false;
                switch (obj.mapObjectId)
                {
                    case 29: if (player.heal(4)) isObjectConsumed = true; break; // Dog food
                    case 47: if (player.heal(10)) isObjectConsumed = true; break; // Food
                    case 48: if (player.heal(25)) isObjectConsumed = true; break; // First Aid
                    case 49: if (player.addAmmo(8)) isObjectConsumed = true; break; // Ammo Clip
                    case 52: player.addScore(100); isObjectConsumed = true; break;// Cross
                    case 53: player.addScore(500); isObjectConsumed = true; break;// Chalice
                    case 54: player.addScore(1000); isObjectConsumed = true; break;// Treasure Chest
                    case 55: player.addScore(5000); isObjectConsumed = true; break;// Crown
                }
                if (isObjectConsumed == true)
                {
                    objectsToRemove.push_back(i);
                }
            }
        }
        for (auto it = objectsToRemove.rbegin(); it != objectsToRemove.rend(); ++it)
        {
            map.removeObject(*it);
        }
    }
}
