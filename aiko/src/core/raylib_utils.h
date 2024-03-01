#pragma once

#include "aiko_types.h"
#include "shared/math.h"
#include "core/render_types.h"
#include "core/textures.h"
#include "models/shader.h"
#include "models/camera.h"
#include "core/color.h"

// Last as all above is not dependent of this, this is a temporal include
#include <core/libs.h>

namespace raylib::utils
{

    // This class is temporary for now. As I want someting working then improve later!

    // To Aiko

    static aiko::Color toColor(Color& c)
    {
        return aiko::Color(c.r, c.g, c.b, c.a);
    }

    static aiko::vec2 toVec2(Vector2& v)
    {
        return { v.x, v.y };
    }

    static aiko::vec3 toVec3(Vector3& v)
    {
        return { v.x, v.y, v.z };
    }

    static aiko::texture::Texture toTexture2D(Texture& texture)
    {
        return { texture.id, texture.width, texture.height, texture.mipmaps, texture.format };
    }

    static aiko::texture::RenderTexture2D toRenderTexture2D(RenderTexture2D& texture)
    {
        return { texture.id, toTexture2D( texture.texture ), toTexture2D ( texture.depth )};
    }

    // To Raylib

    static Color toRaylibColor(aiko::Color& ptr)
    {
        // FIXME :: Baaad! But it compiles
        using UC = unsigned char;
        return { (UC)ptr.r, (UC)ptr.g, (UC)ptr.b, (UC)ptr.a };
    }

    static Vector3 toRayliVector3(aiko::vec3& ptr)
    {
        return { ptr.x, ptr.y, ptr.z };
    }

    static Shader toRaylibShader(aiko::shader::Shader& ptr)
    {
        return { ptr.m_id, ptr.m_locs.data() };
    }

    static Texture toRaylibTexture(aiko::texture::Texture& ptr)
    {
        return { ptr.id, ptr.width, ptr.height, ptr.mipmaps, ptr.format };
    }

    static RenderTexture2D toRaylibRenderTexture2D(aiko::texture::RenderTexture2D ptr)
    {
        return { ptr.id, toRaylibTexture(ptr.texture), toRaylibTexture(ptr.depth) };
    }

    static Camera3D toRaylibCamera3D(aiko::Camera& ptr)
    {
        return { toRayliVector3(ptr.position), toRayliVector3(ptr.target), toRayliVector3(ptr.getUp()), ptr.getFOV(), ptr.getCameraType()};
    }

}
