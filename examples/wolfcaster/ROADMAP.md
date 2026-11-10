# WolfCaster Roadmap

Updated: 2026-11-10  
Repository: `jotask/Aiko` · branch `dev/trunk`  
Current format: original Wolfenstein 3D `*.WL1` assets.

## Goal

Develop WolfCaster into a playable Wolfenstein-style game while keeping the original binary-format interpretation, runtime simulation, raycasting, rendering, and top-level orchestration separate. Implement and validate one vertical slice at a time.

## History and reprioritization

The roadmap agreed in the November 9 discussion was:

1. World sprites/objects.
2. Interactions (pickups, keys, secret walls).
3. Multiple levels.
4. Enemies/AI.
5. Weapons/combat.
6. HUD/sound/music.
7. Full Wolfenstein gameplay.
8. Optional rendering experiments.

The original enemy implementation order was **object-plane enemy spawns → directional/animated sprites → perception/LOS → idle/patrol/alert/chase/attack/pain/death state machine → movement → combat/audio**.

**Decision (November 10): bring enemies forward.** Wall/sprite rendering, world navigation, and static-object collision now provide the foundations. Keys and level progression remain important but do not block the first enemy milestone. This changes *priority*, not the intended feature set.

## Already implemented (inspected source; not a runtime test certification)

- [x] MAPHEAD/GAMEMAPS, Carmack/RLEW decoding, metadata, level selector/restart.
- [x] VSWAP wall textures, sprite decode, and audio decoding/manual playback.
- [x] DDA walls, textured rendering, widescreen projection, depth-buffered billboard rendering and sprite occlusion.
- [x] Player movement, rotation, independent-axis sliding, and wall collision.
- [x] Door recognition, orientation, partial opening/closing animation, raycasting and player collision.
- [x] Object-plane static scenery (IDs 23–70), blocking classification and static-object circle collision.
- [x] Debug minimap, level picker, texture viewers, and sound player.

Important gaps: enemies are not parsed; player inventory, pickup state, locked-door authorization, exits/level progression, gameplay sound events, weapon/damage logic, and enemy AI are not implemented. Locked and elevator doors are *typed*, but the corresponding special gameplay rules are incomplete.

## Immediate quality checkpoint (before or alongside enemies)

- [ ] Validate the newly completed barrel/table/object collision against player movement; confirm non-blocking scenery remains passable.
- [ ] Recheck wall/door collision and sliding, including doors mid-transition.
- [ ] Check sprite occlusion and reloading multiple levels.
- [ ] Add focused tests around map-ID classification and collision when practical.

## Milestone 1 — enemy foundation (NEXT)

**Objective:** spawn and display *one guard type* using map data; ensure it is represented as an enemy, not a static decoration.

- [ ] Inspect the original game actor object IDs, orientation/difficulty variants, and the sprite-frame layout in the exact `WL1` data; document the verified mapping. Do not guess enemy sprite indices from `objectId - 23`.
- [ ] Parse enemy spawns from the *Objects* plane, separate from existing static objects and player spawns.
- [ ] Represent immutable spawn/type/orientation separately from mutable enemy runtime state (position, facing, alive/active and animation state).
- [ ] Render one stationary guard using the existing projected billboard pipeline and depth occlusion. Choose the correct view-dependent directional sprite.
- [ ] Verify front/back/side views, positioning, sorting behind scenery, and level reload/reset.

**Likely files:** `src/map/wolf_map_types.h`, `src/map/wolf_map_loader.h/.cpp`, `src/map/wolf_map.h/.cpp`, `src/objects/wolf_enemy.h` (new), `src/raycasting/wolf_raycast_renderer.h/.cpp`, and a small shared sprite-projection helper only if duplication warrants it.

**Done when:** a correctly oriented guard appears at a real map spawn, rotates visually with the player's viewing angle, is hidden by walls/doors, and resets on level reload. No AI or weapons required for this milestone.

## Milestone 2 — enemy perception and AI state machine

- [ ] Introduce `WolfEnemySystem` (or equivalent focused owner) responsible for updating enemies, keeping simulation out of `WolfRaycastRenderer`.
- [ ] Implement distance/FOV sensing and a world-aware LOS query respecting walls and *current* door openness. Note: today's interaction raycast is meant to hit doors even if visually open, so do not reuse its semantics blindly for AI sight.
- [ ] Add explicit enemy states, beginning with `Idle`, `Alert`, and `Chase`. Extend to `Patrol`, `Attack`, `Pain`, and `Death` only as behavior needs them.
- [ ] Add simple chasing/movement using world geometry/door collision checks (not raw free movement); prevent going through walls and closed doors.
- [ ] Track enemy frame animations and facing independently from player-facing rendering.
- [ ] Define enemy/player collision behavior and test stationary, moving, occluded, and reloaded enemies.

