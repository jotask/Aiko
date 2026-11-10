# WolfCaster Roadmap

Updated: 2026-11-10  
Source of truth: `jotask/Aiko`, branch `dev/trunk`  
Scope: the original Wolfenstein 3D data currently loaded from `*.WL1` files.

## Goal

Turn the working Wolfenstein-style raycaster example into a small, coherent, playable level while keeping binary decoding, runtime map state, player/gameplay rules, raycasting, and UI rendering separated. Prefer complete milestones and tests over speculative refactors.

## Implemented (source inspected)

- [x] MAPHEAD/GAMEMAPS level loading, Carmack decompression, RLEW decompression, and level metadata.
- [x] VSWAP wall and sprite decoding; audio assets and manual sound playback.
- [x] DDA wall raycasting, textured walls, directional shading, and widescreen-aware projection.
- [x] Billboard sprites with depth sorting and wall-column occlusion.
- [x] Player movement, independent-axis sliding, and wall collision.
- [x] Doors: discovery, type/orientation metadata, open/close animation, ray rendering, and collision.
- [x] Object-plane static object creation, per-ID blocking classification, and circular static-object collision.
- [x] Debug minimap, level selection, load/restart, and texture/sound inspection UI.

The checklist above means present in source, not fully runtime-validated. In particular, door types are recognized but locked/elevator-specific gameplay rules are still missing.

## Immediate checkpoint — validate the newly completed world systems

- [ ] Test blocking barrels/furniture against the player collider and sliding in both axes.
- [ ] Verify non-blocking decoration remains passable.
- [ ] Retest walls, partially opening/closing doors, and door passage near edges.
- [ ] Retest sprite occlusion, screen edges, and wall proportions at multiple aspect ratios.
- [ ] Retest level switching/restart with object state and map texture refreshed.

**Done when:** you can navigate multiple levels without passing through blocking scenery or introducing wall/door/rendering regressions.

## Milestone 1 — pickups and mutable world objects (NEXT)

**Why:** all recognized static object IDs are currently added as world objects, rendered, and optionally marked as blocking. Pickups need to disappear and change gameplay state when collected.

- [ ] Classify relevant object IDs by gameplay role: decoration/blocker vs collectible (start with keys).
- [ ] Keep immutable source identity (map ID/sprite mapping) separate from mutable runtime state (collected/active).
- [ ] Introduce minimal player inventory for held keys, with a simple visual/debug readout.
- [ ] Implement proximity-based pickup collection: recognize active collectible, update player state, deactivate it, and stop rendering it.
- [ ] Keep collisions, rendering, and collection aligned with the same active state.
- [ ] Validate that pickup state resets on level restart/load and cannot be collected twice.

**Likely files:** `src/map/wolf_map_types.h`, `src/map/wolf_map_loader.cpp`, `src/map/wolf_map.h/.cpp`, `src/objects/wolf_player.h/.cpp`, `src/raycasting/wolf_raycast_renderer.cpp`, `src/wolfcaster.cpp`. Add a dedicated pickup/world-object system only if needed by growing behavior.

**Done when:** the player can collect a key, the key disappears, inventory updates once, and restart restores it.

## Milestone 2 — locked doors, feedback, and key usage

- [ ] Make `WolfDoor::requiredKeyIndex` an enforced requirement rather than metadata only.
- [ ] Pass player key ownership to the interaction decision without coupling map decoding to the player.
- [ ] Distinguish successful open/close from denied interaction; show a debug message or play a sound.
- [ ] Verify that normal doors remain freely usable, locked doors require the correct key, and existing raycast/collision movement remains consistent.
- [ ] Verify behavior while doors are already opening, closing, or open.

**Likely files:** `src/map/wolf_map.h/.cpp`, `src/map/wolf_door_system.h/.cpp`, `src/objects/wolf_player.h/.cpp`, `src/wolfcaster.cpp`.

**Done when:** retrieving the right key allows passage through its corresponding locked door; other locked doors still deny access.

## Milestone 3 — level exits and progression

- [ ] Define what constitutes an exit and which map data specifies it; keep this separate from ordinary door toggling.
- [ ] Handle elevator/exit interactions and request level transitions through `WolfCaster`.
- [ ] Define what persists (if anything) across levels versus restart.
- [ ] Provide minimal completion feedback and prevent invalid level transitions.
- [ ] Retest map/player/world-object resets when changing levels.

**Done when:** one level can be played from spawn to its exit and advance to the next available level.

## Milestone 4 — remaining world interactions and polish

- [ ] Add health/ammo/treasure/food pickups and associated player state, reusing the pickup path from Milestone 1.
- [ ] Add secret/push-wall interactions only after validating the relevant original map semantics; ensure geometry, raycasting, collision, and minimap see the same changes.
- [ ] Connect already decoded audio to door, pickup, and interaction events.
- [ ] Improve minimal HUD and useful debug feedback; avoid building a full UI framework.
- [ ] Profile if the per-column UI rendering or linear collision/object passes become bottlenecks.

## Later — full gameplay, intentionally deferred

- [ ] Parse actor/enemy placements and directional actor sprites.
- [ ] Enemy state machines, movement, perception, collision, and AI.
- [ ] Player weapons, firing/hitscan, damage, ammo, death, and feedback.
- [ ] Score/health HUD, level completion, and additional audio/game flow.
- [ ] Automated tests for decoders, map IDs, collision edges, and state transitions.

## Architecture guardrails

- `WolfMapLoader`: binary interpretation and initial world metadata. No runtime movement or gameplay decisions.
- `WolfMap`: level data, mutable world-object state, door system, collision/world queries.
- `WolfPlayer`: position, movement, and player-owned gameplay state such as keys.
- `WolfRaycaster`: visibility and interaction raycasts, consistent with mutable geometry/doors.
- `WolfRaycastRenderer`: presentation only; no collection or gameplay mutation.
- `WolfCaster`: input, high-level interaction orchestration, level transitions, and UI integration.
- Keep the initial implementation small. Avoid a full ECS/physics rewrite, spatial indexing, or premature abstractions.

## Next implementation handoff

Start **Milestone 1** with one narrow vertical slice: **collect one key type, hide its sprite, and reflect its ownership in debug UI**. Once that works, expand other key IDs and implement locked-door authorization in Milestone 2.
