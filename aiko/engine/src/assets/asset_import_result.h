#pragma once

#include <aiko_macros.h>
#include <logger/logger.h>

#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace aiko
{
    enum class AssetImportError
    {
        None,
        FileNotFound,
        DecodeFailed,
        InvalidData
    };

    template<typename T>
    class [[nodiscard]] AssetImportResult final
    {
    public:
        AssetImportResult(const AssetImportResult&) = delete;
        AssetImportResult& operator=(const AssetImportResult&) = delete;

        AssetImportResult(AssetImportResult&&) = default;
        AssetImportResult& operator=(AssetImportResult&&) = default;

        static AssetImportResult success(T asset)
        {
            AssetImportResult result;
            result.m_asset.emplace(std::move(asset));
            return result;
        }

        static AssetImportResult failure(AssetImportError error, std::string message)
        {
            AIKO_ASSERT(error != AssetImportError::None, "Failed asset import must have an error code");

            AssetImportResult result;
            result.m_error = error;
            result.m_message = std::move(message);

            logger::Log::errorFmt("Asset import failed: {}", result.m_message);

            return result;
        }

        explicit operator bool() const noexcept
        {
            return m_asset.has_value();
        }

        bool succeeded() const noexcept
        {
            return m_asset.has_value();
        }

        AssetImportError error() const noexcept
        {
            return m_error;
        }

        std::string_view message() const noexcept
        {
            return m_message;
        }

        T& operator*() &
        {
            AIKO_ASSERT(succeeded(), "Asset import failed");
            return *m_asset;
        }

        const T& operator*() const &
        {
            AIKO_ASSERT(succeeded(), "Asset import failed");
            return *m_asset;
        }

        T* operator->()
        {
            return &**this;
        }

        const T* operator->() const
        {
            return &**this;
        }

    private:
        AssetImportResult() = default;

        std::optional<T> m_asset;
        AssetImportError m_error = AssetImportError::None;
        std::string m_message;
    };
}
