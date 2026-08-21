#pragma once

#include <aiko_types.h>
#include <assets/types/mesh_asset.h>

#include "types/render_types.h"

namespace aiko
{
    class Mesh;
    namespace interfaces
    {
        struct IMeshImpl
        {
            IMeshImpl(const Mesh* mesh)
                : m_mesh(mesh)
            {

            }

            virtual ~IMeshImpl() = default;

            virtual bool isValid() const = 0;

            // load
            virtual void unload() = 0;

            // modify
            virtual void refresh(const MeshAsset& asset) = 0;

            virtual RenderResourceId id() = 0;

        protected:
            Mesh const * m_mesh;

        };
    }
}
