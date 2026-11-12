#pragma once

#include "map/wolf_map_types.h"

#include <magic_enum/magic_enum.hpp>

#include <array>
#include <optional>

namespace wc
{
    struct WolfWeaponStat
    {
        int32_t damage = 0;
        float maxRange = 0.0f;
        float fireIntervalSeconds = 0.0f;
        bool automatic = false;
        int32_t ammoCost = 0;
    };

    inline constexpr std::array<WolfWeaponStat, magic_enum::enum_count<WolfWeapon>()> WeaponsStats =
    {
        WolfWeaponStat{.damage = 25, .maxRange =  1.5f, .fireIntervalSeconds = 0.45f,  .automatic = true,  .ammoCost = 0, },
        WolfWeaponStat{.damage = 20, .maxRange = 20.0f, .fireIntervalSeconds = 0.34f,  .automatic = false, .ammoCost = 1, },
        WolfWeaponStat{.damage = 16, .maxRange = 20.0f, .fireIntervalSeconds = 0.17f,  .automatic = true,  .ammoCost = 1, },
        WolfWeaponStat{.damage = 12, .maxRange = 20.0f, .fireIntervalSeconds = 0.086f, .automatic = true,  .ammoCost = 1, },
    };

    inline const WolfWeaponStat& getWolfWeaponStats(WolfWeapon weapon)
    {
        const std::optional<size_t> index = magic_enum::enum_index(weapon);
        AIKO_ASSERT(index != std::nullopt, "Wolf Weapon State OOB");
        return WeaponsStats[index.value()];
    }

}