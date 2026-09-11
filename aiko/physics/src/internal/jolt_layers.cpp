#include "jolt_layers.h"

namespace aiko::physics
{
    bool ObjectLayerPairFilterImpl::ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const
    {
        switch (inObject1)
        {
            case Layers::NON_MOVING:                        return inObject2 == Layers::MOVING;
            case Layers::MOVING:                            return true;
            default:
                JPH_ASSERT(false);
                return false;
        }
    }

    BPLayerInterfaceImpl::BPLayerInterfaceImpl()
    {
        m_objectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        m_objectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
    }

    JPH::uint BPLayerInterfaceImpl::GetNumBroadPhaseLayers() const
    {
        return BroadPhaseLayers::NUM_LAYERS;
    }

    JPH::BroadPhaseLayer BPLayerInterfaceImpl::GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const
    {
        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        return m_objectToBroadPhase[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    const char* BPLayerInterfaceImpl::GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const
    {
        switch ((JPH::BroadPhaseLayer::Type)inLayer)
        {
            case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:      return "NON_MOVING";
            case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:          return "MOVING";
            default:
                JPH_ASSERT(false);
                return "INVALID";
        }
    }
#endif

    bool ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const
    {
        switch (inLayer1)
        {
            case Layers::NON_MOVING:                return inLayer2 == BroadPhaseLayers::MOVING;
            case Layers::MOVING:                    return true;
            default:
                JPH_ASSERT(false);
                return false;
        }
    }
}