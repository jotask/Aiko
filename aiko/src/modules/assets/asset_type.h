#pragma once

namespace aiko::asset
{

    using ID = unsigned long long;

    class Asset
    {
    private:
        ID m_id;
        static ID generateID();
    public:
        Asset();
        ~Asset() = default;
        ID getID() const { return m_id; }
    };

    class TextAsset : public Asset
    {
    private:
        const char* data;
    public:
        TextAsset(const char* d) : Asset(), data(d) {} // Correctly initializing `data`
        ~TextAsset() = default;
        const char* getData() const { return data; } // Getter for `data`
    };

    class TextureAsset : public Asset
    {
    public:
        TextureAsset() = default;
        ~TextureAsset() = default;
    };

}
