#pragma once

#include "assets/asset_id.h"

#include <aiko_types.h>

#include <utility>

namespace aiko
{
    enum class AssetReferenceState
    {
        Empty,
        Requested,
        Loading,
        Ready,
        Failed
    };

    template<typename T>
    class AssetReference
    {
    public:

        void request(string source)
        {
            AIKO_ASSERT(source.empty() == false, "Attempting to request an empty asset source");
            m_source = std::move(source);
            m_id = InvalidAssetId;
            m_state = AssetReferenceState::Requested;
        }

        void set(const AssetId& id)
        {
            m_source.clear();
            m_id = id;
            m_state = id != InvalidAssetId
                ? AssetReferenceState::Ready
                : AssetReferenceState::Empty;
        }

        const string& source() const
        {
            return m_source;
        }

        const AssetId& id() const
        {
            return m_id;
        }

        AssetReferenceState state() const
        {
            return m_state;
        }

        bool isRequested() const
        {
            return m_state == AssetReferenceState::Requested;
        }

        bool isLoading() const
        {
            return m_state == AssetReferenceState::Loading;
        }

        bool isReady() const
        {
            return m_state == AssetReferenceState::Ready;
        }

        bool hasFailed() const
        {
            return m_state == AssetReferenceState::Failed;
        }

        void markLoading()
        {
            AIKO_ASSERT(m_state == AssetReferenceState::Requested, "Asset reference must be requested before loading");
            m_state = AssetReferenceState::Loading;
        }

        void resolve(const AssetId& id)
        {
            AIKO_ASSERT(id != InvalidAssetId, "Cannot resolve invalid asset id");
            m_id = id;
            m_state = AssetReferenceState::Ready;
        }

        void fail()
        {
            m_id = InvalidAssetId;
            m_state = AssetReferenceState::Failed;
        }

        void reset()
        {
            m_source.clear();
            m_id = InvalidAssetId;
            m_state = AssetReferenceState::Empty;
        }

    private:

        string m_source;
        AssetId m_id = InvalidAssetId;
        AssetReferenceState m_state = AssetReferenceState::Empty;
    };
}