**Likely files:** `src/objects/wolf_enemy.h` (possibly `.cpp`), `src/objects/wolf_enemy_system.h/.cpp` (new), `src/map/wolf_map.h/.cpp`, `src/raycasting/wolf_raycaster.h/.cpp`, `src/wolfcaster.cpp`.

**Done when:** a guard can detect a visible nearby player, enter an alerted state, chase without crossing solid geometry, and stop seeing through walls.

## Milestone 3 — first combat loop

- [ ] Add minimal player health, ammo, and a single pistol (knife optional later).
- [ ] Implement player hitscan targeting against enemies, respecting closer blocking walls/doors, and apply damage.
- [ ] Implement enemy pain/death states and visual frames; dead enemies no longer chase or block active actors.
- [ ] Implement one enemy attack and player damage; gate attacks by range/LOS and cadence.
- [ ] Show basic health/ammo/enemy debug state; connect existing sound assets to firing, pain, and death.
- [ ] Only then extend to knife, machine gun, chaingun, ammo pickups, weapon switching, and richer animations.

**Done when:** one guard can notice, pursue, attack, take pistol damage, and die in a repeatable gameplay loop.

## Milestone 4 — world interactions and locked doors (deferred, not dropped)

- [ ] Classify pickups (start with keys) separately from blocking scenery; store mutable active/collected state.
- [ ] Collect on proximity, update player inventory, hide/deactivate collected sprites, and restore on restart.
- [ ] Enforce `WolfDoor::requiredKeyIndex` when interacting with locked doors; leave ordinary door behavior unchanged.
- [ ] Add health, ammo, food, treasure, and weapon pickups after the first key slice.
- [ ] Add secrets/push-walls based on verified map semantics; keep collision, rendering, and raycasting synchronized.
- [ ] Show minimal feedback for successful/denied interactions.

## Milestone 5 — exits, multiple levels, and game flow

- [ ] Implement elevator/exit activation and request a level transition through `WolfCaster`.
- [ ] Specify what resets or persists on transitions (player state, collected objects, enemy states, doors).
- [ ] Validate completing a level, moving to the next, dying/restarting, and no invalid transitions.
- [ ] Add completion feedback and eventually scoring.

## Milestone 6 — HUD, audio, and polish

- [ ] Minimal gameplay HUD: health, ammo, keys, equipped weapon, score, and feedback.
- [ ] Hook decoded audio into doors, footsteps/alert, pickups, combat, death, and transitions.
- [ ] Add music if desired after basic sound integration is stable.
- [ ] Measure performance before restructuring linear actor collision/LOS passes or per-column UI drawing.
- [ ] Add tests for map decoders, actor-ID mapping, LOS and combat edge cases.

## Optional longer-term scope

- More enemy types, bosses, full animation repertoire, difficulty handling, full Wolfenstein maps/episodes, saves/menus.
- GPU / rendering experiments in Aiko after gameplay is coherent.

## Architecture guardrails

- `WolfMapLoader`: decode source data and recognize static vs actor spawn IDs; no gameplay updates.
- `WolfMap`: world data, geometry/doors, collision and visibility queries; avoid stuffing all enemy behavior into it.
- `WolfEnemy`: individual enemy data and state; `WolfEnemySystem`: AI/simulation ownership if needed.
- `WolfPlayer`: player position/movement and later health/ammo/inventory.
- `WolfRaycaster`: geometry/door ray queries with explicit visibility/interaction semantics.
- `WolfRaycastRenderer`: sprites, walls, and visual presentation; do not mutate AI or pickup state.
- `WolfCaster`: input, orchestration, actor updates, sound triggers, UI, and transitions.
- Avoid premature ECS, physics-engine integration, spatial-indexing, or a broad refactor. Favor small validated slices.

## Next implementation handoff

Begin **Milestone 1, Enemy Foundation**: identify the original guard spawn IDs and 8-direction sprite frames in the asset version, parse guard spawns into a separate structure, and render a *stationary guard* correctly from multiple player viewpoints. Do **not** implement chase or combat until this foundation works.
