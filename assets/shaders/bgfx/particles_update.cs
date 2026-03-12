#include "bgfx_compute.sh"

BUFFER_RW(u_pos, vec4, 0);
BUFFER_RW(u_vel, vec4, 1);
BUFFER_RW(u_life, vec4, 2);

uniform vec4 u_params;      // x=dt, y=lifetime, z=startSpeed
uniform vec4 u_emitterPos;  // xyz = emitter world position
uniform vec4 u_spawnWindow; // x=spawnStart, y=spawnCount, z=totalCount
uniform vec4 u_spawnShape;  // x=shapeId
uniform vec4 u_spawnData;   // x=radius, y=boxX, z=boxY, w=boxZ
uniform vec4 u_direction;   // xyz=preferred direction, w=randomness
uniform vec4 u_gravity;     // xyz=gravity
uniform vec4 u_spawnSeed;   // x=spawn seed

float hash1(float n)
{
    return fract(sin(n) * 43758.5453123);
}

vec2 hash2(float n)
{
    return vec2(
        hash1(n * 1.1234 + 0.123),
        hash1(n * 2.3456 + 4.567)
    );
}

vec3 hash3(float n)
{
    return vec3(
        hash1(n * 1.2345 + 0.123),
        hash1(n * 2.3456 + 1.234),
        hash1(n * 3.4567 + 2.345)
    );
}

vec3 safeNormalize(vec3 v)
{
    float len = length(v);
    if (len > 1e-6)
        return v / len;
    return vec3(0.0, 0.0, 0.0);
}

NUM_THREADS(64, 1, 1)
void main()
{
    uint idx = gl_GlobalInvocationID.x;

    uint totalCount = uint(u_spawnWindow.z);
    if (idx >= totalCount)
        return;

    vec4 pos = u_pos[idx];
    vec4 vel = u_vel[idx];
    vec4 life = u_life[idx];

    float dt = u_params.x;
    float lifetime = u_params.y;
    float startSpeed = u_params.z;

    if (life.x > 0.0)
    {
        vel.xyz += u_gravity.xyz * dt;
        pos.xyz += vel.xyz * dt;
        life.x -= dt;
    }
    else
    {
        uint spawnStart = uint(u_spawnWindow.x);
        uint spawnCount = uint(u_spawnWindow.y);
        uint totalCount = uint(u_spawnWindow.z);

        bool shouldSpawn = false;
        if (spawnCount > 0u)
        {
            uint spawnEnd = spawnStart + spawnCount;

            if (spawnEnd <= totalCount)
            {
                shouldSpawn = (idx >= spawnStart && idx < spawnEnd);
            }
            else
            {
                uint wrappedEnd = spawnEnd - totalCount;
                shouldSpawn = (idx >= spawnStart || idx < wrappedEnd);
            }
        }

        if (shouldSpawn)
        {
            float baseSeed = float(idx) + u_spawnSeed.x * 97.13;
            float seed1 = baseSeed * 1.731 + 29.7;
            float seed2 = baseSeed * 2.417 + 53.2;
            float seed3 = baseSeed * 3.113 + 71.9;

            vec3 spawnOffset = vec3(0.0);
            uint shape = uint(u_spawnShape.x);

            if (shape == 0u) // Point
            {
                spawnOffset = vec3(0.0);
            }
            else if (shape == 1u) // Circle (random inside disk)
            {
                vec2 rnd = hash2(seed1);
                float angle = rnd.x * 6.2831853;
                float radius = sqrt(rnd.y) * u_spawnData.x;

                spawnOffset = vec3(cos(angle) * radius, sin(angle) * radius, 0.0);
            }
            else if (shape == 2u) // Box (random inside volume)
            {
                vec3 rnd = hash3(seed1);
                vec3 ext = vec3(u_spawnData.y, u_spawnData.z, u_spawnData.w);

                spawnOffset = vec3(
                    (rnd.x * 2.0 - 1.0) * ext.x,
                    (rnd.y * 2.0 - 1.0) * ext.y,
                    (rnd.z * 2.0 - 1.0) * ext.z
                );
            }
            else if (shape == 3u) // Sphere (random inside volume)
            {
                vec3 rndDir = hash3(seed1) * 2.0 - 1.0;
                rndDir = safeNormalize(rndDir);

                float radius = pow(hash1(seed2), 1.0 / 3.0) * u_spawnData.x;
                spawnOffset = rndDir * radius;
            }

            vec3 baseDir = safeNormalize(u_direction.xyz);
            if (length(baseDir) < 1e-6)
                baseDir = vec3(0.0, 1.0, 0.0);

            vec3 randDir = safeNormalize(hash3(seed2) * 2.0 - 1.0);
            if (length(randDir) < 1e-6)
                randDir = vec3(0.0, 1.0, 0.0);

            vec3 outwardDir = safeNormalize(spawnOffset);
            if (length(outwardDir) < 1e-6)
                outwardDir = baseDir;

            float outwardWeight = 0.0;
            if (shape == 0u)      // Point
            {
                outwardWeight = 0.0;
            }
            else if (shape == 1u) // Circle
            {
                outwardWeight = 0.6;
            }
            else if (shape == 2u) // Box
            {
                outwardWeight = 0.4;
            }
            else if (shape == 3u) // Sphere
            {
                outwardWeight = 1.0;
            }

            vec3 shapedBaseDir = safeNormalize(mix(baseDir, outwardDir, outwardWeight));
            if (length(shapedBaseDir) < 1e-6)
                shapedBaseDir = baseDir;

            float randomness = clamp(u_direction.w, 0.0, 1.0);
            vec3 dir = safeNormalize(mix(shapedBaseDir, randDir, randomness));
            if (length(dir) < 1e-6)
                dir = baseDir;

            float speedJitter = mix(0.8, 1.2, hash1(seed3));

            pos.xyz = u_emitterPos.xyz + spawnOffset;
            vel.xyz = dir * (startSpeed * speedJitter);
            life.x = lifetime;
            life.y = lifetime;
        }
        else
        {
            pos.xyz = vec3(100000.0, 100000.0, 100000.0);
        }
    }

    u_pos[idx] = pos;
    u_vel[idx] = vel;
    u_life[idx] = life;
}