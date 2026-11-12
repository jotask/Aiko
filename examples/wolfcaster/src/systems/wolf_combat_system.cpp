#include "wolf_combat_system.h"

#include "map/wolf_map.h"
#include "objects/wolf_enemy.h"
#include "objects/wolf_player.h"
#include "objects/wolf_weapon_stats.h"
#include "raycasting/wolf_hitscan.h"

#include <array>
#include <vector>
#include <algorithm>

namespace wc
{

    namespace
    {
        constexpr int32_t EnemyFireArm = 10;
        constexpr int32_t DogBite = 15;
    }

    void WolfCombatSystem::update(WolfMap& map, WolfPlayer& player)
    {
        resolvePlayerShot(map, player);
        resolveEnemyShots(map, player);
    }

    void WolfCombatSystem::resolvePlayerShot(WolfMap& map, WolfPlayer& player)
    {
        if (player.shotsThisFrame() == 0)
        {
            return;
        }

        std::vector<WolfHitscanTarget> targets;
        for (size_t i = 0 ; i < map.getEnemies().size(); ++i)
        {
            const WolfEnemy& enemy = map.getEnemies()[i];
            if (enemy.isAlive() == false)
            {
                continue;
            }
            targets.push_back(WolfHitscanTarget{
                .position = enemy.position(),
                .radius = WolfEnemy::EnemyRadius,
                .id = i
                });
        }

        const WolfWeaponStat& weaponStat = getWolfWeaponStats(player.getWeapon());

        for (size_t shot = 0; shot < player.shotsThisFrame(); ++shot)
        {
            const WolfHitscanHit hit = WolfHitscan::trace(map, player.position(), player.forward(), weaponStat.maxRange, targets);
            if (hit.kind == WolfHitscanHitKind::Actor)
            {
                WolfEnemy& enemy = map.getEnemy(hit.targetId);
                enemy.takeDamage(weaponStat.damage);
                aiko::logger::Log::infoFmt("Enemy {} hit with {} damage by player", hit.targetId, weaponStat.damage);
                if (enemy.isAlive() == false)
                {
                    std::erase_if(targets, [&](const WolfHitscanTarget& target){ return target.id == hit.targetId; });
                }
            }
        }

    }

    void WolfCombatSystem::resolveEnemyShots(WolfMap& map, WolfPlayer& player)
    {
        const std::array<WolfHitscanTarget, 1> targets = {{
            {
                .position = player.position(),
                .radius = WolfPlayer::PlayerRadius,
                .id = 0,
            }
        }};

        for (const WolfEnemy& enemy : map.getEnemies())
        {
            if ( enemy.isAlive() == false || enemy.shotThisFrame() == false)
            {
                continue;
            }

            const float maxDistance = enemy.type() == WolfEnemyType::Dog ? WolfEnemy::AttackRangeDog : WolfEnemy::AttackRangeEnemy;
            const aiko::vec2 shotDir = player.position() - enemy.position();

            const WolfHitscanHit hit = WolfHitscan::trace(map, enemy.position(), shotDir, maxDistance, targets);
            if (hit.kind == WolfHitscanHitKind::Actor)
            {
                const int32_t damage = enemy.type() == WolfEnemyType::Dog ? DogBite : EnemyFireArm;
                player.takeDamage(damage);
                aiko::logger::Log::infoFmt("Player hit by enemy");
            }
        }
    }

}